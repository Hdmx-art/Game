//
// Created by Rayann CHOUCRALLAH on 18/07/2025.
//

#ifndef CONFIG_H
#define CONFIG_H

#include <signal.h>
#include <SDL3/SDL_log.h>

#define DEBUG_STATE 1
#define ENABLE_SOUND 1
#define CAP_FPS 1

enum class TextureID {
    NONE = 0,
    NINJA_FROG_IDLE,
    NINJA_FROG_RUN,
    NINJA_FROG_FALLING,
    NINJA_FROG_JUMPING,
    NINJA_FROG_DOUBLE_JUMP,
    NINJA_FROG_HIT,
    NINJA_FROG_SLIDE,
    PLAYER_APPEARING,
    SLIME_IDLE,
    SLIME_HIT,
    ANGRYPIG_IDLE,
    ANGRYPIG_WALK,
    ANGRYPIG_RUNINNG,
    ANGRYPIG_HIT1,
    ANGRYPIG_HIT2,
    TURTLE_IDLE1,
    TURTLE_IDLE2,
    TURTLE_SPIKES_IN,
    TURTLE_SPIKES_OUT,
    TURTLE_HIT,
    TERRAIN,
    SHADOW,
    APPLE,
    CHERRY,
    BANANA,
    KIWI,
    ORANGE,
    PINEAPPLE,
    STRAWBERRY,
    MELON,
    TRAMPOLINE,
    FAN,
    FIRE_HIT,
    FIRE_OFF,
    FIRE_ON,
    ARROW,
    ARROW_HIT,
    FALLING_PLATFORM_OFF,
    FALLING_PLATFORM_ON,
    DESAPPEARING_EFFECT,
    DUST_EFFECT,
    TRANSITION_EFFECT,
    BG_PINK,
    BG_BLUE,
    BG_GREEN,
    BG_GRAY,
    BG_BROWN,
    BG_PURPLE,
    BG_YELLOW
};

enum class MapID {
    DEBUG = 0,
    LEVEL_1
};

#define PTR_CHECK(X) if(X == nullptr && DEBUG_STATE) { \
    SDL_Log("fichier %s, ligne %d", __FILE__, __LINE__); \
    SDL_LogError(1, "%s", SDL_GetError()); \
    raise(SIGTRAP); \
    }

#define BOOL_CHECK(X) if(X == false && DEBUG_STATE) { \
    SDL_Log("fichier %s, ligne %d", __FILE__, __LINE__); \
    SDL_LogError(1, "%s", SDL_GetError()); \
    raise(SIGTRAP); \
    }

#define SDL_CHECK(X) if(X != true && DEBUG_STATE) { \
    SDL_Log("fichier %s, ligne %d", __FILE__, __LINE__); \
    SDL_LogError(1, "%s", SDL_GetError()); \
    raise(SIGTRAP); \
    }

#define WARNING_IF(X, s) if(X) { SDL_Log("[WARNING] : %s, fichier %s, ligne %d", s, __FILE__, __LINE__); return; }

#include <cmath>

namespace Math {
    static float easeInOutSine(float t) {
        return -(std::cos(M_PI * t) - 1) / 2.f;
    }

    static float easeOutSine(float t) {
        return std::sin(M_PI * t / 2);
    }

    static float easeInExpo(float t) {
        return t == 0 ? 0 : std::pow(2, 10 * (t - 1));
    }
}

#endif //CONFIG_H