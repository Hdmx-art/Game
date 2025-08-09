#include "Player.h"
#include "Core.h"
#include "World.h"
#include "Map.h"
#include "Item.h"
#include "SoundManager.h"
#include "Config.h"

namespace {
constexpr float kEps = 0.001f;
constexpr float kProbeInset = 2.0f;
inline int tileCol(float x) { return static_cast<int>(std::floor(x / TILE_SIZEF)); }
inline int tileRow(float y) { return static_cast<int>(std::floor(y / TILE_SIZEF)); }
} // namespace

// -----------------------------------------------------------------------------
// Construction / destruction
// -----------------------------------------------------------------------------

Player::Player() {
    initComponents();
    initResources();
    initAnimations();
    m_animator->currentAnimation = APPEARING;
}

Player::~Player() {
    destroyComponents();
}

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

void Player::initComponents() {
    m_transform = addComponent<TransformComponent>();
    m_transform->rect = {0.0f, 0.0f, 64.0f, 64.0f};

    m_collider = addComponent<ColliderComponent>();

    m_texture = addComponent<TextureComponent>();
    m_texture->rect = {0, 0, 32, 32};
    m_texture->textureID = TextureID::NONE;

    m_animator = addComponent<AnimatorComponent>();
}

void Player::initResources() const {
    m_texture->loadTextureFromFile(TextureID::NINJA_FROG_IDLE,        "assets/Main_Characters/Ninja_Frog/Idle (32x32).png");
    m_texture->loadTextureFromFile(TextureID::NINJA_FROG_RUN,         "assets/Main_Characters/Ninja_Frog/Run (32x32).png");
    m_texture->loadTextureFromFile(TextureID::NINJA_FROG_FALLING,     "assets/Main_Characters/Ninja_Frog/Fall (32x32).png");
    m_texture->loadTextureFromFile(TextureID::NINJA_FROG_JUMPING,     "assets/Main_Characters/Ninja_Frog/Jump (32x32).png");
    m_texture->loadTextureFromFile(TextureID::NINJA_FROG_HIT,         "assets/Main_Characters/Ninja_Frog/Hit (32x32).png");
    m_texture->loadTextureFromFile(TextureID::NINJA_FROG_DOUBLE_JUMP, "assets/Main_Characters/Ninja_Frog/Double Jump (32x32).png");
    m_texture->loadTextureFromFile(TextureID::PLAYER_APPEARING,       "assets/Main_Characters/Appearing (96x96).png");
    m_texture->loadTextureFromFile(TextureID::NINJA_FROG_SLIDE,       "assets/Main_Characters/Ninja_Frog/Wall Jump (32x32).png");

    SoundManager::getInstance().loadSoundFromFile(SoundID::JUMP, "assets/Sounds/Jump.wav");
}

void Player::initAnimations() const {
    m_animator->addAnimation(RUN,         {{32.f, 32.f}, 12, 0.80f, true,  TextureID::NINJA_FROG_RUN});
    m_animator->addAnimation(IDLE,        {{32.f, 32.f}, 11, 0.80f, true,  TextureID::NINJA_FROG_IDLE});
    m_animator->addAnimation(JUMPING,     {{32.f, 32.f},  1, 1.00f, true,  TextureID::NINJA_FROG_JUMPING});
    m_animator->addAnimation(FALLING,     {{32.f, 32.f},  1, 1.00f, true,  TextureID::NINJA_FROG_FALLING});
    m_animator->addAnimation(HIT,         {{32.f, 32.f},  7, 1.00f, false, TextureID::NINJA_FROG_HIT});
    m_animator->addAnimation(DOUBLE_JUMP, {{32.f, 32.f},  6, 0.35f, false, TextureID::NINJA_FROG_DOUBLE_JUMP});
    m_animator->addAnimation(APPEARING,   {{96.f, 96.f},  7, 0.55f, false, TextureID::PLAYER_APPEARING});
    m_animator->addAnimation(SLIDE,       {{32.f, 32.f},  4, 0.70f, true,  TextureID::NINJA_FROG_SLIDE});
}

// -----------------------------------------------------------------------------
// Update / Render
// -----------------------------------------------------------------------------

void Player::update(float dt) {
    // Apparition
    if (m_animator->currentAnimation == APPEARING) {
        if (!Core::getInstance().getWorld().isTransitioning()) {
            m_animator->run(m_texture, dt);
        }
        if (m_animator->ended()) m_animator->currentAnimation = FALLING;
        return;
    }

    if (m_dead) {
        m_animator->currentAnimation = HIT;
        if (!Core::getInstance().getWorld().isTransitioning()) {
            m_animator->run(m_texture, dt);
        }
        if (m_animator->ended()) {
            Core::getInstance().getWorld().loadMap(MapID::LEVEL_1);
        }
        return;
    }

    // Maj collider + coord tuile
    m_coord = {
        static_cast<int>(m_transform->center().x / TILE_SIZEF),
        static_cast<int>(m_transform->center().y / TILE_SIZEF)
    };
    m_collider->inflate(m_transform, 0.72f, 1.0f);

    // Timers saut
    if (m_onGround) m_coyoteTime = m_coyoteTimeMax; else m_coyoteTime = std::max(0.0f, m_coyoteTime - dt);
    m_jumpBuffer = std::max(0.0f, m_jumpBuffer - dt);

    // 1) Entrées (course/inertie/jump buffer/skid)
    stepInput(dt);

    // 2) Détection des murs (avant gravité pour moduler)
    detectWalls(dt);

    // 3) Gravité (adaptative + wall slide)
    applyGravity(dt);

    // 4) Intégration + collisions
    integrateAndCollide(dt);

    // 5) Interactions
    trapInteraction(dt);
    enemyInteraction(dt);
    collectItems();

    // 6) Animations / FX
    updateFacing();
    updateAnimation();
    if (!Core::getInstance().getWorld().isTransitioning()) {
        m_animator->run(m_texture, dt);
    }
}

void Player::render(SDL_Renderer* pRenderer) {
    if (m_texture->textureID == TextureID::NONE) return;

    // On rend dans un rect local pour pouvoir corriger l’offset visuel
    SDL_FRect dst = m_transform->rect;

    // Si on est en slide (ou dans sa petite fenêtre coyote mur), on plaque visuellement la texture au mur.
    // Ecart latéral entre le visuel (transform) et le collider :
    //   inset = (largeur visuelle - largeur collider) / 2
    if ((m_onWall || m_wallStickTimer > 0.0f) && m_velocity.y >= 0.0f) {
        const float inset = (m_transform->rect.w - m_collider->rect.w) * 0.8f;
        if (m_wallDir > 0) {
            // Mur à droite -> pousse l'image vers la droite
            dst.x += inset;
        } else if (m_wallDir < 0) {
            // Mur à gauche -> pousse l'image vers la gauche
            dst.x -= inset;
        }
    }

    SDL_CHECK(SDL_RenderTextureRotated(
        pRenderer,
        m_texture->getTexture(),
        &m_texture->rect,
        &dst,                           // <-- on rend avec le rect corrigé
        m_transform->rotation,
        nullptr,
        m_texture->flip));

    debugRender(pRenderer);
}

// -----------------------------------------------------------------------------
// Physique façon Mario + Wall slide/jump
// -----------------------------------------------------------------------------

void Player::stepInput(float dt) {
    if (Core::getInstance().getWorld().isTransitioning()) return;

    const bool* kb = SDL_GetKeyboardState(nullptr);
    const bool right = kb[SDL_SCANCODE_D] != 0;
    const bool left  = kb[SDL_SCANCODE_A] != 0;
    const bool run   = kb[SDL_SCANCODE_LSHIFT] != 0;
    const bool jump  = kb[SDL_SCANCODE_SPACE] != 0;

    // Gestion course
    if ((right ^ left) && run) m_runFactor = std::min(1.0f, m_runFactor + m_runBuildRate * dt);
    else                       m_runFactor = std::max(0.0f, m_runFactor - m_runDecayRate * dt);

    const float maxSpeed = m_maxWalkSpeed + (m_maxRunSpeed - m_maxWalkSpeed) * m_runFactor;

    // Jump buffer
    if (jump && !m_jumpHeldPrev) {
        m_jumpBuffer = m_jumpBufferMax;
    }

    // Direction
    float want = 0.0f;
    if (right ^ left) want = right ? 1.0f : -1.0f;

    // Demi-tour (skid)
    const bool turning = (want != 0.0f) && ((m_velocity.x > 0.0f && want < 0.0f) || (m_velocity.x < 0.0f && want > 0.0f));
    if (m_onGround && turning && std::fabs(m_velocity.x) > m_skidSpeedThresh) {
        const float sign = m_velocity.x > 0.0f ? 1.0f : -1.0f;
        m_velocity.x -= sign * m_turnDecel * dt;
        if ((m_velocity.x > 0.0f && sign < 0.0f) || (m_velocity.x < 0.0f && sign > 0.0f)) {
            m_velocity.x = 0.0f;
        }
    } else {
        // Accélère vers la cible
        const float target = want * maxSpeed;
        const float accel  = m_onGround ? m_groundAccel : m_airAccel;
        const float delta  = target - m_velocity.x;
        const float step   = accel * dt;

        if      (delta >  step) m_velocity.x += step;
        else if (delta < -step) m_velocity.x -= step;
        else                    m_velocity.x  = target;

        // Friction au sol sans entrée
        if (m_onGround && want == 0.0f) {
            if (m_velocity.x > 0.0f) m_velocity.x = std::max(0.0f, m_velocity.x - m_groundFriction * dt);
            if (m_velocity.x < 0.0f) m_velocity.x = std::min(0.0f, m_velocity.x + m_groundFriction * dt);
        }
    }

    // Consommer un saut (inclut wall jump)
    consumeJumpIfReady();

    // Short hop
    if (!jump && m_jumpHeldPrev && m_velocity.y < 0.0f) {
        m_velocity.y *= 0.5f;
    }

    m_jumpHeldPrev = jump;
}

void Player::detectWalls(float dt) {
    // Éligible uniquement en l’air
    if (m_onGround) {
        m_onWall = false;
        m_wallDir = 0;
        m_wallStickTimer = 0.0f;
        return;
    }

    const bool* kb = SDL_GetKeyboardState(nullptr);
    const bool pushLeft  = kb[SDL_SCANCODE_A] != 0;
    const bool pushRight = kb[SDL_SCANCODE_D] != 0;

    auto& r = m_transform->rect;
    const int rowTop    = tileRow(r.y + kProbeInset);
    const int rowBottom = tileRow(r.y + r.h - kProbeInset);
    const int colLeft   = tileCol(r.x - kProbeInset);
    const int colRight  = tileCol(r.x + r.w + kProbeInset);

    const bool touchLeft  = colHasSolid(rowTop, rowBottom, colLeft);
    const bool touchRight = colHasSolid(rowTop, rowBottom, colRight);

    // On “tient” le mur seulement si on pousse vers lui et qu’on descend
    bool canSlide = (m_velocity.y >= 0.0f) && ((touchLeft && pushLeft) || (touchRight && pushRight));

    if (canSlide) {
        m_onWall = true;
        m_wallDir = touchLeft ? -1 : +1;
        m_wallStickTimer = m_wallStickMax; // refresh du coyote mur
    } else {
        // On n’est plus en contact direct : coyote mur décrémente
        m_onWall = false;
        m_wallDir = 0;
        m_wallStickTimer = std::max(0.0f, m_wallStickTimer - dt);
    }
}

void Player::applyGravity(float dt) {
    float g = m_gravity;

    // Apex allégé
    if (std::fabs(m_velocity.y) < m_apexVy) g *= m_apexGravityScale;

    // Chute plus lourde
    if (m_velocity.y > 0.0f) g *= m_fallGravityMul;

    // Relâche du saut pendant montée
    const bool* kb = SDL_GetKeyboardState(nullptr);
    const bool jumpHeld = kb[SDL_SCANCODE_SPACE] != 0;
    if (m_velocity.y < 0.0f && !jumpHeld) g *= m_lowJumpMul;

    // Wall slide : gravité réduite et clamp de la descente
    if (m_onWall && m_velocity.y >= 0.0f) {
        g *= m_wallSlideGravMul;
    }

    m_velocity.y += g * dt;
    if (m_onWall && m_velocity.y > m_wallSlideMaxSpeed) {
        m_velocity.y = m_wallSlideMaxSpeed;
    }
    if (m_velocity.y > m_maxFallSpeed) m_velocity.y = m_maxFallSpeed;
}

void Player::integrateAndCollide(float dt) {
    const float dx = m_velocity.x * dt;
    const float dy = m_velocity.y * dt;

    resolveVertical(dy);
    resolveHorizontal(dx);

    m_collider->inflate(m_transform, 0.72f, 1.0f);
}

void Player::resolveVertical(float dy) {
    if (std::fabs(dy) < kEps) return;

    auto& r = m_transform->rect;

    if (dy > 0.0f) {
        const float startBottom = r.y + r.h;
        const int startRow = tileRow(startBottom - kProbeInset);
        const int endRow   = tileRow(startBottom + dy);
        const int colL     = tileCol(r.x + kProbeInset);
        const int colR     = tileCol(r.x + r.w - kProbeInset);

        bool landed = false;
        for (int row = startRow + 1; row <= endRow; ++row) {
            if (rowHasSolid(colL, colR, row)) {
                r.y = row * TILE_SIZEF - r.h;
                m_velocity.y = 0.0f;
                onLand(row * TILE_SIZEF);
                landed = true;
                break;
            }
        }
        if (!landed) {
            r.y += dy;
            m_onGround = false;
        }
    } else {
        const float startTop = r.y;
        const int startRow = tileRow(startTop + kProbeInset);
        const int endRow   = tileRow(startTop + dy);
        const int colL     = tileCol(r.x + kProbeInset);
        const int colR     = tileCol(r.x + r.w - kProbeInset);

        bool bonk = false;
        for (int row = startRow - 1; row >= endRow; --row) {
            if (rowHasSolid(colL, colR, row)) {
                r.y = (row + 1) * TILE_SIZEF;
                onBonk((row + 1) * TILE_SIZEF);
                bonk = true;
                break;
            }
        }
        if (!bonk) r.y += dy;
    }
}

void Player::resolveHorizontal(float dx) {
    if (std::fabs(dx) < kEps) return;

    auto& r = m_transform->rect;

    const int rowTop    = tileRow(r.y + kProbeInset);
    const int rowBottom = tileRow(r.y + r.h - kProbeInset);

    if (dx > 0.0f) {
        const float startRight = r.x + r.w;
        const int startCol = tileCol(startRight - kProbeInset);
        const int endCol   = tileCol(startRight + dx);

        bool blocked = false;
        for (int col = startCol + 1; col <= endCol; ++col) {
            if (colHasSolid(rowTop, rowBottom, col)) {
                r.x = col * TILE_SIZEF - r.w;
                m_velocity.x = 0.0f;
                blocked = true;
                break;
            }
        }
        if (!blocked) r.x += dx;
    } else {
        const float startLeft = r.x;
        const int startCol = tileCol(startLeft + kProbeInset);
        const int endCol   = tileCol(startLeft + dx);

        bool blocked = false;
        for (int col = startCol - 1; col >= endCol; --col) {
            if (colHasSolid(rowTop, rowBottom, col)) {
                r.x = (col + 1) * TILE_SIZEF;
                m_velocity.x = 0.0f;
                blocked = true;
                break;
            }
        }
        if (!blocked) r.x += dx;
    }
}

// -----------------------------------------------------------------------------
// Collisions utilitaires
// -----------------------------------------------------------------------------

bool Player::rowHasSolid(int colStart, int colEnd, int row) const {
    if (colStart > colEnd) std::swap(colStart, colEnd);
    const Map& map = Core::getInstance().getWorld().getMap();
    for (int c = colStart; c <= colEnd; ++c) {
        if (map.getTileAt(c, row) > 0) return true;
    }
    return false;
}

bool Player::colHasSolid(int rowStart, int rowEnd, int col) const {
    if (rowStart > rowEnd) std::swap(rowStart, rowEnd);
    const Map& map = Core::getInstance().getWorld().getMap();
    for (int r = rowStart; r <= rowEnd; ++r) {
        if (map.getTileAt(col, r) > 0) return true;
    }
    return false;
}

// -----------------------------------------------------------------------------
// Sauts (inclut Wall Jump)
// -----------------------------------------------------------------------------

void Player::consumeJumpIfReady() {
    // 1) Wall jump prioritaire si coyote mur actif
    if (m_jumpBuffer > 0.0f && !m_onGround && (m_onWall || m_wallStickTimer > 0.0f)) {
        int dir = m_wallDir;
        if (dir == 0) dir = -1; // fallback : pousse à gauche par défaut si indéterminé

        m_onWall = false;
        m_wallStickTimer = 0.0f;
        m_canDoubleJump = true; // réarme le double saut après un wall jump

        m_velocity.y = -std::fabs(m_wallJumpVVel);
        m_velocity.x = static_cast<float>(-dir) * m_wallJumpHVel;

        m_animator->currentAnimation = JUMPING;
        SoundManager::getInstance().playSound(SoundID::JUMP);

        m_jumpBuffer = 0.0f;
        return;
    }

    // 2) Saut normal (ground/coyote)
    if (m_jumpBuffer > 0.0f && (m_onGround || m_coyoteTime > 0.0f)) {
        const float vel = m_baseJumpVel + m_runJumpBonus * m_runFactor;
        startJump(vel);
        m_jumpBuffer = 0.0f;
        return;
    }

    // 3) Double saut
    const bool* kb = SDL_GetKeyboardState(nullptr);
    const bool jump = kb[SDL_SCANCODE_SPACE] != 0;
    const bool pressed = jump && !m_jumpHeldPrev;

    if (pressed && !m_onGround && m_canDoubleJump) {
        startJump(m_doubleJumpVel);
        m_canDoubleJump = false;
        m_animator->currentAnimation = DOUBLE_JUMP;
        SoundManager::getInstance().playSound(SoundID::JUMP);
    }
}

void Player::startJump(float vel) {
    m_onGround = false;
    m_velocity.y = -std::fabs(vel);
    m_coyoteTime = 0.0f;
    m_animator->currentAnimation = JUMPING;
    SoundManager::getInstance().playSound(SoundID::JUMP);
}

void Player::onLand(float /*platformTopY*/) {
    m_onGround = true;
    m_onWall = false;
    m_wallDir = 0;
    m_wallStickTimer = 0.0f;
    m_canDoubleJump = true;
    m_velocity.y = 0.0f;
}

void Player::onBonk(float /*ceilingBottomY*/) {
    m_velocity.y = 0.0f;
}

void Player::die() {
    if (m_dead) return;
    m_dead = true;
    SoundManager::getInstance().playSound(SoundID::HURT);
}

// -----------------------------------------------------------------------------
// Animations & VFX
// -----------------------------------------------------------------------------

void Player::updateFacing() {
    // En slide: le joueur doit faire face AU mur (pas l'inverse)
    if (m_onWall && m_velocity.y >= 0.0f) {
        // m_wallDir = -1 (mur à gauche) -> face à gauche
        // m_wallDir = +1 (mur à droite) -> face à droite
        m_facingRight = (m_wallDir > 0);
    } else if (std::fabs(m_velocity.x) > 1.0f) {
        // Hors slide, oriente selon la vitesse horizontale
        m_facingRight = (m_velocity.x >= 0.0f);
    }
    m_texture->flip = m_facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
}

void Player::updateAnimation() {
    if (m_dead) { m_animator->currentAnimation = HIT; return; }

    // Laisse l’anim de double saut se jouer
    if (m_animator->currentAnimation == DOUBLE_JUMP && !m_animator->ended()) return;

    if (!m_onGround) {
        if (m_onWall && m_velocity.y >= 10.0f) {
            m_animator->currentAnimation = SLIDE;
        } else {
            m_animator->currentAnimation = (m_velocity.y > 50.0f) ? FALLING : JUMPING;
        }
        return;
    }

    const float speed = std::fabs(m_velocity.x);
    m_animator->currentAnimation = (speed > 20.0f) ? RUN : IDLE;
}

void Player::spawnRunDust() {
    if (!m_onGround) return;
    const float dir = m_facingRight ? -1.0f : 1.0f;
    m_dustEmitter->emit(m_transform->rect.x + dir * 18.0f, m_transform->bottom(), static_cast<int>(dir));
}

void Player::spawnSkidDust() {
    if (!m_onGround) return;
    m_skidDustTimer -= 0.016f;
    if (m_skidDustTimer <= 0.0f) {
        const float dir = m_facingRight ? 1.0f : -1.0f;
    }
}

void Player::spawnLandDust() {
}

// -----------------------------------------------------------------------------
// Interactions monde
// -----------------------------------------------------------------------------

void Player::collectItems() const {
    auto& world = Core::getInstance().getWorld();
    for (int32_t i = static_cast<int32_t>(world.getItemCount()) - 1; i >= 0; --i) {
        Item* item = world.getItem(i);
        if (m_collider->collide(item->getComponent<ColliderComponent>())) {
            world.destroyItem(item, i);
        }
    }
}

void Player::trapInteraction(float dt) {
    const auto& world = Core::getInstance().getWorld();
    for (int32_t i = 0; i < world.getTrapCount(); ++i) {
        Trap* trap = world.getTrap(i);

        if (auto* trampoline = dynamic_cast<Trampoline*>(trap)) {
            handleTrampolineInteraction(trampoline);
        } else if (const auto* fan = dynamic_cast<Fan*>(trap)) {
            handleFanInteraction(fan, dt);
        } else if (auto* arrow = dynamic_cast<Arrow*>(trap)) {
            handleArrowInteraction(arrow);
        } else if (auto* fp = dynamic_cast<FallingPlatform*>(trap)) {
            handleFallingPlatformInteraction(fp);
        } else if (auto* fire = dynamic_cast<Fire*>(trap)) {
            handleFireInteraction(fire);
        }
    }
}

void Player::enemyInteraction(float /*dt*/) {
    const auto& world = Core::getInstance().getWorld();
    for (int32_t i = 0; i < world.getEnemyCount(); ++i) {
        Enemy* enemy = world.getEnemy(i);
        if (enemy->isDying()) continue;

        if (auto* slime = dynamic_cast<Slime*>(enemy)) {
            handleSlimeInteraction(slime);
        } else if (auto* pig = dynamic_cast<AngryPig*>(enemy)) {
            handleAngryPigInteraction(pig);
        } else if (auto* turtle = dynamic_cast<Turtle*>(enemy)) {
            handleTurtleInteraction(turtle);
        }
    }
}

// -----------------------------------------------------------------------------
// Traps
// -----------------------------------------------------------------------------

void Player::handleTrampolineInteraction(Trampoline* trampoline) {
    if (m_collider->collide(trampoline->getComponent<ColliderComponent>())) {
        m_onGround = false;
        m_canDoubleJump = true;
        startJump(1000.0f);
        trampoline->onJump();
    }
}

void Player::handleFanInteraction(const Fan* fan, float dt) {
    if (!fan->overlap(m_collider)) return;

    const Map& map = Core::getInstance().getWorld().getMap();

    if (fan->getType() == Fan::FanType::VERTICAL) {
        m_onGround = false;
        const float mult = fan->distanceFromFan(m_collider);
        m_velocity.y -= dt * m_gravity * 4.0f * mult;
    } else {
        const int checkCol = tileCol(m_transform->center().x + TILE_SIZEF);
        const int row      = tileRow(m_transform->center().y);
        if (map.getTileAt(checkCol, row) == 0) {
            m_velocity.x += 600.0f * dt * fan->distanceFromFan(m_collider);
        }
    }
}

void Player::handleArrowInteraction(Arrow* arrow) {
    if (arrow->used()) return;

    if (SDL_HasRectIntersectionFloat(&m_collider->rect, &arrow->getComponent<TransformComponent>()->rect)) {
        arrow->onHit();
        m_canDoubleJump = true;

        switch (arrow->getType()) {
            case Arrow::ArrowType::UP:    startJump(700.0f); break;
            case Arrow::ArrowType::DOWN:  m_velocity.y = 700.0f; m_onGround = false; break;
            case Arrow::ArrowType::RIGHT: m_velocity.x = 600.0f; break;
            case Arrow::ArrowType::LEFT:  m_velocity.x = -600.0f; break;
        }
    }
}

void Player::handleFallingPlatformInteraction(FallingPlatform* fp) {
    const auto* t = fp->getComponent<TransformComponent>();
    if (SDL_HasRectIntersectionFloat(&m_collider->rect, &t->rect)) {
        if (m_transform->bottom() <= t->rect.y + 10.0f) {
            onLand(t->rect.y);
            fp->onHit();
            m_transform->rect.y = t->rect.y - m_transform->rect.h + 1.0f;
        }
    }
}

void Player::handleFireInteraction(Fire* fire) {
    if (fire->overlapFire(m_collider) && fire->activated()) {
        die();
        return;
    }

    if (m_collider->collide(fire->getComponent<ColliderComponent>())) {
        if (m_transform->bottom() <= fire->getComponent<ColliderComponent>()->rect.y + 10.0f) {
            onLand(fire->getComponent<ColliderComponent>()->rect.y);
            fire->onHit(true);
            m_transform->rect.y = fire->getComponent<ColliderComponent>()->rect.y - m_transform->rect.h + 1.0f;
        }
    } else {
        fire->onHit(false);
    }
}

// -----------------------------------------------------------------------------
// Enemies
// -----------------------------------------------------------------------------

void Player::handleSlimeInteraction(Slime* slime) {
    const auto* st = slime->getComponent<TransformComponent>();
    if (SDL_HasRectIntersectionFloat(&m_collider->rect, &st->rect)) {
        if (m_transform->bottom() <= st->rect.y + 10.0f) {
            m_transform->rect.y = st->rect.y - m_transform->rect.h;
            startJump(750.0f);
            slime->onJump();
        } else {
            die();
        }
    }
}

void Player::handleAngryPigInteraction(AngryPig* pig) {
    const auto* pt = pig->getComponent<TransformComponent>();
    if (SDL_HasRectIntersectionFloat(&m_collider->rect, &pt->rect)) {
        if (m_transform->bottom() <= pt->rect.y + 10.0f) {
            m_transform->rect.y = pt->rect.y - m_transform->rect.h;
            startJump(750.0f);
            pig->onJump();
        } else {
            die();
        }
    }
}

void Player::handleTurtleInteraction(Turtle* turtle) {
    const auto* tt = turtle->getComponent<TransformComponent>();
    if (SDL_HasRectIntersectionFloat(&m_collider->rect, &tt->rect)) {
        if (turtle->spikeState() == Turtle::SpikesState::OUT) {
            die();
            return;
        }
        if (m_transform->bottom() <= tt->rect.y + 10.0f && turtle->spikeState() == Turtle::SpikesState::IN) {
            m_transform->rect.y = tt->rect.y - m_transform->rect.h;
            startJump(750.0f);
            turtle->onJump();
        }
        else {
            die();
        }
    }
}