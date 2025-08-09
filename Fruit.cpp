//
// Created by Rayann CHOUCRALLAH on 23/07/2025.
//

#include "Fruit.h"
#include "SoundManager.h"

Fruit::Fruit() {
    m_transform = addComponent<TransformComponent>();
    m_transform->rect = { 0,0, 64.f, 64.f};

    m_collider = addComponent<ColliderComponent>();

    m_texture = addComponent<TextureComponent>();
    m_texture->rect = { 0,0, 32.f, 32.f};
    initRessources();

    m_animator = addComponent<AnimatorComponent>();
    m_animator->currentAnimation = 0;
    m_animator->addAnimation(0, {
                                 {32.f, 32.f},
                                 17,
                                 0.55f,
                                 true,
                                 TextureID::NONE
                             });

    SoundManager::getInstance().loadSoundFromFile(
        SoundID::COLLECT_FRUIT,
        "assets/Sounds/coin.wav"
        );
}


void Fruit::initRessources() const {
    m_texture->loadTextureFromFile(TextureID::APPLE, "assets/Items/Fruits/Apple.png");
    m_texture->loadTextureFromFile(TextureID::BANANA, "assets/Items/Fruits/Bananas.png");
    m_texture->loadTextureFromFile(TextureID::KIWI, "assets/Items/Fruits/Kiwi.png");
    m_texture->loadTextureFromFile(TextureID::CHERRY, "assets/Items/Fruits/Cherries.png");
    m_texture->loadTextureFromFile(TextureID::ORANGE, "assets/Items/Fruits/Orange.png");
    m_texture->loadTextureFromFile(TextureID::PINEAPPLE, "assets/Items/Fruits/Pineapple.png");
    m_texture->loadTextureFromFile(TextureID::STRAWBERRY, "assets/Items/Fruits/Strawberry.png");
    m_texture->loadTextureFromFile(TextureID::MELON, "assets/Items/Fruits/Melon.png");
}

Fruit::~Fruit() {
    destroyComponents();
}

void Fruit::setType(TextureID type) {
    m_texture->textureID = type;
}

void Fruit::update(float dt) {
    m_collider->inflate(m_transform, 0.5f, 0.5f);
    m_animator->run(m_texture, dt);
}

void Fruit::render(SDL_Renderer *pRenderer) {
    SDL_CHECK(SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255));
    SDL_CHECK(SDL_RenderTexture(pRenderer, m_texture->getTexture(),
        &m_texture->rect, &m_transform->rect));

    if (DEBUG_STATE) {
        SDL_CHECK(SDL_SetRenderDrawColor(pRenderer, 0, 0, 255, 255));
        SDL_CHECK(SDL_RenderRect(pRenderer, &m_transform->rect));
        m_collider->debugRender(pRenderer);
    }
}