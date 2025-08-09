//
// Created by Rayann CHOUCRALLAH on 06/08/2025.
//

#include "FallingPlatform.h"

FallingPlatform::FallingPlatform() {
    m_transform = addComponent<TransformComponent>();

    m_texture = addComponent<TextureComponent>();
    m_texture->loadTextureFromFile(TextureID::FALLING_PLATFORM_ON, "assets/Traps/Falling Platforms/On (32x10).png");
    m_texture->loadTextureFromFile(TextureID::FALLING_PLATFORM_OFF, "assets/Traps/Falling Platforms/Off.png");
    m_texture->textureID = TextureID::FALLING_PLATFORM_ON;

    m_animator = addComponent<AnimatorComponent>();
    m_animator->addAnimation(OFF, {
        {32.f, 10.f},
        1,
        1.f,
        true,
        TextureID::FALLING_PLATFORM_OFF
    });
    m_animator->addAnimation(ON, {
        {32.f, 10.f},
        4,
        0.2f,
        true,
        TextureID::FALLING_PLATFORM_ON
    });
    m_animator->currentAnimation = ON;

    m_time = 0.f;
    m_state =  State::ON;
}

void FallingPlatform::onHit() {
    if (m_state == State::ON) {
        m_state = State::OFF;
        m_scalar = 300.f;
        m_time = 0.5f;
    }
}

void FallingPlatform::update(float dt) {
    m_time += dt;
    float offset = (Math::easeInOutSine(m_time) - 0.5f) * 2.f;

    if (m_state == State::OFF) {
        m_scalar = std::max(0.f, m_scalar - 520.f * dt);
        if (m_scalar == 0)
            m_state = State::FALLING;
    }

    if (m_state == State::FALLING) {
        m_animator->currentAnimation = OFF;
        m_scalar = 580.f;
        offset = 1.f;
    }

    m_transform->move( 0.f,offset * dt * m_scalar);

    m_animator->run(m_texture, dt);
}

void FallingPlatform::render(SDL_Renderer *pRenderer) {
    SDL_CHECK(SDL_RenderTexture(pRenderer, m_texture->getTexture(), &m_texture->rect, &m_transform->rect));
    debugRender(pRenderer);
}
