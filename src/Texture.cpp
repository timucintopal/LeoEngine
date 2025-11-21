#include "Texture.h"
#include <iostream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture() : ID(0), width(0), height(0), nrChannels(0) {}

Texture::~Texture() { Cleanup(); }

bool Texture::Load(const char *path) {
  glGenTextures(1, &ID);
  glBindTexture(GL_TEXTURE_2D, ID);

  // Set texture wrapping/filtering options
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // stbi_set_flip_vertically_on_load(true); // Removed to fix inverted texture
  // with Top-Left origin
  unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);

  if (data) {
    GLenum format = GL_RGB;
    if (nrChannels == 1)
      format = GL_RED;
    else if (nrChannels == 3)
      format = GL_RGB;
    else if (nrChannels == 4)
      format = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    std::cout << "Texture loaded: " << path << std::endl;
  } else {
    std::cout << "Failed to load texture: " << path << ". Using fallback."
              << std::endl;

    // Fallback: Checkerboard
    width = 64;
    height = 64;
    std::vector<unsigned char> checkImage(width * height * 3);
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        unsigned char color = ((((y / 8) + (x / 8)) % 2) == 0) ? 0 : 255;
        int index = (y * width + x) * 3;
        checkImage[index] = color;     // R
        checkImage[index + 1] = color; // G
        checkImage[index + 2] = color; // B
      }
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, checkImage.data());
    glGenerateMipmap(GL_TEXTURE_2D);
  }

  stbi_image_free(data);
  return true;
}

void Texture::Bind() { glBindTexture(GL_TEXTURE_2D, ID); }

void Texture::Unbind() { glBindTexture(GL_TEXTURE_2D, 0); }

void Texture::Cleanup() { glDeleteTextures(1, &ID); }
