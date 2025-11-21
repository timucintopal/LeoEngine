#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cmath>
#include <iostream>

Renderer::Renderer()
    : m_ShaderProgram(0), m_VAO(0), m_VBO(0), m_EBO(0),
      m_CircleVAO(0), m_CircleVBO(0), m_CircleEBO(0), m_CircleIndexCount(0) {}

Renderer::~Renderer() {
  Cleanup();
}

bool Renderer::Init() {
  if (!CompileShaders()) {
    return false;
  }
  CreateQuadBuffers();
  CreateCircleBuffers();
  return true;
}

bool Renderer::CompileShaders() {
  const char *vertexShaderSource =
      "#version 330 core\n"
      "layout (location = 0) in vec3 aPos;\n"
      "layout (location = 1) in vec2 aTexCoord;\n"
      "out vec2 TexCoord;\n"
      "uniform mat4 model;\n"
      "uniform mat4 view;\n"
      "uniform mat4 projection;\n"
      "uniform vec4 textureOffsetScale;\n"
      "void main()\n"
      "{\n"
      "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
      "   TexCoord = (aTexCoord * textureOffsetScale.zw) + textureOffsetScale.xy;\n"
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
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog << std::endl;
    return false;
  }

  // Fragment Shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
              << infoLog << std::endl;
    return false;
  }

  // Link shaders
  m_ShaderProgram = glCreateProgram();
  glAttachShader(m_ShaderProgram, vertexShader);
  glAttachShader(m_ShaderProgram, fragmentShader);
  glLinkProgram(m_ShaderProgram);
  glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(m_ShaderProgram, 512, NULL, infoLog);
    std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
              << infoLog << std::endl;
    return false;
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return true;
}

void Renderer::CreateQuadBuffers() {
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

  glGenVertexArrays(1, &m_VAO);
  glGenBuffers(1, &m_VBO);
  glGenBuffers(1, &m_EBO);

  glBindVertexArray(m_VAO);

  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
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
}

void Renderer::CreateCircleBuffers() {
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
  m_CircleIndexCount = circleIndices.size();
  
  glGenVertexArrays(1, &m_CircleVAO);
  glGenBuffers(1, &m_CircleVBO);
  glGenBuffers(1, &m_CircleEBO);
  
  glBindVertexArray(m_CircleVAO);
  
  glBindBuffer(GL_ARRAY_BUFFER, m_CircleVBO);
  glBufferData(GL_ARRAY_BUFFER, circleVertices.size() * sizeof(float),
               circleVertices.data(), GL_STATIC_DRAW);
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_CircleEBO);
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
}

void Renderer::Cleanup() {
  glDeleteVertexArrays(1, &m_VAO);
  glDeleteBuffers(1, &m_VBO);
  glDeleteBuffers(1, &m_EBO);
  glDeleteVertexArrays(1, &m_CircleVAO);
  glDeleteBuffers(1, &m_CircleVBO);
  glDeleteBuffers(1, &m_CircleEBO);
  glDeleteProgram(m_ShaderProgram);
}

