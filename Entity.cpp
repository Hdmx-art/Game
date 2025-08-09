//
// Created by Rayann CHOUCRALLAH on 03/08/2025.
//

#include "Entity.h"
#include "Components.h"

void Entity::destroyComponents() const{
    for (auto& component : m_components) {
        component->destroy();
        delete component;
    }
}