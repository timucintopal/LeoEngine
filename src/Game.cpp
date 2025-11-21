#include "Game.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cmath>

Game::Game()
    : isRunning(false), window(nullptr), glContext(nullptr),
      playerTexture(nullptr), circleIndexCount(0) {}

Game::~Game() {
  if (playerTexture) {
    delete playerTexture;
  }
}

void Game::Init(const char *title, int width, int height, bool fullscreen) {
  int flags = 0;
  if (fullscreen) {
    flags = SDL_WINDOW_FULLSCREEN;
  }

  if (SDL_Init(SDL_INIT_VIDEO) == 0) {
    std::cout << "Subsystems Initialized!..." << std::endl;

    // Configure OpenGL Attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
#ifdef __APPLE__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                        SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, width, height,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | flags);
    if (window) {
      std::cout << "Window created!" << std::endl;
    } else {
      std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
      return;
    }

    glContext = SDL_GL_CreateContext(window);
    if (glContext) {
      std::cout << "OpenGL Context created!" << std::endl;
    } else {
      std::cerr << "OpenGL Context creation failed: " << SDL_GetError()
                << std::endl;
      return;
    }

    SDL_GL_MakeCurrent(window, glContext);

    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
      std::cerr << "Error initializing GLEW: " << glewGetErrorString(glewError)
                << std::endl;
      return;
    }
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    // --- Shader Compilation ---
    const char *vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "out vec2 TexCoord;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
        "   TexCoord = aTexCoord;\n"
        "}\0";
    const char *fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec2 TexCoord;\n"
        "uniform sampler2D tex;\n"
        "uniform vec4 color;\n"
        "uniform bool useColor;\n"
        "void main()\n"
        "{\n"
        "   if (useColor) {\n"
        "       FragColor = color;\n"
        "   } else {\n"
        "       FragColor = texture(tex, TexCoord);\n"
        "   }\n"
        "}\n\0";

    // Vertex Shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // Check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
      std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                << infoLog << std::endl;
    }

    // Fragment Shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // Check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
      std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                << infoLog << std::endl;
    }

    // Link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
      std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // --- Vertex Data & Buffers ---
    float vertices[] = {
        // positions          // texture coords
        0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // top right
        0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f,  0.0f, 0.0f, 1.0f  // top left
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    // Texture Coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // --- Circle Vertex Data & Buffers (for reference point) ---
    const int circleSegments = 32;
    std::vector<float> circleVertices;
    std::vector<unsigned int> circleIndices;
    
    // Center vertex
    circleVertices.push_back(0.0f); // x
    circleVertices.push_back(0.0f); // y
    circleVertices.push_back(0.0f); // z
    circleVertices.push_back(0.5f); // tex x
    circleVertices.push_back(0.5f); // tex y
    
    // Circle vertices
    for (int i = 0; i <= circleSegments; i++) {
      float angle = 2.0f * 3.14159265359f * i / circleSegments;
      float x = cosf(angle);
      float y = sinf(angle);
      circleVertices.push_back(x);  // x
      circleVertices.push_back(y);  // y
      circleVertices.push_back(0.0f); // z
      circleVertices.push_back(x * 0.5f + 0.5f); // tex x
      circleVertices.push_back(y * 0.5f + 0.5f); // tex y
    }
    
    // Circle indices (triangles from center)
    for (int i = 0; i < circleSegments; i++) {
      circleIndices.push_back(0); // center
      circleIndices.push_back(i + 1);
      circleIndices.push_back(i + 2);
    }
    circleIndexCount = circleIndices.size();
    
    glGenVertexArrays(1, &circleVAO);
    glGenBuffers(1, &circleVBO);
    glGenBuffers(1, &circleEBO);
    
    glBindVertexArray(circleVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glBufferData(GL_ARRAY_BUFFER, circleVertices.size() * sizeof(float),
                 circleVertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, circleEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 circleIndices.size() * sizeof(unsigned int),
                 circleIndices.data(), GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    
    // Texture Coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);

    // Load Texture
    playerTexture = new Texture();
    playerTexture->Load("assets/char.png");

    // Projection Matrix (Ortho)
    glUseProgram(shaderProgram);
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
    unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    playerPosition = glm::vec2(400.0f, 300.0f);
    isRunning = true;
  } else {
    std::cerr << "SDL Init failed: " << SDL_GetError() << std::endl;
    isRunning = false;
  }
}

void Game::HandleEvents() {
  SDL_Event event;
  SDL_PollEvent(&event);
  switch (event.type) {
  case SDL_QUIT:
    isRunning = false;
    break;
  default:
    break;
  }
}

void Game::Update(float deltaTime) {
  const Uint8 *state = SDL_GetKeyboardState(NULL);
  float speed = 300.0f;

  if (state[SDL_SCANCODE_W]) {
    // Move up (decrease Y)
    playerPosition.y -= speed * deltaTime;
  }
  if (state[SDL_SCANCODE_S]) {
    // Move down (increase Y)
    playerPosition.y += speed * deltaTime;
  }
  if (state[SDL_SCANCODE_A]) {
    // Move left (decrease X)
    playerPosition.x -= speed * deltaTime;
  }
  if (state[SDL_SCANCODE_D]) {
    // Move right (increase X)
    playerPosition.x += speed * deltaTime;
  }
}

void Game::Render() {
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw Quad
  glUseProgram(shaderProgram);

  // View Matrix (Identity - no camera movement, or slight follow)
  glm::mat4 view = glm::mat4(1.0f);
  unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

  // Model Matrix for player - position character at playerPosition
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(playerPosition.x, playerPosition.y, 0.0f));
  model = glm::scale(model, glm::vec3(100.0f, 100.0f, 1.0f));
  unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

  playerTexture->Bind();
  // Set useColor to false for texture rendering
  unsigned int useColorLoc = glGetUniformLocation(shaderProgram, "useColor");
  glUniform1i(useColorLoc, 0);
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  // Draw Static Reference Circle (Red)
  glm::mat4 circleModel = glm::mat4(1.0f);
  circleModel = glm::translate(circleModel, glm::vec3(400.0f, 300.0f, 0.0f));
  circleModel = glm::scale(circleModel, glm::vec3(50.0f, 50.0f, 1.0f));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(circleModel));
  
  // Set color to red and useColor to true
  unsigned int colorLoc = glGetUniformLocation(shaderProgram, "color");
  glUniform4f(colorLoc, 1.0f, 0.0f, 0.0f, 1.0f); // Red color
  glUniform1i(useColorLoc, 1);
  
  glBindVertexArray(circleVAO);
  glDrawElements(GL_TRIANGLES, circleIndexCount, GL_UNSIGNED_INT, 0);

  SDL_GL_SwapWindow(window);
}

void Game::Clean() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteVertexArrays(1, &circleVAO);
  glDeleteBuffers(1, &circleVBO);
  glDeleteBuffers(1, &circleEBO);
  glDeleteProgram(shaderProgram);

  if (playerTexture) {
    playerTexture->Cleanup();
  }

  SDL_GL_DeleteContext(glContext);
  SDL_DestroyWindow(window);
  SDL_Quit();
  std::cout << "Game Cleaned" << std::endl;
}
