#include "Game.h"
#include "Animation.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Game::Game()
    : isRunning(false), window(nullptr), glContext(nullptr),
      m_Renderer(nullptr), m_InputManager(nullptr), 
      m_ResourceManager(nullptr), m_Scene(nullptr),
      m_Camera(nullptr), m_ScreenWidth(800), m_ScreenHeight(600),
      m_WasColliding(false) {}

Game::~Game() {
  // Clean() should be called before destructor, but just in case:
  Clean();
}

void Game::Init(const char *title, int width, int height, bool fullscreen) {
  m_ScreenWidth = width;
  m_ScreenHeight = height;
  
  InitSDL();
  InitOpenGL();
  InitResources();
  InitScene();
  
  isRunning = true;
}

void Game::InitSDL() {
  int flags = 0;
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == 0) {
    std::cout << "Subsystems Initialized!..." << std::endl;
    
    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
      std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
    } else {
      std::cout << "SDL_mixer initialized!" << std::endl;
    }
    
    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
      std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
    } else {
      std::cout << "SDL_ttf initialized!" << std::endl;
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

    window = SDL_CreateWindow("Wayne Engine", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, m_ScreenWidth, m_ScreenHeight,
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
    
    // Enable alpha blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  } else {
    std::cerr << "SDL Init failed: " << SDL_GetError() << std::endl;
    isRunning = false;
  }
}

void Game::InitOpenGL() {
  // Initialize Renderer
  m_Renderer = new Renderer();
  if (!m_Renderer->Init()) {
    std::cerr << "Failed to initialize Renderer!" << std::endl;
    isRunning = false;
    return;
  }
  
  // Initialize InputManager
  m_InputManager = new InputManager();
}

void Game::InitResources() {
  // Initialize ResourceManager
  m_ResourceManager = new ResourceManager();
  
  // Load textures
  m_ResourceManager->LoadTexture("player", "assets/Character/knight.png");
  
  // Load sounds
  m_ResourceManager->LoadMusic("background", "assets/background.ogg");
  m_ResourceManager->LoadSound("jump", "assets/jump.wav");
  m_ResourceManager->LoadSound("collision", "assets/Audio/collision.wav");
  
  // Load font
  m_ResourceManager->LoadFont("assets/Font/Roboto-Bold.ttf", 24);
  
  // Play background music
  m_ResourceManager->PlayMusic("background", -1);
}

void Game::InitScene() {
  // Initialize Camera
  glm::vec2 initialCameraPos = glm::vec2(400.0f, 300.0f);
  m_Camera = new Camera(initialCameraPos, m_ScreenWidth, m_ScreenHeight);

  // Initialize Scene
  m_Scene = new Scene();
  
  // Create GameObjects
  // Player object (first in vector)
  Texture* playerTexture = m_ResourceManager->GetTexture("player");
  GameObject* player = new GameObject(glm::vec2(400.0f, 300.0f), 
                                      glm::vec2(100.0f, 100.0f), 
                                      playerTexture);
  
  // Create animation for player using knight sprite sheet
  // Assuming: 8 frames in the first row, each frame is 16x16 pixels
  // Get actual texture dimensions to calculate UV coordinates
  int sheetWidth = playerTexture->GetWidth();
  int sheetHeight = playerTexture->GetHeight();
  
  // Frame dimensions in pixels
  const int frameWidthPx = 16;
  const int frameHeightPx = 16;
  const int numFrames = 8;
  
  // Calculate UV coordinates for each frame
  // textureOffsetScale format: (x_uv, y_uv, width_uv, height_uv) in normalized coordinates [0-1]
  std::vector<glm::vec4> walkFrames;
  float widthUV = static_cast<float>(frameWidthPx) / static_cast<float>(sheetWidth);
  float heightUV = static_cast<float>(frameHeightPx) / static_cast<float>(sheetHeight);
  
  // First row (y = 0), 8 frames horizontally
  for (int i = 0; i < numFrames; i++) {
    float xUV = static_cast<float>(i * frameWidthPx) / static_cast<float>(sheetWidth);
    float yUV = 0.0f; // First row
    walkFrames.push_back(glm::vec4(xUV, yUV, widthUV, heightUV));
  }
  
  Animation* walkAnimation = new Animation(playerTexture, walkFrames, 0.1f);
  player->currentAnimation = walkAnimation;
  
  m_Scene->AddGameObject(player);

  // Static reference object (red circle) - second in vector
  GameObject* referencePoint = new GameObject(glm::vec2(400.0f, 300.0f),
                                              glm::vec2(50.0f, 50.0f),
                                              nullptr);
  m_Scene->AddGameObject(referencePoint);

  // Wall object (immobile) - third in vector
  GameObject* wall = new GameObject(glm::vec2(600.0f, 200.0f),
                                    glm::vec2(150.0f, 100.0f),
                                    nullptr);
  m_Scene->AddGameObject(wall);
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
        if (m_ResourceManager) {
          m_ResourceManager->PlaySound("jump");
        }
      }
      break;
    default:
      break;
    }
  }
  
  // Update input manager
  if (m_InputManager) {
    m_InputManager->Update();
  }
}

void Game::Update(float deltaTime) {
  if (!m_InputManager || !m_Scene) return;
  
  float speed = 300.0f;
  glm::vec2 movement = m_InputManager->GetMovementInput();
  
  // Update player movement with collision detection
  bool wasColliding = m_WasColliding;
  m_Scene->UpdatePlayerMovement(movement, speed, deltaTime, m_WasColliding);
  
  // Update player animation
  if (m_Scene->GetGameObjectCount() > 0) {
    GameObject* player = m_Scene->GetGameObject(0);
    if (player) {
      player->Update(deltaTime);
    }
  }
  
  // Play collision sound if collision just started
  if (m_WasColliding && !wasColliding && m_ResourceManager) {
    m_ResourceManager->PlaySound("collision");
  }
  
  // Camera follows player
  if (m_Camera && m_Scene->GetGameObjectCount() > 0) {
    GameObject* player = m_Scene->GetGameObject(0);
    if (player) {
      m_Camera->Follow(player->position, deltaTime);
    }
  }
}

void Game::Render() {
  if (!m_Renderer || !m_Camera || !m_Scene) {
    return;
  }

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Get view and projection matrices from camera
  glm::mat4 view = m_Camera->GetViewMatrix();
  glm::mat4 projection = m_Camera->GetProjectionMatrix(m_ScreenWidth, m_ScreenHeight);

  GLuint shaderProgram = m_Renderer->GetShaderProgram();
  GLuint VAO = m_Renderer->GetVAO();
  GLuint circleVAO = m_Renderer->GetCircleVAO();
  int circleIndexCount = m_Renderer->GetCircleIndexCount();

  // Draw all GameObjects
  for (size_t i = 0; i < m_Scene->GetGameObjectCount(); i++) {
    GameObject* obj = m_Scene->GetGameObject(i);
    if (!obj) continue;
    
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

  // Render text in screen space (UI elements)
  if (m_ResourceManager && m_ResourceManager->GetTextRenderer()) {
    m_ResourceManager->GetTextRenderer()->RenderText("SCORE: 100", 100, 20, 
                                                      shaderProgram, VAO, 
                                                      m_ScreenWidth, m_ScreenHeight);
  }

  SDL_GL_SwapWindow(window);
}

void Game::Clean() {
  // Clean up Scene
  if (m_Scene) {
    m_Scene->Cleanup();
    delete m_Scene;
    m_Scene = nullptr;
  }

  // Clean up Camera
  if (m_Camera) {
    delete m_Camera;
    m_Camera = nullptr;
  }

  // Clean up ResourceManager (textures, sounds, fonts)
  if (m_ResourceManager) {
    m_ResourceManager->Cleanup();
    delete m_ResourceManager;
    m_ResourceManager = nullptr;
  }

  // Clean up Renderer (shaders, buffers)
  if (m_Renderer) {
    m_Renderer->Cleanup();
    delete m_Renderer;
    m_Renderer = nullptr;
  }

  // Clean up InputManager
  if (m_InputManager) {
    delete m_InputManager;
    m_InputManager = nullptr;
  }

  // Quit SDL_mixer
  Mix_CloseAudio();

  // Quit SDL_ttf
  TTF_Quit();

  // Clean up OpenGL context and window
  if (glContext) {
    SDL_GL_DeleteContext(glContext);
    glContext = nullptr;
  }
  
  if (window) {
    SDL_DestroyWindow(window);
    window = nullptr;
  }
  
  SDL_Quit();
  std::cout << "Game Cleaned" << std::endl;
}
