#include "Enemy.h"
#include "../Core.h"
#include "../Map.h"

namespace {
    inline bool isSolid(int tileID) { return tileID > 0; }
    inline int  tileIndex(float coord) { return static_cast<int>(coord / TILE_SIZEF); }
}

bool Enemy::isDead() {
    if (getComponent<TransformComponent>() == nullptr)
        return false;

    // TODO : dynamic height of screen
    return (getComponent<TransformComponent>()->rect.y > 720.f);
}

void Enemy::applyPhysics(float dt) {
    if (isDying())
        applyDyingPhysics(dt);
    else
        applyAlivePhysics(dt);
}

void Enemy::setHorizontalVelocity(float velocity) {
    m_velocity.x = velocity;

    if (m_velocity.x > 0.f)
        getComponent<TextureComponent>()->flip = SDL_FLIP_HORIZONTAL;
}

void Enemy::applyAlivePhysics(float dt) {
    const auto& map = Core::getInstance().getWorld().getMap();
    auto* transform = getComponent<TransformComponent>();
    PTR_CHECK(transform);

    m_velocity.y += m_accelerationY * dt;

    const float leftX  = transform->rect.x + 1.f;
    const float rightX = transform->rect.x + transform->rect.w - 1.f;
    const float footY  = transform->rect.y + transform->rect.h;

    const int leftTileX  = tileIndex(leftX);
    const int rightTileX = tileIndex(rightX);
    const int footTileY  = tileIndex(footY);

    if (isSolid(map.getTileAt(leftTileX, footTileY)) ||
        isSolid(map.getTileAt(rightTileX, footTileY))) {
        m_velocity.y = 0.f;
        transform->rect.y = footTileY * TILE_SIZEF - transform->rect.h;}

    // Collision obstacles (tête & centre)
    const int headTileY   = tileIndex(transform->rect.y);
    const int centerTileY = tileIndex(transform->rect.y + transform->rect.h / 2.f);
    const bool obstacleAhead =
        isSolid(map.getTileAt(leftTileX, headTileY))   ||
        isSolid(map.getTileAt(rightTileX, headTileY))  ||
        isSolid(map.getTileAt(leftTileX, centerTileY)) ||
        isSolid(map.getTileAt(rightTileX, centerTileY));

    if (obstacleAhead) {
        if (m_direction < 0) {
            getComponent<TextureComponent>()->flip = SDL_FLIP_HORIZONTAL;
            m_direction = 1.f;
            transform->rect.x = leftTileX * TILE_SIZEF + TILE_SIZEF;
        }
        else {
            getComponent<TextureComponent>()->flip = SDL_FLIP_NONE;
            m_direction = -1.f;
            transform->rect.x = rightTileX * TILE_SIZEF - transform->rect.w;
        }
    }

    const float dx = m_velocity.x * dt * m_direction;
    const float dy = m_velocity.y * dt;
    transform->move(dx, dy);
}

void Enemy::applyDyingPhysics(float dt) {
}
