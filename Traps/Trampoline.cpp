//
// Created by Rayann CHOUCRALLAH on 29/07/2025.
//

#include "Trampoline.h"

Trampoline::Trampoline() {

    m_transform = addComponent<TransformComponent>();
    m_transform->rect = {0.f, 0.f, 64.f, 64.f};

    m_texture = addComponent<TextureComponent>();
    m_texture->loadTextureFromFile(TextureID::TRAMPOLINE, "assets/Traps/Trampoline/Jump (28x28).png");
    m_texture->textureID = TextureID::TRAMPOLINE;

    m_animator = addComponent<AnimatorComponent>();
    m_animator->addAnimation(1, {
        {28.f, 28.f},
        8,
        1.f,
        false,
        TextureID::TRAMPOLINE
    });
    m_animator->addAnimation(0, {
        {28.f, 28.f},
        1,
        1.f,
        false,
        TextureID::TRAMPOLINE
    });
    m_animator->currentAnimation = 0;

    m_collider = addComponent<ColliderComponent>();
}

void Trampoline::onJump() {
    m_animator->currentAnimation = 1;
}

void Trampoline::update(float dt) {
    if (m_animator->currentAnimation == 1 && m_animator->ended())
        m_animator->currentAnimation = 0;

    m_collider->inflate(m_transform, 1.f, 0.2f, 0.f, 15.f);
    m_animator->run(m_texture, dt);
}

void Trampoline::render(SDL_Renderer *pRenderer) {
    SDL_CHECK(SDL_RenderTextureRotated(pRenderer, m_texture->getTexture(),
        &m_texture->rect, &m_transform->rect, m_transform->rotation, nullptr, SDL_FLIP_NONE));

    if (DEBUG_STATE) {
        SDL_CHECK(SDL_SetRenderDrawColor(pRenderer, 0, 0, 255, 255));
        SDL_CHECK(SDL_RenderRect(pRenderer, &m_transform->rect));
        m_collider->debugRender(pRenderer);
    }
}

Trampoline::~Trampoline() {
    destroyComponents();
}