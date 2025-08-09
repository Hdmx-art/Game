//
// Created by Rayann CHOUCRALLAH on 29/07/2025.
//

#ifndef TRAMPOLINE_H
#define TRAMPOLINE_H

#include "Trap.h"
#include "../Components.h"

class Trampoline final : public Trap {
public:
    Trampoline();

    void onJump();

    void update(float dt) override;
    void render(SDL_Renderer *pRenderer) override;

    ~Trampoline() override;

private:
    TransformComponent* m_transform;
    AnimatorComponent* m_animator;
    TextureComponent* m_texture;
    ColliderComponent* m_collider;
};



#endif //TRAMPOLINE_H
