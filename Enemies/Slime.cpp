//
// Created by Rayann CHOUCRALLAH on 03/08/2025.
//

#include "Slime.h"
#include "../Core.h"

Slime::Slime() {
    m_transf = addComponent<TransformComponent>();
    m_transf->rect = {0.f, 0.f, 44.f * 1.6f, 30.f * 1.6f}; // Set the size of

    m_texture = addComponent<TextureComponent>();
    m_texture->loadTextureFromFile(TextureID::SLIME_IDLE, "assets/Enemies/Slime/Idle-Run (44x30).png");
    m_texture->loadTextureFromFile(TextureID::SLIME_HIT, "assets/Enemies/Slime/Hit (44x30).png");
    m_texture->textureID = TextureID::SLIME_IDLE;
    m_texture->rect = {0, 0, 44, 30}; // Set the size of the texture

    m_animator = addComponent<AnimatorComponent>();
    m_animator->currentAnimation = 1;
    m_animator->addAnimation(1,
        {
            {44.f, 30.f},
            10,
            0.75f,
            true,
            TextureID::SLIME_IDLE
        });
    m_animator->addAnimation(0,
        {
            {44.f, 30.f},
            5,
            0.65f,
            false,
            TextureID::SLIME_HIT
        });

    setHorizontalVelocity(60.f);
    m_timeSinceDeath = 0.f;
}

void Slime::onJump() {
    m_animator->currentAnimation = 0;
    setHorizontalVelocity(0.f);
}

void Slime::update(float dt) {
    m_animator->run(m_texture, dt);
    applyPhysics(dt);

    if (m_animator->ended()) {
        m_timeSinceDeath += dt;

        if (m_timeSinceDeath >= m_deathTimer)
            Core::getInstance().getWorld().destroyEnemy(this, -1);
    }
}

void Slime::render(SDL_Renderer *pRenderer) {
    SDL_CHECK(SDL_RenderTextureRotated(pRenderer, m_texture->getTexture(), &m_texture->rect,
        &m_transf->rect, 0.f, nullptr, m_texture->flip));
    debugRender(pRenderer);
}

bool Slime::isDying() const {
    return m_animator->ended();
}