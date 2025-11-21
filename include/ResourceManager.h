#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "Texture.h"
#include "TextRenderer.h"
#include <SDL_mixer.h>
#include <string>
#include <map>

class ResourceManager {
public:
  ResourceManager();
  ~ResourceManager();

  bool LoadTexture(const std::string& name, const std::string& path);
  Texture* GetTexture(const std::string& name);
  
  bool LoadSound(const std::string& name, const std::string& path);
  Mix_Chunk* GetSound(const std::string& name);
  
  bool LoadMusic(const std::string& name, const std::string& path);
  Mix_Music* GetMusic(const std::string& name);
  
  bool LoadFont(const std::string& path, int fontSize);
  TextRenderer* GetTextRenderer() { return m_TextRenderer; }
  
  void PlayMusic(const std::string& name, int loops = -1);
  void PlaySound(const std::string& name);
  
  void Cleanup();

private:
  std::map<std::string, Texture*> m_Textures;
  std::map<std::string, Mix_Chunk*> m_Sounds;
  std::map<std::string, Mix_Music*> m_Music;
  TextRenderer* m_TextRenderer;
};

#endif // RESOURCEMANAGER_H

