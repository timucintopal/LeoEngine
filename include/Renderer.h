#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include <glm/glm.hpp>

class Renderer {
public:
  Renderer();
  ~Renderer();

  bool Init();
  void Cleanup();

  GLuint GetShaderProgram() const { return m_ShaderProgram; }
  GLuint GetVAO() const { return m_VAO; }
  GLuint GetVBO() const { return m_VBO; }
  GLuint GetEBO() const { return m_EBO; }
  GLuint GetCircleVAO() const { return m_CircleVAO; }
  int GetCircleIndexCount() const { return m_CircleIndexCount; }

private:
  GLuint m_ShaderProgram;
  GLuint m_VAO, m_VBO, m_EBO;
  GLuint m_CircleVAO, m_CircleVBO, m_CircleEBO;
  int m_CircleIndexCount;

  bool CompileShaders();
  void CreateQuadBuffers();
  void CreateCircleBuffers();
};

#endif // RENDERER_H

