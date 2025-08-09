//
// Created by Rayann CHOUCRALLAH on 06/08/2025.
//

#include "Turtle.h"

Turtle::Turtle() {
    m_transform = addComponent<TransformComponent>();

    m_texture = addComponent<TextureComponent>();
    initResources();
    m_texture->textureID = TextureID::TURTLE_IDLE1;

    m_animator = addComponent<AnimatorComponent>();
    initAnimations();
    m_animator->currentAnimation = IDLE1;
}

void Turtle::initResources() const {
    m_texture->loadTextureFromFile(TextureID::TURTLE_IDLE1, "assets/Enemies/Turtle/Idle 1 (44x26).png");
    m_texture->loadTextureFromFile(TextureID::TURTLE_IDLE2, "assets/Enemies/Turtle/Idle 2 (44x26).png");
    m_texture->loadTextureFromFile(TextureID::TURTLE_SPIKES_IN, "assets/Enemies/Turtle/Spikes in (44x26).png");
    m_texture->loadTextureFromFile(TextureID::TURTLE_SPIKES_OUT, "assets/Enemies/Turtle/Spikes out (44x26).png");
    m_texture->loadTextureFromFile(TextureID::TURTLE_HIT, "assets/Enemies/Turtle/Hit (44x26).png");
}

void Turtle::initAnimations() const {
    m_animator->addAnimation(IDLE1,
        {
            {44.f, 26.f},
            14,
            0.8f,
            true,
            TextureID::TURTLE_IDLE1,
        });
    m_animator->addAnimation(IDLE2,
        {
            {44.f, 26.f},
            14,
            0.8f,
            true,
            TextureID::TURTLE_IDLE2,
        });
    m_animator->addAnimation(SPIKES_IN,
        {
            {44.f, 26.f},
            8,
            0.6f,
            false,
            TextureID::TURTLE_SPIKES_IN,
        });
    m_animator->addAnimation(SPIKES_OUT,
        {
            {44.f, 26.f},
            8,
            0.6f,
            false,
            TextureID::TURTLE_SPIKES_OUT,
        });
    m_animator->addAnimation(HIT,
        {
            {44.f, 26.f},
            5,
            0.6f,
            false,
            TextureID::TURTLE_HIT,
        });
}

void Turtle::updateAliveState(float dt) {
    m_elapsedTime += dt;

    if (m_elapsedTime >= m_spikesDelay || m_animator->ended()) {
        m_animator->currentAnimation = (m_animator->currentAnimation + 1) % 4;
        if (m_animator->currentAnimation == IDLE1) m_spikesDelay = 2.5f;
        else if (m_animator->currentAnimation == IDLE2) m_spikesDelay = 1.25f;
        m_elapsedTime = 0.f;
        m_animator->reset();
    }
}

void Turtle::onJump() const {
    m_animator->currentAnimation = HIT;
    m_animator->reset();
}

#include "../Core.h"
void Turtle::update(float dt) {
    if (!isDying())
        updateAliveState(dt);
    else if (m_animator->ended()){
        Core::getInstance().getWorld().destroyEnemy(this, -1);
    }

    m_animator->run(m_texture, dt);
}

void Turtle::render(SDL_Renderer *pRenderer) {
    SDL_CHECK(SDL_RenderTextureRotated(pRenderer,
        m_texture->getTexture(),
        &m_texture->rect,
        &m_transform->rect,
        m_transform->rotation,
        nullptr,
        m_texture->flip));

    debugRender(pRenderer);
}

Turtle::SpikesState Turtle::spikeState() const {
    if (m_animator->currentAnimation == IDLE1 || m_animator->currentAnimation == SPIKES_OUT ||
            m_animator->currentAnimation == SPIKES_IN)
        return SpikesState::OUT;

    return SpikesState::IN;
}

bool Turtle::isDying() const {
    return (m_animator->currentAnimation == HIT);
}