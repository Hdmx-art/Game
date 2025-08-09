//
// Created by Rayann CHOUCRALLAH on 27/07/2025.
//

#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "Drawable.h"
#include "Components.h"

class Background final : public Drawable {
public:
    Background();

    void update(float dt) override;
    void render(SDL_Renderer *pRenderer) override;

    void reset(TextureID id) const;
private:

    TextureComponent* m_backgroundTexture;

    std::vector<SDL_FRect> m_tiles;
};

#endif //BACKGROUND_H
