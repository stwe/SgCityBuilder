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

void sg::city::city::City::Update(const double t_dt, TileIndexContainer& t_changedTiles) const
{
    for (auto changedTileIndex : t_changedTiles)
    {
        auto& tile{ m_map->GetTiles()[changedTileIndex] };

        // Tile update
        tile->Update();

        // Tile neighbours update
        for (auto& neighbour : tile->GetNeighbours())
        {
            if (m_map->GetTiles()[neighbour.second]->type == map::tile::TileType::TRAFFIC)
            {
                m_map->GetTiles()[neighbour.second]->Update();
            }

        }
    }

    t_changedTiles.clear();
}

void sg::city::city::City::Render() const
{
    m_mapRenderer->Render();
}

//-------------------------------------------------
// Edit
//-------------------------------------------------

int sg::city::city::City::ReplaceTile(const int t_mapX, const int t_mapZ, map::tile::TileType t_tileType) const
{
    auto& tiles{ m_map->GetTiles() };
    const auto index{ m_map->GetTileMapIndexByMapPosition(t_mapX, t_mapZ) };

    // store neighbours
    const auto neighbours{ tiles[index]->GetNeighbours() };

    // delete the unique pointer
    tiles[index].reset();

    SG_OGL_ASSERT(tiles[index] == nullptr, "[City::ReplaceTile()] The pointer should be nullptr.");

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

        tiles[index] = std::move(newTile);
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

        tiles[index] = std::move(newTile);
    }

    // copy neighbours
    tiles[index]->GetNeighbours() = neighbours;

    return index;
}

//-------------------------------------------------
// Spawn
//-------------------------------------------------

bool sg::city::city::City::SpawnCarAtSafeTrack(const int t_mapX, const int t_mapZ)
{
    // get RoadTile at position
    auto* tile{ dynamic_cast<map::tile::RoadTile*>(&m_map->GetTileByMapPosition(t_mapX, t_mapZ)) };
    if (!tile)
    {
        SG_OGL_LOG_INFO("[City::SpawnCarAtSafeTrack()] No Road Tile available at position x: {}, z: {}.", t_mapX, t_mapZ);

        return false;
    }

    // check if there is a Safe Auto Track
    if (!tile->safeAutoTrack)
    {
        SG_OGL_LOG_INFO("[City::SpawnCarAtSafeTrack()] No Safe Auto Track found.");

        return false;
    }

    SG_OGL_LOG_INFO("[City::SpawnCarAtSafeTrack()] Spawn a new Car at map x: {}, map z: {}", tile->GetMapX(), tile->GetMapZ());

    // create an Automata
    auto automata{ std::make_shared<automata::Automata>() };
    automata->autoLength = 0.2f;
    automata->currentTrack = tile->safeAutoTrack;
    automata->currentTrack->automatas.push_back(automata);
    automata->rootNode = tile->safeAutoTrack->startNode;
    automata->Update(0.0f);

    automatas.push_back(automata);

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
