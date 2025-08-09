//
// Created by Rayann CHOUCRALLAH on 05/08/2025.
//

#ifndef ANGRYPIG_H
#define ANGRYPIG_H

#include "Enemy.h"
#include "../Components.h"

class AngryPig final : public Enemy {
    enum Animation {
        IDLE,
        WALKING,
        RUNNING,
        HIT1,
        HIT2
    };

public:
    AngryPig();
    ~AngryPig() override;

    void onJump();

    void update(float dt) override;
    void render(SDL_Renderer *pRenderer) override;

    bool isDying() const override;

private:
    TransformComponent* m_transform;
    TextureComponent* m_texture;
    AnimatorComponent* m_animator;

    void initResources() const;
    void initAnimations() const;
};



#endif //ANGRYPIG_H
