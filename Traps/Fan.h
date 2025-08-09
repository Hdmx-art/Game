//
// Created by Rayann CHOUCRALLAH on 31/07/2025.
//

#ifndef FAN_H
#define FAN_H

#include "Trap.h"
#include "../Components.h"

class Fan final : public Trap {
public:
    enum class FanType {
        VERTICAL,
        HORIZONTAL
    };

    Fan();
    ~Fan() override;

    bool overlap(const ColliderComponent* pColl) const;
    float distanceFromFan(const ColliderComponent* pColl) const;
    void setType(const FanType type);

    void update(float dt) override;
    void render(SDL_Renderer *pRenderer) override;

    Fan::FanType getType() const;

private:
    TransformComponent* m_transform;
    AnimatorComponent* m_animator;
    TextureComponent* m_texture;

    SDL_FRect m_projection;
    FanType m_type;
    float m_duration, m_elapsedTime, m_resetTimer;
};

#endif //FAN_H
