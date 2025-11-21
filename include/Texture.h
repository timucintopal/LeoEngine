#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

class Texture {
public:
  Texture();
  ~Texture();

  bool Load(const char *path);
  void Bind();
  void Unbind();
  void Cleanup();

private:
  GLuint ID;
  int width, height, nrChannels;
};

#endif // TEXTURE_H
