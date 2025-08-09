//
// Created by Rayann CHOUCRALLAH on 18/07/2025.
//

#include "Components.h"

float TransformComponent::bottom() const  {
    return rect.y + rect.h;
}

float TransformComponent::right() const {
    return rect.x + rect.w;
}

void TransformComponent::setCenter(const float x, const float y) {
    rect.x = x - rect.w / 2;
    rect.y = y - rect.h / 2;
}

SDL_FPoint TransformComponent::center() const {
    return {rect.x + (rect.w / 2), rect.y + (rect.h / 2)};
}

void TransformComponent::move(const float x, const float y){
    rect.x += x;
    rect.y += y;
}

void ColliderComponent::debugRender(SDL_Renderer *pRenderer) const {
    SDL_CHECK(SDL_SetRenderDrawColor(pRenderer, 255, 0, 0, 255));
    SDL_CHECK(SDL_RenderRect(pRenderer, &rect));
}

#include "Core.h"
void TextureComponent::loadTextureFromFile(const TextureID id, const std::string &path)  {
    Core::getInstance().loadTexture(id, path);
}

SDL_Texture * TextureComponent::getTexture() const {
    return Core::getInstance().getTexture(textureID);
}

void AnimatorComponent::addAnimation(int32_t id, AnimationProperties anim) {
    m_animations[id] = anim;
}

bool AnimatorComponent::ended() {
    const AnimationProperties anim = m_animations[currentAnimation];
    if (anim.repeat == true)
        return false;

    return (m_currentFrame >= anim.frameCount - 1);
}

void AnimatorComponent::run(TextureComponent * const text, float dt) {
    if (m_previousAnim != currentAnimation) reset();
    m_elapsedTime += dt;

    const AnimationProperties anim = m_animations[currentAnimation];
    if (m_elapsedTime >= anim.duration / anim.frameCount) {

        m_currentFrame++;
        if (m_currentFrame > anim.frameCount - 1) {
            if (anim.repeat == true)
                m_currentFrame = 0;
            else
                m_currentFrame = anim.frameCount - 1;
        }

        m_elapsedTime = 0.f;
    }

    text->rect = {
        anim.frameSize.x * m_currentFrame,
        0.f,
        anim.frameSize.x,
        anim.frameSize.y,
    };

    if (anim.textureID != TextureID::NONE) {
        text->textureID = anim.textureID;
    }
    m_previousAnim = currentAnimation;
}

void AnimatorComponent::reset() {
    m_currentFrame = 0;
    m_elapsedTime = 0.f;
}

void ColliderComponent::inflate(TransformComponent *pTransf, const float x, const float y, const float offsetx,
                                const float offsety)  {
    rect.w = pTransf->rect.w * x;
    rect.h = pTransf->rect.h * y;

    rect.x = pTransf->rect.x + (pTransf->rect.w - rect.w) / 2 + offsetx;
    rect.y = pTransf->rect.y + (pTransf->rect.h - rect.h) / 2 + offsety;
}

bool ColliderComponent::collide(const ColliderComponent *pOther) const {
    return SDL_HasRectIntersectionFloat(&rect, &pOther->rect);
}