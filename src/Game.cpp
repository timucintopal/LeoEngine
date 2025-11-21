#include "Game.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cmath>

Game::Game()
    : isRunning(false), window(nullptr), glContext(nullptr),
      playerTexture(nullptr), circleIndexCount(0),
      m_Camera(nullptr), m_ScreenWidth(800), m_ScreenHeight(600),
      m_BackgroundMusic(nullptr), m_JumpSound(nullptr), 
      m_CollisionSound(nullptr), m_WasColliding(false) {}

Game::~Game() {
  // Clean() should be called before destructor, but just in case:
  if (playerTexture) {
    delete playerTexture;
    playerTexture = nullptr;
  }
}

void Game::Init(const char *title, int width, int height, bool fullscreen) {
  m_ScreenWidth = width;
  m_ScreenHeight = height;
  
  int flags = 0;
  if (fullscreen) {
    flags = SDL_WINDOW_FULLSCREEN;
  }

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == 0) {
    std::cout << "Subsystems Initialized!..." << std::endl;
    
    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
      std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
    } else {
      std::cout << "SDL_mixer initialized!" << std::endl;
    }

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

    // Initialize Camera
    glm::vec2 initialCameraPos = glm::vec2(400.0f, 300.0f);
    m_Camera = new Camera(initialCameraPos, m_ScreenWidth, m_ScreenHeight);

    // Create GameObjects
    // Player object (first in vector)
    GameObject* player = new GameObject(glm::vec2(400.0f, 300.0f), 
                                        glm::vec2(100.0f, 100.0f), 
                                        playerTexture);
    m_GameObjects.push_back(player);

    // Static reference object (red circle) - second in vector
    // For the circle, we'll use a null texture and useColor=true
    GameObject* referencePoint = new GameObject(glm::vec2(400.0f, 300.0f),
                                                glm::vec2(50.0f, 50.0f),
                                                nullptr);
    m_GameObjects.push_back(referencePoint);

    // Wall object (immobile) - third in vector
    // This will be used for collision testing
    GameObject* wall = new GameObject(glm::vec2(600.0f, 200.0f),
                                      glm::vec2(150.0f, 100.0f),
                                      nullptr);
    m_GameObjects.push_back(wall);

    // Load background music
    m_BackgroundMusic = Mix_LoadMUS("assets/background.ogg");
    if (m_BackgroundMusic == nullptr) {
      std::cerr << "Failed to load background music! SDL_mixer Error: " << Mix_GetError() << std::endl;
    } else {
      // Play background music looping
      if (Mix_PlayMusic(m_BackgroundMusic, -1) == -1) {
        std::cerr << "Failed to play background music! SDL_mixer Error: " << Mix_GetError() << std::endl;
      } else {
        std::cout << "Background music playing!" << std::endl;
      }
    }

    // Load jump sound effect
    m_JumpSound = Mix_LoadWAV("assets/jump.wav");
    if (m_JumpSound == nullptr) {
      std::cerr << "Failed to load jump sound! SDL_mixer Error: " << Mix_GetError() << std::endl;
    } else {
      std::cout << "Jump sound loaded!" << std::endl;
    }

    // Load collision sound effect
    m_CollisionSound = Mix_LoadWAV("assets/Audio/collision.wav");
    if (m_CollisionSound == nullptr) {
      std::cerr << "Failed to load collision sound! SDL_mixer Error: " << Mix_GetError() << std::endl;
    } else {
      std::cout << "Collision sound loaded!" << std::endl;
    }

    isRunning = true;
  } else {
    std::cerr << "SDL Init failed: " << SDL_GetError() << std::endl;
    isRunning = false;
  }
}

void Game::HandleEvents() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      isRunning = false;
      break;
    case SDL_KEYDOWN:
      if (event.key.keysym.sym == SDLK_w) {
        // Play jump sound effect
        if (m_JumpSound != nullptr) {
          Mix_PlayChannel(-1, m_JumpSound, 0);
        }
      }
      break;
    default:
      break;
    }
  }
}

void Game::Update(float deltaTime) {
  const Uint8 *state = SDL_GetKeyboardState(NULL);
  float speed = 300.0f;

  // Update first object (player) position
  if (m_GameObjects.size() > 0) {
    GameObject* player = m_GameObjects[0];
    
    // Calculate next potential position
    glm::vec2 nextPosition = player->position;
    
    if (state[SDL_SCANCODE_W]) {
      // Move up (decrease Y)
      nextPosition.y -= speed * deltaTime;
    }
    if (state[SDL_SCANCODE_S]) {
      // Move down (increase Y)
      nextPosition.y += speed * deltaTime;
    }
    if (state[SDL_SCANCODE_A]) {
      // Move left (decrease X)
      nextPosition.x -= speed * deltaTime;
    }
    if (state[SDL_SCANCODE_D]) {
      // Move right (increase X)
      nextPosition.x += speed * deltaTime;
    }
    
    // Check collision with potential new position
    // Create temporary GameObject for collision check
    GameObject tempPlayer = *player;
    tempPlayer.position = nextPosition;
    
    // Check collision with all other objects (skip player itself and reference point)
    // Reference point (index 1) is just visual, skip it in collision detection
    bool canMoveX = true;
    bool canMoveY = true;
    bool isColliding = false;
    
    for (size_t i = 2; i < m_GameObjects.size(); i++) {
      GameObject* other = m_GameObjects[i];
      
      // Check X-axis collision
      GameObject tempX = tempPlayer;
      tempX.position.x = nextPosition.x;
      tempX.position.y = player->position.y; // Keep original Y
      if (CollisionManager::CheckCollision(tempX, *other)) {
        canMoveX = false;
        isColliding = true;
      }
      
      // Check Y-axis collision
      GameObject tempY = tempPlayer;
      tempY.position.x = player->position.x; // Keep original X
      tempY.position.y = nextPosition.y;
      if (CollisionManager::CheckCollision(tempY, *other)) {
        canMoveY = false;
        isColliding = true;
      }
    }
    
    // Play collision sound if collision just started (wasn't colliding before)
    if (isColliding && !m_WasColliding && m_CollisionSound != nullptr) {
      Mix_PlayChannel(-1, m_CollisionSound, 0);
    }
    m_WasColliding = isColliding;
    
    // Update position only if no collision
    if (canMoveX) {
      player->position.x = nextPosition.x;
    }
    if (canMoveY) {
      player->position.y = nextPosition.y;
    }
    
    // Camera follows player
    if (m_Camera) {
      m_Camera->Follow(player->position, deltaTime);
    }
  }
}

void Game::Render() {
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  if (!m_Camera) {
    return;
  }

  // Get view and projection matrices from camera
  glm::mat4 view = m_Camera->GetViewMatrix();
  glm::mat4 projection = m_Camera->GetProjectionMatrix(m_ScreenWidth, m_ScreenHeight);

  // Draw all GameObjects
  for (size_t i = 0; i < m_GameObjects.size(); i++) {
    GameObject* obj = m_GameObjects[i];
    
    if (i == 0) {
      // Player object - use texture
      obj->Draw(view, projection, shaderProgram, VAO, 6, false);
    } else if (i == 1) {
      // Reference point - use red color and circle VAO
      obj->Draw(view, projection, shaderProgram, circleVAO, circleIndexCount, 
                true, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    } else if (i == 2) {
      // Wall object - use blue color
      obj->Draw(view, projection, shaderProgram, VAO, 6, 
                true, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
    } else {
      // Other objects - default rendering
      obj->Draw(view, projection, shaderProgram, VAO, 6, false);
    }
  }

  SDL_GL_SwapWindow(window);
}

void Game::Clean() {
  // Clean up GameObjects
  for (GameObject* obj : m_GameObjects) {
    delete obj;
  }
  m_GameObjects.clear();

  if (m_Camera) {
    delete m_Camera;
    m_Camera = nullptr;
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteVertexArrays(1, &circleVAO);
  glDeleteBuffers(1, &circleVBO);
  glDeleteBuffers(1, &circleEBO);
  glDeleteProgram(shaderProgram);

  if (playerTexture) {
    delete playerTexture;
    playerTexture = nullptr;
  }

  // Free sound effects and music
  if (m_JumpSound != nullptr) {
    Mix_FreeChunk(m_JumpSound);
    m_JumpSound = nullptr;
  }
  
  if (m_CollisionSound != nullptr) {
    Mix_FreeChunk(m_CollisionSound);
    m_CollisionSound = nullptr;
  }
  
  if (m_BackgroundMusic != nullptr) {
    Mix_FreeMusic(m_BackgroundMusic);
    m_BackgroundMusic = nullptr;
  }
  
  // Quit SDL_mixer
  Mix_CloseAudio();

  SDL_GL_DeleteContext(glContext);
  SDL_DestroyWindow(window);
  SDL_Quit();
  std::cout << "Game Cleaned" << std::endl;
}

