cmake_minimum_required(VERSION 3.20)
project(Game LANGUAGES CXX)

# → C++17 obligatoire
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# → Répertoires de sortie binaires
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# → Ne pas installer les SDL vendored
set(SDL3_SKIP_INSTALL        ON CACHE BOOL "" FORCE)
set(SDL3IMAGE_SKIP_INSTALL   ON CACHE BOOL "" FORCE)
set(SDL3_TTF_SKIP_INSTALL    ON CACHE BOOL "" FORCE)
set(SDL3MIXER_SKIP_INSTALL   ON CACHE BOOL "" FORCE)

# ───────────────────────────────────────────────────────────────────────────────
# 1) Récupérer la bibliothèque JSON (nlohmann/json) via FetchContent
# ───────────────────────────────────────────────────────────────────────────────
include(FetchContent)

FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG        v3.11.2               # ou la version souhaitée
)
FetchContent_MakeAvailable(nlohmann_json)

# ───────────────────────────────────────────────────────────────────────────────
# 2) Vos sous‑projets SDL vendored (avec SDL_mixer !)
# ───────────────────────────────────────────────────────────────────────────────
add_subdirectory(vendored/SDL         EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_image   EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_ttf     EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_mixer   EXCLUDE_FROM_ALL)    # <-- Ajout ici

# ───────────────────────────────────────────────────────────────────────────────
# 3) Exécutable principal
# ───────────────────────────────────────────────────────────────────────────────
add_executable(Game
        main.cpp
        Core.h     Core.cpp
        Map.h      Map.cpp
        Entity.h
        Components.h Components.cpp
        Drawable.h
        Player.h   Player.cpp
        Config.h
        Item.h
        Fruit.cpp
        Fruit.h
        Effects.h
        Effects.cpp
        SoundManager.cpp
        SoundManager.h
        Background.cpp
        Background.h
        Traps/Trap.h
        Traps/Trampoline.cpp
        Traps/Trampoline.h
        Traps/Fan.cpp
        Traps/Fan.h
        Entity.cpp
        Drawable.cpp
        World.cpp
        World.h
        Traps/Arrow.cpp
        Traps/Arrow.h
        Enemies/Enemy.h
        Enemies/Slime.cpp
        Enemies/Slime.h
        Enemies/Enemy.cpp
        Enemies/AngryPig.cpp
        Enemies/AngryPig.h
        Enemies/Turtle.cpp
        Enemies/Turtle.h
        Traps/FallingPlatform.cpp
        Traps/FallingPlatform.h
        Traps/Fire.cpp
        Traps/Fire.h
)

# ───────────────────────────────────────────────────────────────────────────────
# 4) Linker : SDL + JSON (+ SDL_mixer !)
# ───────────────────────────────────────────────────────────────────────────────
target_link_libraries(Game
        PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
        SDL3_mixer::SDL3_mixer         # <-- Ajout ici
        nlohmann_json::nlohmann_json
)