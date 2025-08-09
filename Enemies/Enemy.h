//
// Created by Rayann CHOUCRALLAH on 03/08/2025.
//

#ifndef ENEMY_H
#define ENEMY_H

#include "../Drawable.h"
#include <SDL3/SDL_rect.h>

class Enemy : public Drawable {
public:
    Enemy(): m_oldVel(0), m_newVel(0) {
        m_velocity = {150.f, 500.f};
    }

    virtual bool isDying() const = 0;
    bool isDead();

protected:
    void applyPhysics(float dt);
    void setHorizontalVelocity(float velocity);

private:
    void applyAlivePhysics(float dt);
    void applyDyingPhysics(float dt);

    SDL_FPoint m_velocity;
    float m_accelerationY = 1500.f;
    int32_t m_direction = 1;

    float m_oldVel, m_newVel;
    float m_elapsedTime = 0.f;
};

#endif //ENNEMY_H
