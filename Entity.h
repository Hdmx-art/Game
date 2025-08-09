//
// Created by Rayann CHOUCRALLAH on 17/07/2025.
//

#ifndef ENTITY_H
#define ENTITY_H

#include <unordered_set>

struct Component;

class Entity {
public:
    Entity() = default;
    virtual ~Entity() = default;

    virtual void update(float dt) = 0;

    template<class T>
    T* addComponent() {
        T* newComponent = new T();
        m_components.emplace(dynamic_cast<Component*>(newComponent));
        return newComponent;
    }

    template<class T>
    T* getComponent() {
        for (auto& component : m_components) {
            if (dynamic_cast<T*>(component) != nullptr)
                return dynamic_cast<T*>(component);
        }

        return nullptr;
    }

protected:
    void destroyComponents() const;

private:
    std::unordered_set<Component*> m_components;
};

#endif //ENTITY_H
