//
// Created by Rayann CHOUCRALLAH on 26/07/2025.
//

#include "Effects.h"

#include "vendored/SDL_mixer/src/timidity/options.h"

DesappearingEffect::DesappearingEffect(float x, float y) {
    m_transform = addComponent<TransformComponent>();
    m_transform->rect = {x,y, 32.f, 32.f};

    m_texture= addComponent<TextureComponent>();
    m_texture->loadTextureFromFile(TextureID::DESAPPEARING_EFFECT,
        "assets/Items/Fruits/Collected.png");
    m_texture->textureID = TextureID::DESAPPEARING_EFFECT;

    m_animator = addComponent<AnimatorComponent>();
    m_animator->currentAnimation = 0;
    m_animator->addAnimation(
        0,
        {
            {32.f, 32.f},
            6,
            0.45f,
            false,
            TextureID::DESAPPEARING_EFFECT,
        });
}

DesappearingEffect::~DesappearingEffect() {
}

void DesappearingEffect::update(float dt) {
    m_animator->run(m_texture, dt);
}

void DesappearingEffect::render(SDL_Renderer *pRenderer) {
    SDL_CHECK(SDL_RenderTexture(pRenderer, m_texture->getTexture(),
        &m_texture->rect, &m_transform->rect));
}

bool DesappearingEffect::ended() const {
    return m_animator->ended();
}

DustEmitter::DustEmitter() {
    m_texture = addComponent<TextureComponent>();
    m_texture->loadTextureFromFile(TextureID::DUST_EFFECT, "assets/Other/Dust Particle.png");
    m_texture->textureID = TextureID::DUST_EFFECT;

    m_elapsedTime = 0.25f;
}

void DustEmitter::update(float dt) {

    for (int32_t i = 0;i<m_particles.size();i++) {
        m_particles[i].rect.w -= dt * 125.f;
        m_particles[i].rect.h -= dt * 125.f;

        m_particles[i].rect.x += dt * 90.f * m_particles[i].scalar;
        m_particles[i].rect.y -= dt * 20.f;

        if (m_particles[i].rect.h <= 0||m_particles[i].rect.w <= 0) {
            m_particles.erase(m_particles.begin() + i);
            i--;
        }
    }

    m_elapsedTime += dt;
}

void DustEmitter::render(SDL_Renderer *pRenderer) {
    for (auto& particle : m_particles) {
        SDL_CHECK(SDL_RenderTexture(pRenderer, m_texture->getTexture(),
            nullptr, &particle.rect));
    }
}

void DustEmitter::clear() {
    m_particles.clear();
}

void DustEmitter::emit(float x, float y, float scalar) {
    if (m_elapsedTime > m_rate) {
        m_particles.push_back({
            {x,y - 32.f,32.f, 32.f}, scalar
        });
        m_elapsedTime = 0.f;
    }
}

TransitionEffect::TransitionEffect() {
    m_texture = addComponent<TextureComponent>();
    m_texture->rect = {0.f, 0.f, 44.f, 44.f};
    m_texture->textureID = TextureID::TRANSITION_EFFECT;
}

void TransitionEffect::reset() {
    m_texture->loadTextureFromFile(TextureID::TRANSITION_EFFECT,
    "assets/Other/Transition.png");

    for (int32_t i = 0;i < 3;i++) {
        for (int32_t j = 0;j < 6;j++) {
            Diamond diamond{};

            diamond.scalar = 1;
            diamond.rect = {
                static_cast<float>(j * 244.f) + MAX_SIZE / 4,
                static_cast<float>(i * 244.f) + MAX_SIZE / 4,
                1.f, 1.f
            };

            m_diamonds.push_back(diamond);
        }
    }
}

void TransitionEffect::updateDiamond(float dt, Diamond &diamond) {
    const float centerX = diamond.rect.x + diamond.rect.w / 2.f;
    const float centerY = diamond.rect.y + diamond.rect.h / 2.f;

    diamond.elapsedTime += dt * 2.f;
    float newSize = Math::easeInOutSine(diamond.elapsedTime) * MAX_SIZE;
    diamond.rect.w = newSize;
    diamond.rect.h = newSize;

    diamond.rect.x = centerX - newSize / 2.f;
    diamond.rect.y = centerY - newSize / 2.f;
}

void TransitionEffect::update(float dt) {
    for (int32_t i = 0; i < m_diamonds.size(); i++) {
        if (i % 6 != 0 && m_diamonds[i - 1].rect.w <= 50.f && m_diamonds[i - 1].elapsedTime <= 1.f)
            continue;

        updateDiamond(dt, m_diamonds[i]);
        if (m_diamonds[i].elapsedTime >= 2.f)
            m_diamonds.erase(m_diamonds.begin() + i);
    }
}

void TransitionEffect::render(SDL_Renderer *pRenderer) {
    for (auto& diamond : m_diamonds) {
        SDL_CHECK(SDL_RenderTexture(pRenderer, m_texture->getTexture(),
            &m_texture->rect, &diamond.rect));
    }
}

bool TransitionEffect::ended() const {
    return m_diamonds.empty();
}