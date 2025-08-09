//
// Created by Rayann CHOUCRALLAH on 07/08/2025.
//

#ifndef FIRE_H
#define FIRE_H

#include "Trap.h"
#include "../Components.h"

class Fire final : public Trap{
    enum Animations {
        ON,
        OFF,
        HIT
    };
public:
    Fire();

    void onHit(bool activate) const;
    bool overlapFire(const ColliderComponent* collide) const;

    void update(float dt) override;
    void render(SDL_Renderer* pRenderer) override;

    bool activated() const;

private:
    TransformComponent* m_transform;
    TextureComponent* m_texture;
    AnimatorComponent* m_animator;
    ColliderComponent* m_collider;

    void initResources() const;
    void initAnimation() const;

    SDL_FRect m_proj;
};



#endif //FIRE_H
