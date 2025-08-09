//
// Created by Rayann CHOUCRALLAH on 03/08/2025.
//

#include "World.h"
#include "SoundManager.h"

World::World() {
    m_player = nullptr;
}

World::~World() {
    for (auto& item : m_items) {
        delete item;
    }
    m_items.clear();

    for (auto& effect : m_effects) {
        delete effect;
    }
    m_effects.clear();
}

void World::init() {
    m_transitionEffect.reset();
    m_map.load(MapID::DEBUG);
}

void World::loadMap(MapID mapID) {
    m_transitionEffect.reset();
    m_items.clear();
    m_traps.clear();
    m_effects.clear();
    m_enemies.clear();

    if (m_player) {
        delete m_player;
        m_player = nullptr;
    }

    m_map.load(mapID);
}

void World::update(float dt) {
    m_map.update(dt);

    for (const auto& item : m_items)
        item->update(dt);

    for (const auto& trap : m_traps)
        trap->update(dt);

    for (const auto& enemy : m_enemies)
        enemy->update(dt);

    updateEffects(dt);
    m_player->update(dt);

    if (m_items.empty())
        loadMap(MapID::LEVEL_1);
}

void World::render(SDL_Renderer *pRenderer) {
    m_map.render(pRenderer);

    for (const auto& item : m_items)
        item->render(pRenderer);

    for (const auto& trap : m_traps)
        trap->render(pRenderer);

    for (const auto& enemy : m_enemies)
        enemy->render(pRenderer);

    for (const auto& effect : m_effects)
        effect->render(pRenderer);

    m_player->render(pRenderer);
    m_transitionEffect.render(pRenderer);
}

Player * World::createPlayer(const float x, const float y) {
    m_player = new Player();
    m_player->getComponent<TransformComponent>()->move(x, y - m_player->getComponent<TransformComponent>()->rect.h);
    return m_player;
}

Item * World::createItem(Item *item, const float x, const float y) {
    item->getComponent<TransformComponent>()->move(x, y - item->getComponent<TransformComponent>()->rect.h);
    m_items.push_back(std::move(item));
    return item;
}

void World::destroyItem(Item *item, const int32_t index) {
    DesappearingEffect* effect = new DesappearingEffect(item->getComponent<TransformComponent>()->center().x,
                                              item->getComponent<TransformComponent>()->center().y);

    effect->getComponent<TransformComponent>()->move(-effect->getComponent<TransformComponent>()->rect.w / 2,
                                                     -effect->getComponent<TransformComponent>()->rect.h / 2);
    m_effects.push_back(effect);

    SoundManager::getInstance().playSound(SoundID::COLLECT_FRUIT);

    delete item;
    m_items.erase(m_items.begin() + index);
}

void World::destroyTrap(Trap *trap, int32_t index) {
    if (index > 0){
        delete trap;
        m_traps.erase(m_traps.begin() + index);
        return;
    }

    m_traps.erase(std::find(m_traps.begin(), m_traps.end(), trap));
}

void World::destroyEnemy(Enemy *enemy, int32_t index) {
    if (index > 0){
        delete enemy;
        m_enemies.erase(m_enemies.begin() + index);
        return;
    }

    m_enemies.erase(std::find(m_enemies.begin(), m_enemies.end(), enemy));
}

Item * World::getItem(const int32_t index) const {
    return m_items[index];
}

int32_t World::getItemCount() const {
    return static_cast<int32_t>(m_items.size());
}

Trap * World::getTrap(const int32_t index) const {
    return m_traps[index];
}

int32_t World::getTrapCount() const {
    return m_traps.size();
}

Enemy * World::getEnemy(const int32_t index) const {
    return m_enemies[index];
}

int32_t World::getEnemyCount() const {
    return m_enemies.size();
}

Map & World::getMap() {
    return m_map;
}

bool World::isTransitioning() const {
    return !m_transitionEffect.ended();
}

void World::updateEffects(const float dt) {
    m_transitionEffect.update(dt);

    for (int32_t i = 0; i < static_cast<int32_t>(m_effects.size()); ) {
        if (m_effects[i]->ended()) {
            delete m_effects[i];
            m_effects.erase(m_effects.begin() + i);
        } else {
            m_effects[i]->update(dt);
            ++i;
        }
    }
}
