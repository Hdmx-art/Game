//
// Created by Rayann CHOUCRALLAH on 17/07/2025.
//

#ifndef DRAWABLE_H
#define DRAWABLE_H

#include "Entity.h"

struct SDL_Renderer;

class Drawable : public Entity {
public:
    Drawable() = default;
    virtual ~Drawable();

    virtual void render(SDL_Renderer* pRenderer) = 0;
protected:
    void debugRender(SDL_Renderer* pRenderer);

};

#endif //DRAWABLE_H
