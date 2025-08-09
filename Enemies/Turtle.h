//
// Created by Rayann CHOUCRALLAH on 06/08/2025.
//

#ifndef TURTLE_H
#define TURTLE_H

#include "Enemy.h"
#include "../Components.h"

class Turtle final : public Enemy {
    enum Animation {
        IDLE1,
        SPIKES_IN,
        IDLE2,
        SPIKES_OUT,
        HIT
    };
public:
    enum class SpikesState {
        IN,
        OUT
    };

    Turtle();

    void onJump() const;

    void update(float dt) override;
    void render(SDL_Renderer* pRenderer) override;

    SpikesState spikeState() const;
    bool isDying() const override;

private:
    TransformComponent* m_transform;
    TextureComponent* m_texture;
    AnimatorComponent* m_animator;

    void initResources() const;
    void initAnimations() const;

    void updateAliveState(float dt);

    float m_spikesDelay = 2.f;
    float m_elapsedTime = 0.f;
};



#endif //TURTLE_H
