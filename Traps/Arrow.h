//
// Created by Rayann CHOUCRALLAH on 03/08/2025.
//

#ifndef ARROW_H
#define ARROW_H

#include "Trap.h"
#include "../Components.h"

class Arrow final : public Trap {
public:
    enum class ArrowType {
        UP,
        LEFT,
        DOWN,
        RIGHT
    };

    Arrow();

    void onHit();
    void setDirection(float rotation);

    void update(float dt) override;
    void render(SDL_Renderer *pRenderer) override;

    ArrowType getType() const;
    bool used() const;

private:
    TransformComponent* m_transform;
    TextureComponent* m_texture;
    AnimatorComponent* m_animator;

    ArrowType m_type;
    bool m_used;
};

#endif //ARROW_H
