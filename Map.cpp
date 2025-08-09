#include "Map.h"

#include <fstream>
#include <unordered_map>
#include <utility>
#include <nlohmann/json.hpp>

#include "Background.h"
#include "Core.h"
#include "World.h"
#include "Item.h"
#include "Fruit.h"

// Traps
#include "Traps/Arrow.h"
#include "Traps/Fan.h"
#include "Traps/Trampoline.h"
#include "Traps/Fire.h"
#include "Traps/FallingPlatform.h"

// Enemies
#include "Enemies/Slime.h"
#include "Enemies/AngryPig.h"
#include "Enemies/Turtle.h"

namespace {

using json = nlohmann::json;

constexpr int   kTerrainCols = 22;
constexpr float kTexTileW    = 16.0f;
constexpr float kTexTileH    = 16.0f;

inline std::optional<TextureID> FruitNameToTexture(std::string_view name) {
    static const std::unordered_map<std::string, TextureID> kMap{
        {"Banana",     TextureID::BANANA},
        {"Kiwi",       TextureID::KIWI},
        {"Cherry",     TextureID::CHERRY},
        {"Apple",      TextureID::APPLE},
        {"Orange",     TextureID::ORANGE},
        {"Pineapple",  TextureID::PINEAPPLE},
        {"Strawberry", TextureID::STRAWBERRY},
        {"Melon",      TextureID::MELON}
    };
    if (auto it = kMap.find(std::string{name}); it != kMap.end()) return it->second;
    return std::nullopt;
}

inline std::optional<TextureID> BackgroundNameToTexture(std::string_view name) {
    static const std::unordered_map<std::string, TextureID> kMap{
        {"BG_BROWN",  TextureID::BG_BROWN},
        {"BG_PINK",   TextureID::BG_PINK},
        {"BG_YELLOW", TextureID::BG_YELLOW},
        {"BG_BLUE",   TextureID::BG_BLUE},
        {"BG_GREEN",  TextureID::BG_GREEN},
        {"BG_GRAY",   TextureID::BG_GRAY},
        {"BG_PURPLE", TextureID::BG_PURPLE}
    };
    if (auto it = kMap.find(std::string{name}); it != kMap.end()) return it->second;
    return std::nullopt;
}

} // namespace

Map::Map() = default;

Map::~Map() {
    if (m_background) {
        delete m_background;
        m_background = nullptr;
    }
}

void Map::update(float dt) const {
    if (m_background) {
        m_background->update(dt);
    }
}

void Map::render(SDL_Renderer* pRenderer) const {
    if (m_background) {
        m_background->render(pRenderer);
    }

    for (const auto& shadow : m_shadows) {
        SDL_CHECK(SDL_RenderTexture(
            pRenderer,
            Core::getInstance().getTexture(shadow.textureID),
            &shadow.textureRect,
            &shadow.rect));
    }

    for (const auto& layer : m_layers) {
        for (const auto& row : layer) {
            for (const auto& tile : row) {
                if (tile.textureID == TextureID::NONE) continue;
                SDL_CHECK(SDL_RenderTexture(
                    pRenderer,
                    Core::getInstance().getTexture(tile.textureID),
                    &tile.textureRect,
                    &tile.rect));
            }
        }
    }
}

int32_t Map::getTileAt(int32_t column, int32_t row) const {
    const LayerContent* ground = groundLayer();
    if (!ground || row < 0 || column < 0) return 0;

    const size_t r = static_cast<size_t>(row);
    const size_t c = static_cast<size_t>(column);
    if (ground->empty() || r >= ground->size() || c >= (*ground)[r].size()) return 0;

    return (*ground)[r][c];
}

void Map::load(MapID id) {
    if (!m_background) {
        m_background = new Background();
    }

    m_layers.clear();
    m_shadows.clear();
    m_fileContent.clear();

    std::string path;
    switch (id) {
        case MapID::DEBUG:   path = "assets/Maps/debug_map.tmj"; break;
        case MapID::LEVEL_1: path = "assets/Maps/level_1.tmj";   break;
        default:             path.clear();                        break;
    }

    if (!path.empty()) {
        m_fileContent = parseTiledFile(path);
    }

    buildRenderableLayers();
    buildShadows();

    m_currentMapID = id;
}

const LayerContent* Map::groundLayer() const noexcept {
    return m_fileContent.empty() ? nullptr : &m_fileContent.front();
}

void Map::buildRenderableLayers() {
    m_layers.clear();
    m_layers.reserve(m_fileContent.size());

    for (const auto& layerContent : m_fileContent) {
        const size_t rows = layerContent.size();
        const size_t cols = rows > 0 ? layerContent[0].size() : 0;

        Layer layer;
        layer.resize(rows);
        for (size_t i = 0; i < rows; ++i) {
            layer[i].resize(cols);
            for (size_t j = 0; j < cols; ++j) {
                const int32_t gid = layerContent[i][j];
                if (gid <= 0) continue;

                Tile t = makeTileFromGID(gid);
                t.rect.x = static_cast<float>(j) * TILE_SIZEF;
                t.rect.y = static_cast<float>(i) * TILE_SIZEF;
                layer[i][j] = t;
            }
        }
        m_layers.emplace_back(std::move(layer));
    }
}

void Map::buildShadows() {
    const LayerContent* ground = groundLayer();
    if (!ground || ground->empty()) return;

    const int32_t height = static_cast<int32_t>(ground->size());
    const int32_t width  = static_cast<int32_t>((*ground)[0].size());

    m_shadows.clear();
    m_shadows.reserve(static_cast<size_t>(height) * static_cast<size_t>(width));

    for (int32_t i = 0; i < height; ++i) {
        for (int32_t j = 0; j < width; ++j) {
            if ((*ground)[i][j] == 0) continue;

            Tile shadow;
            shadow.rect.x = TILE_SIZEF * static_cast<float>(j) - (TILE_SIZEF / 8.0f);
            shadow.rect.y = TILE_SIZEF * static_cast<float>(i) + (TILE_SIZEF / 8.0f);
            shadow.rect.w = TILE_SIZEF;
            shadow.rect.h = TILE_SIZEF;

            shadow.textureRect = {0.0f, 0.0f, kTexTileW, kTexTileH};
            shadow.textureID   = TextureID::SHADOW;

            m_shadows.emplace_back(shadow);
        }
    }
}

void Map::initTransform(Entity* entity, const nlohmann::json& object) const {
    auto* transform = entity->getComponent<TransformComponent>();

    const float x = object.at("x").get<float>();
    const float h = object.at("height").get<float>();
    const float y = object.at("y").get<float>() - h;

    transform->rect = {x, y, object.at("width").get<float>(), h};
    transform->rotation = object.at("rotation").get<float>();
}

std::vector<LayerContent> Map::parseTiledFile(const std::string& path) {
    std::ifstream file(path);
    BOOL_CHECK(file.is_open());

    nlohmann::json root;
    file >> root;

    World& world = Core::getInstance().getWorld();

    std::vector<LayerContent> layers;
    layers.reserve(3);

    for (const auto& layer : root.at("layers")) {
        const std::string layerName = layer.value("name", "");

        if (layerName == "Ground") {
            const int w = layer.at("width").get<int>();
            const int h = layer.at("height").get<int>();
            const auto& data = layer.at("data");

            if (static_cast<int>(data.size()) < w * h) continue;

            LayerContent mat(static_cast<size_t>(h), std::vector<int32_t>(static_cast<size_t>(w)));
            for (int idx = 0; idx < w * h; ++idx) {
                const int r = idx / w;
                const int c = idx % w;
                mat[static_cast<size_t>(r)][static_cast<size_t>(c)] =
                    data.at(static_cast<size_t>(idx)).get<int32_t>();
            }
            layers.emplace_back(std::move(mat));
        }
        else if (layerName == "Items") {
            if (layer.contains("objects")) {
                for (const auto& object : layer.at("objects")) {
                    const std::string name = object.value("name", "");
                    if (auto tex = FruitNameToTexture(name)) {
                        auto* fruit = new Fruit();
                        fruit->setType(*tex);

                        const SDL_FPoint pos{
                            object.at("x").get<float>(),
                            object.at("y").get<float>()
                        };
                        world.createItem(fruit, pos.x, pos.y);
                    }
                }
            }
        }
        else if (layerName == "Traps") {
            if (layer.contains("objects")) {
                for (const auto& object : layer.at("objects")) {
                    loadTrap(object);
                }
            }
        }
        else if (layerName == "Characters") {
            if (layer.contains("objects")) {
                for (const auto& object : layer.at("objects")) {
                    loadCharacter(object);
                }
            }
        }
        else if (layerName == "Background") {
            if (layer.contains("objects") && !layer.at("objects").empty()) {
                const std::string bgName = layer.at("objects")[0].value("name", "");
                loadBackgroundByName(bgName);
            }
        }
    }

    return layers;
}

void Map::loadTrap(const nlohmann::json& object) const {
    World& world = Core::getInstance().getWorld();
    const std::string name = object.value("name", "");

    if (name == "Trampoline") {
        auto* trap = world.creatTrap<Trampoline>();
        initTransform(trap, object);
    }
    else if (name == "Fan") {
        auto* trap = world.creatTrap<Fan>();
        const float rot = object.value("rotation", 0.0f);
        if (rot == 90.0f) {
            trap->setType(Fan::FanType::HORIZONTAL);
        }
        initTransform(trap, object);
    }
    else if (name == "Arrow") {
        auto* arrow = world.creatTrap<Arrow>();
        arrow->setDirection(object.value("rotation", 0.0f));
        initTransform(arrow, object);
    }
    else if (name == "FPlatform") {
        auto* fplatform = world.creatTrap<FallingPlatform>();
        initTransform(fplatform, object);
    }
    else if (name == "Fire") {
        auto* fire = world.creatTrap<Fire>();
        initTransform(fire, object);
    }
}

void Map::loadCharacter(const nlohmann::json& object) {
    World& world = Core::getInstance().getWorld();
    const std::string name = object.value("name", "");

    if (name == "Player") {
        const SDL_FPoint pos{
            object.at("x").get<float>(),
            object.at("y").get<float>()
        };
        world.createPlayer(pos.x, pos.y);
    }
    else if (name == "Slime") {
        auto* slime = world.createEnemy<Slime>();
        initTransform(slime, object);
    }
    else if (name == "AngryPig") {
        auto* pig = world.createEnemy<AngryPig>();
        initTransform(pig, object);
    }
    else if (name == "Turtle") {
        auto* turtle = world.createEnemy<Turtle>();
        initTransform(turtle, object);
    }
}

void Map::loadBackgroundByName(std::string_view name) {
    if (!m_background) return;
    if (auto tex = BackgroundNameToTexture(name)) {
        m_background->reset(*tex);
    }
}

Tile Map::makeTileFromGID(int32_t gid) noexcept {
    Tile tile;
    tile.rect.w = TILE_SIZEF;
    tile.rect.h = TILE_SIZEF;

    tile.textureRect.w = kTexTileW;
    tile.textureRect.h = kTexTileH;
    tile.textureID     = TextureID::TERRAIN;

    const int idx = gid - 1;
    const int col = idx % kTerrainCols;
    const int row = idx / kTerrainCols;

    tile.textureRect.x = static_cast<float>(col) * kTexTileW;
    tile.textureRect.y = static_cast<float>(row) * kTexTileH;

    return tile;
}