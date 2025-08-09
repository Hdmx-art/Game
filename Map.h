#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include <string>
#include <optional>
#include <string_view>
#include <nlohmann/json.hpp>
#include "Config.h"

// Déclarations externes existantes dans le projet
class Item;
class DesappearingEffect;
class Background;
class Trap;
class Player;
class Entity;

// Taille d’une tuile (monde/écran)
constexpr float TILE_SIZEF = 32.0f;

// Tuile affichable
struct Tile {
    SDL_FRect rect{0.0f, 0.0f, TILE_SIZEF, TILE_SIZEF};
    SDL_FRect textureRect{0.0f, 0.0f, 0.0f, 0.0f};
    TextureID textureID = TextureID::SHADOW;

    Tile() = default;
    Tile(const SDL_FRect& r, const SDL_FRect& tr, TextureID id) : rect(r), textureRect(tr), textureID(id) {}
    Tile& operator=(const Tile&) = default;
};

using Layer = std::vector<std::vector<Tile>>;
using LayerContent = std::vector<std::vector<int32_t>>;

class Map {
    enum class State {
        GAME,
        TRANSITION
    };

public:
    Map();
    ~Map();

    void load(MapID id);
    void update(float dt) const;
    void render(SDL_Renderer* pRenderer) const;

    [[nodiscard]] int32_t getTileAt(int32_t column, int32_t row) const;

private:
    Background*                m_background = nullptr;
    std::vector<LayerContent>  m_fileContent;
    std::vector<Layer>         m_layers;
    std::vector<Tile>          m_shadows;
    State                      m_currentState = State::GAME;
    MapID                      m_currentMapID{};

    [[nodiscard]] const LayerContent* groundLayer() const noexcept;

    void buildRenderableLayers();
    void buildShadows();

    std::vector<LayerContent> parseTiledFile(const std::string& path);

    void loadTrap(const nlohmann::json& object) const;
    void loadCharacter(const nlohmann::json& object);
    void loadBackgroundByName(std::string_view name);

    static Tile makeTileFromGID(int32_t gid) noexcept;
    void initTransform(Entity* entity, const nlohmann::json& object) const;
};