//
// Created by Rayann CHOUCRALLAH on 06/08/2025.
//

#ifndef FALLINGPLATEFORM_H
#define FALLINGPLATEFORM_H

#include "Trap.h"
#include "../Components.h"

class FallingPlatform final : public Trap {
    enum Animation {
        ON,
        OFF
    };

    enum class State {
        ON,
        OFF,
        FALLING
    };

public:
    FallingPlatform();

    void onHit();

    void update(float dt) override;
    void render(SDL_Renderer* pRenderer) override;

private:
    TransformComponent* m_transform;
    TextureComponent* m_texture;
    AnimatorComponent* m_animator;

    float m_time;
    float m_vely = 600.f;
    float m_scalar = 30.f;
    State m_state;
};



#endif //FALLINGPLATEFORM_H
