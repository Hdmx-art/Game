#pragma once

#include <SDL3/SDL.h>
#include <cmath>
#include <cstdint>

#include "Drawable.h"
#include "Components.h"
#include "Effects.h"

#include "Traps/Fan.h"
#include "Traps/Trampoline.h"
#include "Traps/Arrow.h"
#include "Traps/FallingPlatform.h"
#include "Traps/Fire.h"

#include "Enemies/Slime.h"
#include "Enemies/AngryPig.h"
#include "Enemies/Turtle.h"

class Player final : public Drawable {
    enum Anim : int {
        IDLE = 1,
        RUN,
        FALLING,
        JUMPING,
        HIT,
        DOUBLE_JUMP,
        APPEARING,
        SLIDE
    };

public:
    Player();
    ~Player() override;

    void update(float dt) override;
    void render(SDL_Renderer* pRenderer) override;

private:
    // --- Composants
    TransformComponent* m_transform = nullptr;
    TextureComponent*   m_texture   = nullptr;
    AnimatorComponent*  m_animator  = nullptr;
    ColliderComponent*  m_collider  = nullptr;
    DustEmitter*        m_dustEmitter = nullptr;

    // --- Cinématique / état
    SDL_FPoint m_velocity{0.0f, 0.0f};
    bool  m_onGround      = false;
    bool  m_canDoubleJump = true;
    bool  m_facingRight   = true;
    bool  m_dead          = false;

    // Tuile courante (outil/debug)
    SDL_Point m_coord{0, 0};

    // --- Paramètres façon Mario moderne
    float m_maxWalkSpeed      = 240.0f;
    float m_maxRunSpeed       = 540.0f;
    float m_runBuildRate      = 4.5f;
    float m_runDecayRate      = 3.0f;

    float m_groundAccel       = 5200.0f;
    float m_airAccel          = 2600.0f;
    float m_turnDecel         = 6400.0f;
    float m_groundFriction    = 3200.0f;

    // Saut & gravité
    float m_baseJumpVel       = 620.0f;
    float m_runJumpBonus      = 140.0f;
    float m_gravity           = 1800.0f;
    float m_fallGravityMul    = 1.25f;
    float m_lowJumpMul        = 1.7f;
    float m_apexVy            = 45.0f;
    float m_apexGravityScale  = 0.85f;
    float m_maxFallSpeed      = 1400.0f;
    float m_doubleJumpVel     = 560.0f;

    // Skid (demi-tour)
    float m_skidSpeedThresh   = 160.0f;
    float m_skidDustCooldown  = 0.08f;
    float m_skidDustTimer     = 0.0f;

    // Qualité de saut
    float m_coyoteTimeMax     = 0.100f;
    float m_jumpBufferMax     = 0.120f;
    float m_coyoteTime        = 0.0f;
    float m_jumpBuffer        = 0.0f;
    bool  m_jumpHeldPrev      = false;

    // Facteur de course (0..1)
    float m_runFactor         = 0.0f;

    // --- Wall slide / wall jump
    bool  m_onWall            = false;   // en contact éligible pour slide
    int   m_wallDir           = 0;       // -1 = mur à gauche, +1 = mur à droite
    float m_wallSlideMaxSpeed = 280.0f;  // clamp vitesse de chute en slide
    float m_wallSlideGravMul  = 0.60f;   // gravité * pendant slide
    float m_wallStickMax      = 0.150f;  // coyote sur le mur
    float m_wallStickTimer    = 0.0f;    // décrémente si on n’appuie plus vers le mur
    float m_wallJumpHVel      = 420.0f;  // impulsion horizontale du wall jump
    float m_wallJumpVVel      = 640.0f;  // impulsion verticale du wall jump

    // --- Pipeline de frame
    void stepInput(float dt);
    void applyGravity(float dt);
    void integrateAndCollide(float dt);
    void resolveVertical(float dy);
    void resolveHorizontal(float dx);

    // --- Détection murs / outils collisions
    void detectWalls(float dt);
    bool rowHasSolid(int colStart, int colEnd, int row) const;
    bool colHasSolid(int rowStart, int rowEnd, int col) const;

    // --- Sauts
    void consumeJumpIfReady();
    void startJump(float vel);
    void onLand(float platformTopY);
    void onBonk(float ceilingBottomY);
    void die();

    // --- Animations / FX
    void initComponents();
    void initResources() const;
    void initAnimations() const;
    void updateFacing();
    void updateAnimation();
    void spawnRunDust();
    void spawnSkidDust();
    void spawnLandDust();

    // --- Interactions monde
    void collectItems() const;
    void trapInteraction(float dt);
    void enemyInteraction(float dt);

    // Détails interactions
    void handleTrampolineInteraction(Trampoline* trampoline);
    void handleFanInteraction(const Fan* fan, float dt);
    void handleArrowInteraction(Arrow* arrow);
    void handleFallingPlatformInteraction(FallingPlatform* fallingPlatform);
    void handleFireInteraction(Fire* fire);

    void handleSlimeInteraction(Slime* slime);
    void handleAngryPigInteraction(AngryPig* angryPig);
    void handleTurtleInteraction(Turtle* turtle);
};