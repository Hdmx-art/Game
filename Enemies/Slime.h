//
// Created by Rayann CHOUCRALLAH on 03/08/2025.
//

#ifndef SLIME_H
#define SLIME_H

#include "Enemy.h"
#include "../Components.h"

class Slime final : public Enemy {
public:
    Slime();

    void onJump();

    void update(float dt) override;
    void render(SDL_Renderer *pRenderer) override;

    bool isDying() const override;

private:
    TransformComponent* m_transf;
    TextureComponent* m_texture;
    AnimatorComponent* m_animator;

    float m_timeSinceDeath;
    float m_deathTimer = 0.35f;
};



#endif //SLIME_H
