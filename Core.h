//
// Created by Rayann CHOUCRALLAH on 17/07/2025.
//


#ifndef CORE_H
#define CORE_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <unordered_map>
#include "Config.h"
#include "World.h"

class Player;

class Core {
    static Core *s_instance;

public:
    ~Core();

    void run();

    void loadTexture(TextureID id, const std::string &path);
    SDL_Texture* getTexture(const TextureID id) const;
    World& getWorld();

    static Core& getInstance();

private:
    Core();

    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    TTF_Font* m_debugFont;

    World m_world;

    int32_t m_windowWidth, m_windowHeight;
    uint64_t m_timeSinceLastFrame;
    bool m_running;
    float m_dt;

    std::unordered_map<TextureID, SDL_Texture*> m_textures;

    void init();
    void destroy();

    void updateEvents();
    void update(float dt);
    void render();
};

#endif //CORE_H
