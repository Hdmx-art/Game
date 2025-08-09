//
// Created by Rayann CHOUCRALLAH on 26/07/2025.
//

#ifndef EFFECTS_H
#define EFFECTS_H

#include "Drawable.h"
#include "Components.h"

class Effect : public Drawable {
public:
    Effect() {}

    virtual bool ended() const = 0;
};

class DesappearingEffect final : public Effect {
public:
    DesappearingEffect(float x, float y);
    ~DesappearingEffect() override;

    virtual void update(float dt) override;
    virtual void render(SDL_Renderer* pRenderer) override;

    bool ended() const override;

private:
    TransformComponent* m_transform;
    TextureComponent* m_texture;
    AnimatorComponent* m_animator;
};

class DustEmitter final : public Drawable {
    struct Dust {
        SDL_FRect rect;
        float scalar;
    };
public:
    DustEmitter();

    void update(float dt) override;
    void render(SDL_Renderer *pRenderer) override;

    void clear();
    void emit(float x, float y, float scalar);

private:
    TextureComponent* m_texture;
    std::vector<Dust> m_particles;

    float m_elapsedTime;
    const float m_rate = 0.15f;
};

#include <vector>
class TransitionEffect final : public Drawable {

    const float MAX_SIZE = 550.f;

    struct Diamond {
        int32_t scalar;
        SDL_FRect rect;
        float elapsedTime;
    };

public:
    TransitionEffect();

    void reset();

    void update(float dt) override;
    void render(SDL_Renderer *pRenderer) override;

    bool ended() const;

private:
    void updateDiamond(float dt, Diamond& diamond);

    std::vector<Diamond> m_diamonds;
    TextureComponent* m_texture;
};

#endif //EFFECTS_H
