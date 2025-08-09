//
// Created by Rayann CHOUCRALLAH on 07/08/2025.
//

#include "Fire.h"

Fire::Fire() {
    m_transform = addComponent<TransformComponent>();

    m_texture = addComponent<TextureComponent>();
    m_texture->textureID = TextureID::FIRE_OFF;
    initResources();

    m_animator = addComponent<AnimatorComponent>();
    m_animator->currentAnimation = OFF;
    initAnimation();

    m_collider = addComponent<ColliderComponent>();
}

void Fire::onHit(bool activate) const {
    if (activate) {
        if (m_animator->currentAnimation != ON) {
            m_animator->currentAnimation = HIT;
        }
    }
}

bool Fire::overlapFire(const ColliderComponent *collide) const {
    return SDL_HasRectIntersectionFloat(&collide->rect, &m_proj);
}

void Fire::update(float dt) {
    m_proj = m_collider->rect;
    m_proj.y = m_transform->rect.y;

    if (m_animator->ended() && m_animator->currentAnimation == HIT) {
        m_animator->currentAnimation = ON;
    }

    m_collider->inflate(m_transform, 1.f, 0.5f, 0.f, m_transform->rect.h / 4.f);
    m_animator->run(m_texture, dt);
}

void Fire::render(SDL_Renderer *pRenderer) {
    SDL_CHECK(SDL_RenderTextureRotated(pRenderer, m_texture->getTexture(), &m_texture->rect,
        &m_transform->rect, m_transform->rotation, nullptr, SDL_FLIP_NONE));
    debugRender(pRenderer);

    SDL_SetRenderDrawColor(pRenderer, 0, 255, 0, 255);
    SDL_RenderRect(pRenderer, &m_proj);
    m_collider->debugRender(pRenderer);
}

bool Fire::activated() const {
    return m_animator->currentAnimation == ON;
}

void Fire::initResources() const {
    m_texture->loadTextureFromFile(TextureID::FIRE_OFF, "assets/Traps/Fire/Off.png");
    m_texture->loadTextureFromFile(TextureID::FIRE_ON, "assets/Traps/Fire/On (16x32).png");
    m_texture->loadTextureFromFile(TextureID::FIRE_HIT, "assets/Traps/Fire/Hit (16x32).png");
}

void Fire::initAnimation() const {
    m_animator->addAnimation(OFF, {
        {16.f, 32.f},
        1,
        1.f,
        true,
        TextureID::FIRE_OFF
    });
    m_animator->addAnimation(ON, {
        {16.f, 32.f},
        3,
        0.45f,
        true,
        TextureID::FIRE_ON
    });
    m_animator->addAnimation(HIT, {
            {16.f, 32.f},
            4,
            0.8f,
            false,
            TextureID::FIRE_HIT
        });
}