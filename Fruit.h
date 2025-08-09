//
// Created by Rayann CHOUCRALLAH on 23/07/2025.
//

#ifndef COIN_H
#define COIN_H

#include "Item.h"
#include "Components.h"

enum class FruitType {
    APPLE,
    BANANAS,
    CHERRIES,
    KIWI,
    MELON,
    ORANGE,
    PINEAPPLE,
    STRAWBERRY
};

class Fruit final : public Item {
public:
    Fruit();
    ~Fruit();

    void setType(TextureID type);

    void update(float dt) override;
    void render(SDL_Renderer *pRenderer) override;

private:
    TransformComponent* m_transform;
    TextureComponent* m_texture;
    AnimatorComponent* m_animator;
    ColliderComponent* m_collider;

    void initRessources() const;
};



#endif //COIN_H
