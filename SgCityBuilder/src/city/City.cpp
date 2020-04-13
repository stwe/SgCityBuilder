// This file is part of the SgCityBuilder package.
// 
// Filename: City.cpp
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#include <Core.h>
#include <Log.h>
#include <Application.h>
#include <glm/vec3.hpp>
#include <ecs/component/Components.h>
#include <ecs/factory/EntityFactory.h>
#include <scene/Scene.h>
#include "City.h"
#include "map/Map.h"
#include "map/tile/RoadTile.h"
#include "renderer/MapRenderer.h"
#include "automata/Automata.h"
#include "automata/AutoTrack.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

sg::city::city::City::City(std::string t_name, ogl::scene::Scene* t_scene, const int t_mapSize)
    : m_name{ std::move(t_name) }
    , m_scene{ t_scene }
{
    SG_OGL_CORE_ASSERT(t_scene, "[City::City()] Null pointer.")

    SG_OGL_LOG_DEBUG("[City::City()] Construct City.");

    Init(m_scene, t_mapSize);
}

sg::city::city::City::~City() noexcept
{
    SG_OGL_LOG_DEBUG("[City::~City()] Destruct City.");
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

const std::string& sg::city::city::City::GetName() const noexcept
{
    return m_name;
}

const sg::city::map::Map& sg::city::city::City::GetMap() const noexcept
{
    return *m_map;
}

sg::city::map::Map& sg::city::city::City::GetMap() noexcept
{
    return *m_map;
}

sg::city::city::City::MapSharedPtr sg::city::city::City::GetMapSharedPtr() const
{
    return m_map;
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void sg::city::city::City::Update(const double t_dt, TileIndexContainer& t_changedTiles)
{
    // handle the changed Tiles

    for (auto changedTileIndex : t_changedTiles)
    {
        // get Tile
        auto& changedTile{ m_map->GetTiles()[changedTileIndex] };

        // check if the new Tile is of type TRAFFIC
        if (changedTile->type == map::tile::TileType::TRAFFIC)
        {
            // if this is the case, clear AutoTracks and StopPattern from all TRAFFIC Tiles
            for (auto& tile : m_map->GetTiles())
            {
                if (tile->type == map::tile::TileType::TRAFFIC)
                {
                    auto* roadTile{ dynamic_cast<map::tile::RoadTile*>(tile.get()) };
                    SG_OGL_CORE_ASSERT(roadTile, "[City::Update()] Null pointer.")

                    roadTile->ClearTracksAndStops();
                }
            }

            // and finally: update all TRAFFIC Tiles
            for (auto& tile : m_map->GetTiles())
            {
                if (tile->type == map::tile::TileType::TRAFFIC)
                {
                    tile->Update();
                }
            }
        }
        else
        {
            changedTile->Update();
        }
    }

    t_changedTiles.clear();


    // create some Automatas

    if (spawnCars)
    {
        if (automatas.size() < MAX_AUTOMATAS)
        {
            auto newCarCreated{ false };
            auto attempts{ ATTEMPS };

            while (!newCarCreated && attempts > 0)
            {
                attempts--;
                newCarCreated = TrySpawnCarAtSafeTrack(rand() % m_map->GetMapSize(), rand() % m_map->GetMapSize());
            }
        }
    }
}

void sg::city::city::City::Render() const
{
    m_mapRenderer->Render();
}

//-------------------------------------------------
// Edit
//-------------------------------------------------

auto sg::city::city::City::ReplaceTile(const int t_mapX, const int t_mapZ, map::tile::TileType t_tileType) const -> std::tuple<int, bool>
{
    auto& tiles{ m_map->GetTiles() };
    const auto currentTileIndex{ m_map->GetTileMapIndexByMapPosition(t_mapX, t_mapZ) };

    // if the type does not change - skip
    if (tiles[currentTileIndex]->type == t_tileType)
    {
        SG_OGL_LOG_INFO("[City::ReplaceTile()] This type already exists at this position. Skip replace.");
        return { currentTileIndex, true };
    }

    // store neighbours
    const auto neighbours{ tiles[currentTileIndex]->GetNeighbours() };

    // delete the unique pointer
    tiles[currentTileIndex].reset();

    SG_OGL_ASSERT(tiles[currentTileIndex] == nullptr, "[City::ReplaceTile()] The pointer should be nullptr.");

    if (t_tileType == map::tile::TileType::TRAFFIC)
    {
        // create a RoadTile
        auto newTile{ std::make_unique<map::tile::RoadTile>(
                static_cast<float>(t_mapX),
                static_cast<float>(t_mapZ),
                t_tileType,
                m_map.get()
            )
        };

        tiles[currentTileIndex] = std::move(newTile);
        tiles[currentTileIndex]->GetNeighbours() = neighbours;

        dynamic_cast<map::tile::RoadTile*>(tiles[currentTileIndex].get())->Init();
    }
    else
    {
        // create a new Tile with the new type
        auto newTile{ std::make_unique<map::tile::Tile>(
                static_cast<float>(t_mapX),
                static_cast<float>(t_mapZ),
                t_tileType,
                m_map.get()
            )
        };

        tiles[currentTileIndex] = std::move(newTile);
        tiles[currentTileIndex]->GetNeighbours() = neighbours;
    }

    return { currentTileIndex, false };
}

//-------------------------------------------------
// Spawn
//-------------------------------------------------

bool sg::city::city::City::TrySpawnCarAtSafeTrack(const int t_mapX, const int t_mapZ)
{
    // get RoadTile at position
    auto* tile{ dynamic_cast<map::tile::RoadTile*>(&m_map->GetTileByMapPosition(t_mapX, t_mapZ)) };
    if (!tile)
    {
        return false;
    }

    // check if there is a Safe Auto Track
    if (!tile->safeAutoTrack)
    {
        return false;
    }

    SG_OGL_LOG_INFO("[City::TrySpawnCarAtSafeTrack()] Spawn a new car at Map x: {}, z: {}", tile->GetMapX(), tile->GetMapZ());

    // create an Automata
    auto automata{ std::make_unique<automata::Automata>() };
    automata->autoLength = 0.2f;
    automata->currentTrack = tile->safeAutoTrack;
    automata->currentTrack->automatas.push_back(automata.get());
    automata->rootNode = tile->safeAutoTrack->startNode;
    automata->Update(0.0f);

    automatas.push_back(std::move(automata));

    CreateCarEntity();

    return true;
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void sg::city::city::City::Init(ogl::scene::Scene* t_scene, const int t_mapSize)
{
    // create Map
    m_map = std::make_shared<map::Map>(t_scene);
    m_map->CreateMap(t_mapSize);
    m_map->position = glm::vec3(0.0f);
    m_map->rotation = glm::vec3(0.0f);
    m_map->scale = glm::vec3(1.0f);

    // create Renderer
    m_mapRenderer = std::make_unique<renderer::MapRenderer>(t_scene);

    // create Map Entity
    CreateMapEntity();
}

//-------------------------------------------------
// Entity
//-------------------------------------------------

void sg::city::city::City::CreateMapEntity()
{
    const auto entity{ m_scene->GetApplicationContext()->registry.create() };

    // add MapComponent
    m_scene->GetApplicationContext()->registry.assign<ecs::MapComponent>(
        entity,
        m_map
    );

    // add TransformComponent
    m_scene->GetApplicationContext()->registry.assign<ogl::ecs::component::TransformComponent>(
        entity,
        m_map->position,
        m_map->rotation,
        m_map->scale
    );
}

void sg::city::city::City::CreateCarEntity()
{
    // create Entity from the last Automat in the list
    auto& automata{ automatas.back() };

    SG_OGL_ASSERT(!automata->isEntity, "[City::CreateCarEntity()] The Automat is already an Entity.");

    // add a Model
    auto entity{ m_scene->GetApplicationContext()->GetEntityFactory().CreateModelEntity(
        "res/model/Plane1/plane1.obj",
        glm::vec3(automata->position.x, 0.015f, automata->position.z),
        glm::vec3(0.0f),
        glm::vec3(0.125f / 4.0f),
        false
    ) };

    automata->isEntity = true;

    // add Automata as component
    m_scene->GetApplicationContext()->registry.assign<ecs::AutomataComponent>(
        entity,
        automata
    );
}
