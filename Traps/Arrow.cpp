//
// Created by Rayann CHOUCRALLAH on 03/08/2025.
//

#include "Arrow.h"
#include "../Core.h"
#include "../World.h"
#include "../SoundManager.h"

Arrow::Arrow() {
    m_transform = addComponent<TransformComponent>();
    m_transform->rect = {0.f, 0.f, 32.f, 32.f};

    m_texture = addComponent<TextureComponent>();
    m_texture->rect = {0.f, 0.f, 18.f, 18.f};
    m_texture->textureID = TextureID::ARROW;
    m_texture->loadTextureFromFile(TextureID::ARROW, "assets/Traps/Arrow/Idle (18x18).png");
    m_texture->loadTextureFromFile(TextureID::ARROW_HIT, "assets/Traps/Arrow/Hit (18x18).png");

    m_animator = addComponent<AnimatorComponent>();
    m_animator->currentAnimation = 1;
    m_animator->addAnimation(1, {
    {18.f, 18.f},
        4,
        0.45f,
        true,
        TextureID::ARROW,
    });
    m_animator->addAnimation(0, {
    {18.f, 18.f},
        4,
        .25f,
        false,
        TextureID::ARROW_HIT,
    });

    m_used = false;
}

void Arrow::onHit() {
    m_animator->currentAnimation = 0;
    m_used = true;
    SoundManager::getInstance().playSound(SoundID::COLLECT_FRUIT);
}

void Arrow::setDirection(float rotation) {
    if (rotation == 0.f) {
        m_type = ArrowType::UP;
    } else if (rotation == 90.f) {
        m_type = ArrowType::RIGHT;
    } else if (rotation == 180.f) {
        m_type = ArrowType::DOWN;
    } else if (rotation == 270.f) {
        m_type = ArrowType::LEFT;
    } else {
        m_type = ArrowType::UP; // Default case
    }
    m_transform->rotation = rotation;
}

void Arrow::update(float dt) {
    m_animator->run(m_texture, dt);

    if (m_animator->ended()) {
        World& world = Core::getInstance().getWorld();
        world.destroyTrap(this, -1);
    }
}

void Arrow::render(SDL_Renderer *pRenderer) {
    SDL_CHECK(SDL_RenderTextureRotated(pRenderer, m_texture->getTexture(),
        &m_texture->rect, &m_transform->rect, m_transform->rotation, nullptr, m_texture->flip));
    debugRender(pRenderer);
}

Arrow::ArrowType Arrow::getType() const {
    return m_type;
}

bool Arrow::used() const {
    return m_used;
}
