//
// Created by Rayann CHOUCRALLAH on 17/07/2025.
//

#include "Core.h"

#include "SDL3_image/SDL_image.h"
#include "Player.h"
#include "SoundManager.h"

Core* Core::s_instance = nullptr;

Core::Core() {
    m_window = nullptr;
    m_renderer = nullptr;
    m_debugFont = nullptr;

    m_windowWidth = 1280;
    m_windowHeight = 720;
    m_timeSinceLastFrame = 0;
    m_running = false;
    m_dt = 0.f;
}

Core::~Core() {
    destroy();
}

void Core::run() {
    init();
    m_running = true;

    while (m_running) {
        float dt = static_cast<float>(SDL_GetTicks() - m_timeSinceLastFrame) / 1000.f;
        if (CAP_FPS) dt = std::min(dt, 1.f / 144.f);
        m_dt = dt;
        m_timeSinceLastFrame = SDL_GetTicks();

        updateEvents();
        update(dt);
        render();
    }
}

void Core::loadTexture(TextureID id, const std::string &path) {
    WARNING_IF( (m_textures.find(id) != m_textures.end()),
        "La texture a déjà été chargée");

    SDL_Surface *surface = IMG_Load(path.c_str());
    PTR_CHECK(surface);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    SDL_DestroySurface(surface);
    PTR_CHECK(texture);

    m_textures.emplace(id, texture);
}

SDL_Texture * Core::getTexture(const TextureID id) const {
    if (m_textures.find(id) == m_textures.end()) {
        SDL_Log("%i", id);
        PTR_CHECK(nullptr);
    }

    return m_textures.find(id)->second;
}

World& Core::getWorld() {
    return m_world;
}

Core& Core::getInstance() {
    if (s_instance == nullptr)
        s_instance = new Core();

    return *s_instance;
}

void Core::init() {
    SDL_CHECK(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO));

    SDL_CHECK(SDL_CreateWindowAndRenderer("Game", m_windowWidth, m_windowHeight,
        0, &m_window, &m_renderer));

    SDL_CHECK(TTF_Init());

    m_debugFont = TTF_OpenFont("assets/Menu/Text/Coolvetica Rg.otf", 48);
    PTR_CHECK(m_debugFont);

    SoundManager::getInstance();
    SoundManager::getInstance().loadSoundFromFile(SoundID::HURT, "assets/Sounds/hurt.wav");

    loadTexture(TextureID::TERRAIN, "assets/Terrain/Terrain (16x16).png");
    loadTexture(TextureID::SHADOW, "assets/Other/Shadow.png");
    m_world.init();
}

void Core::destroy() {
    for (auto& texture : m_textures)
        SDL_DestroyTexture(texture.second);
    m_textures.clear();

    SDL_DestroyRenderer(m_renderer);
    m_renderer = nullptr;

    SDL_DestroyWindow(m_window);
    m_window = nullptr;

    SDL_Quit();
}

void Core::updateEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT)
            m_running = false;
        if (event.type == SDL_EVENT_KEY_DOWN) {
            if (event.key.key == SDLK_E) {
                m_world.loadMap(MapID::LEVEL_1);
            }
        }
    }
}

void Core::update(float dt) {
    m_world.update(dt);
}

void Core::render() {
    SDL_CHECK(SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255));
    SDL_CHECK(SDL_RenderClear(m_renderer));

    m_world.render(m_renderer);

    if (DEBUG_STATE) {
        const std::string res = "dt  " + std::to_string(static_cast<int>(m_dt * 1000.f)) + "ms";
        SDL_Surface* surf = TTF_RenderText_Solid(m_debugFont,
            res.c_str(), 36, {255, 255, 255});
        PTR_CHECK(surf);

        SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surf);
        SDL_DestroySurface(surf);
        PTR_CHECK(texture);

        const SDL_FRect rect = {0, 0, res.size() * 8.f, 36};
        SDL_CHECK(SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255));
        SDL_CHECK(SDL_RenderFillRect(m_renderer, &rect));

        SDL_RenderTexture(m_renderer, texture, nullptr, &rect);
    }

    SDL_CHECK(SDL_RenderPresent(m_renderer));
}
