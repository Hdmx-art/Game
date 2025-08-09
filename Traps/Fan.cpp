//
// Created by Rayann CHOUCRALLAH on 31/07/2025.
//

#include "Fan.h"

Fan::Fan() {
    m_transform = addComponent<TransformComponent>();
    m_transform->rect = {0.f, 0.f, 48.f, 16.f};

    m_texture = addComponent<TextureComponent>();
    m_texture->loadTextureFromFile(TextureID::FAN, "assets/Traps/Fan/On (24x8).png");
    m_texture->textureID = TextureID::FAN;

    m_animator = addComponent<AnimatorComponent>();
    m_animator->currentAnimation = 1;
    m_animator->addAnimation(1, {
        {24.f, 8.f},
        4,
        0.2f,
        true,
        TextureID::FAN
    });
    m_animator->addAnimation(0, {
        {24.f, 8.f},
        1,
        1.f,
        false,
        TextureID::FAN
    });

    m_duration = 4.f;
    m_elapsedTime = 0.f;
    m_resetTimer = 0.f;
}

Fan::~Fan() {
    destroyComponents();
}

bool Fan::overlap(const ColliderComponent *pColl) const {
    return (SDL_HasRectIntersectionFloat(&m_projection, &pColl->rect) && m_elapsedTime < 4.f);
}

float Fan::distanceFromFan(const ColliderComponent *pColl) const {
    if (m_transform->rotation == 0.f)
        return 1.f - std::min(0.5f, std::abs(pColl->rect.y - m_transform->rect.y) / m_projection.h);

    return 1.f - std::min(0.5f, std::abs(pColl->rect.x - m_transform->rect.x) / m_projection.w);
}

void Fan::setType(const FanType type) {
    m_type = type;

    if (m_type == FanType::HORIZONTAL)
        m_transform->rotation = 90.f;
}

void Fan::update(float dt) {
    if (m_type == FanType::VERTICAL) {
        m_projection.w = m_transform->rect.w;
        m_projection.h = m_transform->rect.h * 20;
        m_projection.x = m_transform->rect.x;
        m_projection.y = m_transform->rect.y - m_projection.h;
    }
    else if (m_type == FanType::HORIZONTAL) {
        m_projection.h = m_transform->rect.w;
        m_projection.w = m_transform->rect.h * 25;
        m_projection.x = m_transform->rect.x;
        m_projection.y = m_transform->rect.y - m_projection.h / 2.5f;
    }

    if (m_elapsedTime < m_duration) {
        m_elapsedTime += dt;
        m_animator->currentAnimation = 1;
    }
    else {
        m_resetTimer += dt;
        m_animator->currentAnimation = 0;
    }

    if (m_resetTimer >= 3.f) {
        m_elapsedTime = 0.f;
        m_resetTimer = 0.f;
    }

    m_animator->run(m_texture, dt);
}

void Fan::render(SDL_Renderer *pRenderer) {
    SDL_CHECK(SDL_RenderTextureRotated(pRenderer, m_texture->getTexture(),
        &m_texture->rect, &m_transform->rect, m_transform->rotation, nullptr, SDL_FLIP_NONE));

    debugRender(pRenderer);
    SDL_CHECK(SDL_SetRenderDrawColor(pRenderer, 0, 0, 255, 255));
    SDL_CHECK(SDL_RenderRect(pRenderer, &m_projection));
}

Fan::FanType Fan::getType() const {
    return m_type;
}
