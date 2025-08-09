//
// Created by Rayann CHOUCRALLAH on 03/08/2025.
//

#include "Drawable.h"
#include "Config.h"
#include "Components.h"

Drawable::~Drawable() {
}

void Drawable::debugRender(SDL_Renderer *pRenderer) {
    if (DEBUG_STATE) {
        if (getComponent<TransformComponent>()) {
            SDL_CHECK(SDL_SetRenderDrawColor(pRenderer, 0, 255, 0, 255));
            SDL_CHECK(SDL_RenderRect(pRenderer, &getComponent<TransformComponent>()->rect));
        }
        if (getComponent<ColliderComponent>()) {
            getComponent<ColliderComponent>()->debugRender(pRenderer);
        }
    }
}
