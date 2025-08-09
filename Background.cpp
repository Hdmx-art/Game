//
// Created by Rayann CHOUCRALLAH on 27/07/2025.
//

#include "Background.h"

Background::Background() {

    m_backgroundTexture = addComponent<TextureComponent>();
    m_backgroundTexture->loadTextureFromFile(TextureID::BG_BLUE, "assets/Background/Blue.png");
    m_backgroundTexture->loadTextureFromFile(TextureID::BG_BROWN, "assets/Background/Brown.png");
    m_backgroundTexture->loadTextureFromFile(TextureID::BG_GRAY, "assets/Background/Gray.png");
    m_backgroundTexture->loadTextureFromFile(TextureID::BG_GREEN, "assets/Background/Green.png");
    m_backgroundTexture->loadTextureFromFile(TextureID::BG_PINK, "assets/Background/Pink.png");
    m_backgroundTexture->loadTextureFromFile(TextureID::BG_PURPLE, "assets/Background/Purple.png");
    m_backgroundTexture->loadTextureFromFile(TextureID::BG_YELLOW, "assets/Background/Yellow.png");

    reset(TextureID::BG_BROWN);

    constexpr float tileSize = 180.f;

    for (int i = 0;i< 720 / tileSize + 1;i++) {
        for (int j = 0;j< 1280 / tileSize + 1;j++) {
            m_tiles.push_back({
                j * tileSize,
                i * tileSize,
                tileSize,
                tileSize + 0.5f
            });
        }
    }
}

void Background::update(float dt) {
    for (auto& tile : m_tiles) {
        if (tile.y >= 720.f) {
            tile.y = -180.f;
        }
        tile.y += 50.f * dt;
    }
}

void Background::render(SDL_Renderer *pRenderer) {
    for (auto& tile : m_tiles) {
        SDL_CHECK(SDL_RenderTexture(pRenderer, m_backgroundTexture->getTexture(),
            nullptr, &tile));
        //SDL_SetRenderDrawColor(pRenderer, 0, 255, 255, 255);
        //SDL_RenderRect(pRenderer, &tile);
    }
}

void Background::reset(TextureID id) const {
    m_backgroundTexture->textureID = id;
}
