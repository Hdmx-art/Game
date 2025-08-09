//
// Created by Rayann CHOUCRALLAH on 17/07/2025.
//

#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "Config.h"
#include <string>
#include <unordered_map>

struct Component {
    virtual ~Component() = default;
    virtual void destroy() {}
};

struct TransformComponent final : public Component {
    SDL_FRect rect;
    float rotation;

    float bottom() const;
    float right() const;

    void setCenter(const float x, const float y);
    SDL_FPoint center() const;
    void move(const float x, const float y);
};

struct ColliderComponent final : public Component {

    void inflate(TransformComponent* pTransf, const float x, const float y,
        const float offsetx=0.f, const float offsety=0.f);
    bool collide(const ColliderComponent* pOther) const;

    void debugRender(SDL_Renderer* pRenderer) const;

    SDL_FRect rect;
};

struct TextureComponent final : public Component {
    TextureComponent() {
        flip = SDL_FlipMode::SDL_FLIP_NONE;
        textureID = TextureID::NONE;
        rect = {0, 0, 0, 0};
    }

    void loadTextureFromFile(const TextureID id, const std::string &path);

    SDL_Texture* getTexture() const;

    TextureID textureID;
    SDL_FRect rect;
    SDL_FlipMode flip;
};

struct AnimationProperties {
    SDL_FPoint frameSize;
    int32_t frameCount;
    float duration;
    bool repeat;
    TextureID textureID;
};

struct AnimatorComponent final : public Component {

    int32_t currentAnimation;

    void addAnimation(int32_t id, AnimationProperties anim);

    bool ended();
    void run(TextureComponent* const text, float dt);

    void reset();

private:
    std::unordered_map<int32_t, AnimationProperties> m_animations;
    float m_elapsedTime = 0.f;
    int32_t m_previousAnim = 0;
    int32_t m_currentFrame = 0;
    int a = 1;
};

#endif