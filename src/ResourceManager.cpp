#include "ResourceManager.h"
#include <iostream>
#include <map>

ResourceManager::ResourceManager() : m_TextRenderer(nullptr) {
  m_TextRenderer = new TextRenderer();
}

ResourceManager::~ResourceManager() {
  Cleanup();
}

bool ResourceManager::LoadTexture(const std::string& name, const std::string& path) {
  Texture* texture = new Texture();
  if (texture->Load(path.c_str())) {
    m_Textures[name] = texture;
    return true;
  }
  delete texture;
  return false;
}

Texture* ResourceManager::GetTexture(const std::string& name) {
  auto it = m_Textures.find(name);
  if (it != m_Textures.end()) {
    return it->second;
  }
  return nullptr;
}

bool ResourceManager::LoadSound(const std::string& name, const std::string& path) {
  Mix_Chunk* sound = Mix_LoadWAV(path.c_str());
  if (sound != nullptr) {
    m_Sounds[name] = sound;
    return true;
  }
  std::cerr << "Failed to load sound: " << path << " - " << Mix_GetError() << std::endl;
  return false;
}

Mix_Chunk* ResourceManager::GetSound(const std::string& name) {
  auto it = m_Sounds.find(name);
  if (it != m_Sounds.end()) {
    return it->second;
  }
  return nullptr;
}

bool ResourceManager::LoadMusic(const std::string& name, const std::string& path) {
  Mix_Music* music = Mix_LoadMUS(path.c_str());
  if (music != nullptr) {
    m_Music[name] = music;
    return true;
  }
  std::cerr << "Failed to load music: " << path << " - " << Mix_GetError() << std::endl;
  return false;
}

Mix_Music* ResourceManager::GetMusic(const std::string& name) {
  auto it = m_Music.find(name);
  if (it != m_Music.end()) {
    return it->second;
  }
  return nullptr;
}

bool ResourceManager::LoadFont(const std::string& path, int fontSize) {
  if (m_TextRenderer) {
    return m_TextRenderer->LoadFont(path.c_str(), fontSize);
  }
  return false;
}

void ResourceManager::PlayMusic(const std::string& name, int loops) {
  Mix_Music* music = GetMusic(name);
  if (music) {
    Mix_PlayMusic(music, loops);
  }
}

void ResourceManager::PlaySound(const std::string& name) {
  Mix_Chunk* sound = GetSound(name);
  if (sound) {
    Mix_PlayChannel(-1, sound, 0);
  }
}

void ResourceManager::Cleanup() {
  // Cleanup textures
  for (auto& pair : m_Textures) {
    if (pair.second) {
      pair.second->Cleanup();
      delete pair.second;
    }
  }
  m_Textures.clear();

  // Cleanup sounds
  for (auto& pair : m_Sounds) {
    if (pair.second) {
      Mix_FreeChunk(pair.second);
    }
  }
  m_Sounds.clear();

  // Cleanup music
  for (auto& pair : m_Music) {
    if (pair.second) {
      Mix_FreeMusic(pair.second);
    }
  }
  m_Music.clear();

  // Cleanup text renderer
  if (m_TextRenderer) {
    m_TextRenderer->Cleanup();
    delete m_TextRenderer;
    m_TextRenderer = nullptr;
  }
}

