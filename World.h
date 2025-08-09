//
// Created by Rayann CHOUCRALLAH on 03/08/2025.
//

#ifndef WORLD_H
#define WORLD_H

#include "Map.h"
#include "Player.h"
#include "Item.h"
#include "Enemies/Enemy.h"
#include "Effects.h"

#include <vector>

class World {
public:
    World();
    ~World();

    void init();
    void loadMap(MapID mapID);

    void update(float dt);
    void render(SDL_Renderer* pRenderer);

    Player* createPlayer(const float x, const float y);
    Item* createItem(Item* item, const float x, const float y);
    template<class T>
    T* creatTrap() {
        T* trap = new T();
        m_traps.push_back(std::move(trap));
        return trap;
    }

    template<class T>
    T* createEnemy() {
        T* enemy = new T();
        m_enemies.push_back(std::move(enemy));
        return enemy;
    }

    void destroyItem(Item* item, int32_t index);
    void destroyTrap(Trap* trap, int32_t index);
    void destroyEnemy(Enemy* enemy, int32_t index);

    Item* getItem(const int32_t index) const;
    int32_t getItemCount() const;

    Trap* getTrap(const int32_t index) const;
    int32_t getTrapCount() const;

    Enemy* getEnemy(const int32_t index) const;
    int32_t getEnemyCount() const;

    Map& getMap();
    bool isTransitioning() const;

private:
    Map m_map;
    Player* m_player;
    TransitionEffect m_transitionEffect;

    std::vector<Item*> m_items;
    std::vector<Trap*> m_traps;
    std::vector<Enemy*> m_enemies;
    std::vector<Effect*> m_effects;

    void updateEffects(float dt);
};

#endif //WORLD_H
