//
// Created by Rayann CHOUCRALLAH on 05/08/2025.
//

#include "AngryPig.h"

#include "../SoundManager.h"

AngryPig::AngryPig() {
    m_transform = addComponent<TransformComponent>();
    m_transform->rect = {0.f, 0.f, 64.f, 64.f};

    m_texture = addComponent<TextureComponent>();
    m_texture->textureID = TextureID::ANGRYPIG_IDLE;
    initResources();

    m_animator = addComponent<AnimatorComponent>();
    m_animator->currentAnimation = IDLE;
    initAnimations();

    setHorizontalVelocity(0.f);
}

void AngryPig::initResources() const {
    m_texture->loadTextureFromFile(TextureID::ANGRYPIG_IDLE, "assets/Enemies/AngryPig/Idle (36x30).png");
    m_texture->loadTextureFromFile(TextureID::ANGRYPIG_WALK, "assets/Enemies/AngryPig/Walk (36x30).png");
    m_texture->loadTextureFromFile(TextureID::ANGRYPIG_RUNINNG, "assets/Enemies/AngryPig/Run (36x30).png");
    m_texture->loadTextureFromFile(TextureID::ANGRYPIG_HIT1, "assets/Enemies/AngryPig/Hit 1 (36x30).png");
    m_texture->loadTextureFromFile(TextureID::ANGRYPIG_HIT2, "assets/Enemies/AngryPig/Hit 2 (36x30).png");
}

void AngryPig::initAnimations() const {
    m_animator->addAnimation(IDLE, {
            {36.f, 30.f},
            9,
            0.9f,
            true,
            TextureID::ANGRYPIG_IDLE
        });
    m_animator->addAnimation(WALKING, {
        {36.f, 30.f},
        16,
        0.65f,
        true,
        TextureID::ANGRYPIG_WALK
    });
    m_animator->addAnimation(RUNNING, {
            {36.f, 30.f},
            12,
            0.45f,
            true,
            TextureID::ANGRYPIG_RUNINNG
        });
    m_animator->addAnimation(HIT1, {
            {36.f, 30.f},
            5,
            0.35f,
            false,
            TextureID::ANGRYPIG_HIT1
        });
    m_animator->addAnimation(HIT2, {
            {36.f, 30.f},
            5,
            0.75f,
            false,
            TextureID::ANGRYPIG_HIT2
        });
}

AngryPig::~AngryPig() {
}

void AngryPig::onJump() {
    m_animator->reset();
    if (m_animator->currentAnimation == IDLE) {
        m_animator->currentAnimation = WALKING;
        setHorizontalVelocity(100.f);
    }
    else if (m_animator->currentAnimation == WALKING) {
        m_animator->currentAnimation = HIT1;
        setHorizontalVelocity(0.f);
    }
    else if (m_animator->currentAnimation == RUNNING) {
        m_animator->currentAnimation = HIT2;
        setHorizontalVelocity(0.f);
    }
}

#include "../Core.h"
void AngryPig::update(float dt) {
    applyPhysics(dt);
    m_animator->run(m_texture, dt);

    if (m_animator->ended() && m_animator->currentAnimation == HIT1) {
        m_animator->currentAnimation = RUNNING;
        setHorizontalVelocity(450.f);
    }
    else if (m_animator->ended() && m_animator->currentAnimation == HIT2) {
        Core::getInstance().getWorld().destroyEnemy(this, -1);
    }
}

void AngryPig::render(SDL_Renderer *pRenderer) {
    SDL_CHECK(SDL_RenderTextureRotated(pRenderer, m_texture->getTexture(),
        &m_texture->rect, &m_transform->rect, 0.f, nullptr, m_texture->flip));
    debugRender(pRenderer);
}

bool AngryPig::isDying() const {
    return m_animator->currentAnimation == HIT1 || m_animator->currentAnimation == HIT2;
}
