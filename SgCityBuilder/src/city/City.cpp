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
#include <math/Transform.h>
#include "City.h"
#include "map/Map.h"
#include "map/RoadNetwork.h"
#include "map/BuildingGenerator.h"
#include "map/tile/RoadTile.h"
#include "map/tile/BuildingTile.h"
#include "renderer/MapRenderer.h"
#include "renderer/RoadNetworkRenderer.h"
#include "renderer/BuildingsRenderer.h"
#include "automata/Automata.h"
#include "automata/AutoTrack.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

sg::city::city::City::City(std::string t_name, std::string t_mapFileName, ogl::scene::Scene* t_scene)
    : m_name{ std::move(t_name) }
    , m_mapFileName{ std::move(t_mapFileName) }
    , m_scene{ t_scene }
{
    SG_OGL_ASSERT(t_scene, "[City::City()] Null pointer.")

    SG_OGL_LOG_DEBUG("[City::City()] Construct City.");

    Init();
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

void sg::city::city::City::Update(const double t_dt, TileIndexContainer& t_tileIndexContainer)
{
    // handle a changed Tile

    for (auto tileIndex : t_tileIndexContainer)
    {
        // get Tile
        auto& changedTile{ m_map->GetTiles()[tileIndex] };

        // check if the new Tile is of type TRAFFIC
        if (changedTile->type == map::tile::TileType::TRAFFIC)
        {
            ///////////////////// !! very expensive !! /////////////////////

            // the indices of the roads are in a separate vector to avoid unnecessary loops

            auto& tiles{ m_map->GetTiles() };

            for (auto roadIndex : m_map->roadIndices)
            {
                dynamic_cast<map::tile::RoadTile*>(tiles[roadIndex].get())->ClearTracksAndStops();
            }

            for (auto roadIndex : m_map->roadIndices)
            {
                dynamic_cast<map::tile::RoadTile*>(tiles[roadIndex].get())->Update();
            }

            m_roadNetwork->CreateRoadNetworkMesh();

            //////////////////////////////////////////////////////////
        }
        else if (changedTile->type == map::tile::TileType::RESIDENTIAL)
        {
            auto* buildingTile{ dynamic_cast<map::tile::BuildingTile*>(changedTile.get()) };
            SG_OGL_ASSERT(buildingTile, "[City::Update()] Null pointer.")

            m_buildingGenerator->AddBuilding(*buildingTile);

            changedTile->Update();
        }
        else
        {
            changedTile->Update();
        }
    }

    if (!t_tileIndexContainer.empty())
    {
        t_tileIndexContainer.clear();
    }


    // change StopPattern

    /*
    for (auto& tile : m_map->GetTiles())
    {
        if (tile->type == map::tile::TileType::TRAFFIC)
        {
            auto* roadTile{ dynamic_cast<map::tile::RoadTile*>(tile.get()) };
            SG_OGL_ASSERT(roadTile, "[City::Update()] Null pointer.")

            if (!roadTile->GetStopPatterns().empty() && !automatas.empty())
            {
                m_stopPatternTimer += static_cast<float>(t_dt) * STOP_PATTERN_SPEED;
                if (m_stopPatternTimer >= 5.0f)
                {
                    m_stopPatternTimer = 0.0f;

                    const auto lastIndex{ static_cast<int>(roadTile->GetStopPatterns().size()) - 1 };
                    const auto currentIndex{ roadTile->GetCurrentStopPatternIndex() };

                    if (currentIndex == lastIndex)
                    {
                        roadTile->ApplyStopPattern(0);
                    }
                    else
                    {
                        roadTile->ApplyStopPattern(currentIndex + 1);
                    }

                    SG_OGL_LOG_WARN("[City::Update()] StopPattern changed.");
                }
            }
        }
    }
    */


    // connect regions

    // todo must be fixed
    //m_map->FindConnectedRegions();


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
    m_roadNetworkRenderer->Render();
    m_buildingsRenderer->Render();
}

//-------------------------------------------------
// Edit
//-------------------------------------------------

auto sg::city::city::City::ReplaceTile(const int t_mapX, const int t_mapZ, map::tile::TileType t_tileType) const -> std::tuple<int, bool>
{
    auto& tiles{ m_map->GetTiles() };
    const auto currentTileIndex{ m_map->GetTileMapIndexByMapPosition(t_mapX, t_mapZ) };

    // there must be nothing on the tile yet - skip
    if (tiles[currentTileIndex]->type != map::tile::TileType::NONE)
    {
        SG_OGL_LOG_INFO("[City::ReplaceTile()] There is already something on this tile. Skip replace.");
        return { currentTileIndex, true };
    }

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
        auto newTile{ std::make_unique<map::tile::RoadTile>(
                static_cast<float>(t_mapX),
                static_cast<float>(t_mapZ),
                m_map.get()
            )
        };

        tiles[currentTileIndex] = std::move(newTile);
        tiles[currentTileIndex]->GetNeighbours() = neighbours;

        m_map->roadIndices.push_back(currentTileIndex);
    }
    else if(t_tileType == map::tile::TileType::RESIDENTIAL)
    {
        auto newTile{ std::make_unique<map::tile::BuildingTile>(
                static_cast<float>(t_mapX),
                static_cast<float>(t_mapZ),
                m_map.get()
            )
        };

        tiles[currentTileIndex] = std::move(newTile);
        tiles[currentTileIndex]->GetNeighbours() = neighbours;
    }
    else
    {
        // default: create a new Tile with the new type
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
    if (!tile->HasSafeTrack())
    {
        return false;
    }

    // get the safe AutoTrack
    const auto it{ std::find_if(tile->GetAutoTracks().begin(), tile->GetAutoTracks().end(),
        [](const std::shared_ptr<automata::AutoTrack>& t_autoTrack)
             {
                return t_autoTrack->isSafe;
             }
        )
    };

    SG_OGL_ASSERT(*it, "[City::TrySpawnCarAtSafeTrack()] Invalid iterator.");
    SG_OGL_LOG_INFO("[City::TrySpawnCarAtSafeTrack()] Spawn a new car at Map x: {}, z: {}", tile->GetMapX(), tile->GetMapZ());

    // create an Automata
    auto automata{ std::make_unique<automata::Automata>() };
    automata->autoLength = 0.2f;
    automata->currentTrack = *it;
    automata->currentTrack->automatas.push_back(automata.get());
    automata->rootNode = (*it)->startNode;
    automata->Update(0.0f);

    automatas.push_back(std::move(automata));

    CreateCarEntity();

    return true;
}

//-------------------------------------------------
// Debug
//-------------------------------------------------

void sg::city::city::City::RenderAutoTracks() const
{
    for (auto& tile : m_map->GetTiles())
    {
        if (tile->type == map::tile::TileType::TRAFFIC)
        {
            auto* roadTile{ dynamic_cast<map::tile::RoadTile*>(tile.get()) };
            SG_OGL_ASSERT(roadTile, "[City::RenderAutoTracks()] Null pointer.");

            roadTile->RenderAutoTracks();
        }
    }
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void sg::city::city::City::Init()
{
    // create Map
    m_map = std::make_shared<map::Map>(m_scene, m_mapFileName);
    m_map->CreateMap();
    m_map->position = glm::vec3(0.0f);
    m_map->rotation = glm::vec3(0.0f);
    m_map->scale = glm::vec3(1.0f);

    // create RoadNetwork and BuildingGenerator
    m_roadNetwork = std::make_shared<map::RoadNetwork>(this);
    m_buildingGenerator = std::make_shared<map::BuildingGenerator>(this);

    // create a building for each residential Tile
    StoreBuildings();

    // create a road for each traffic Tile
    StoreRoads();

    // create plants from the plants positions
    CreatePlants();

    // create Renderer
    m_mapRenderer = std::make_unique<renderer::MapRenderer>(m_scene);
    m_roadNetworkRenderer = std::make_unique<renderer::RoadNetworkRenderer>(m_scene);
    m_buildingsRenderer = std::make_unique<renderer::BuildingsRenderer>(m_scene);

    // create entities
    CreateMapEntity();
    CreateRoadNetworkEntity();
    CreateBuildingsEntity();
}

void sg::city::city::City::StoreBuildings() const
{
    for (auto& tile : m_map->GetTiles())
    {
        if (tile->type == map::tile::TileType::RESIDENTIAL)
        {
            m_buildingGenerator->AddBuilding(*dynamic_cast<map::tile::BuildingTile*>(tile.get()));
        }
    }
}

void sg::city::city::City::StoreRoads()
{
    // das letzte Tile uebergeben -> alle Roads werden einmal neu erstellt

    // todo: refactor Update Funktion
    if (!m_map->roadIndices.empty())
    {
        TileIndexContainer tiles;
        tiles.push_back(m_map->roadIndices.back());
        Update(0.016f, tiles);
    }
}

void sg::city::city::City::CreatePlants() const
{
    if (!m_map->plantPositions.empty())
    {
        std::vector<glm::mat4> matrices;

        for (auto& plant : m_map->plantPositions)
        {
            ogl::math::Transform transform;
            transform.position = glm::vec3(plant.x, 2.0f, -plant.z);
            transform.rotation = glm::vec3(180.0, 0.0f, 0.0f);
            transform.scale = glm::vec3(2.0f);

            matrices.push_back(static_cast<glm::mat4>(transform));
        }

        m_scene->GetApplicationContext()->GetEntityFactory().CreateModelEntity(
            static_cast<uint32_t>(matrices.size()),
            "res/model/Tree_01/billboardmodel.obj",
            matrices,
            true
        );
    }
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
        "res/model/CarKit/suv.obj",
        glm::vec3(automata->position.x, 0.015f, automata->position.z),
        glm::vec3(0.0f, 90.0f, 0.0f),
        glm::vec3(0.17f),
        false
    ) };

    automata->isEntity = true;

    // add Automata as component
    m_scene->GetApplicationContext()->registry.assign<ecs::AutomataComponent>(
        entity,
        automata
    );
}

void sg::city::city::City::CreateRoadNetworkEntity()
{
    const auto entity{ m_scene->GetApplicationContext()->registry.create() };

    m_scene->GetApplicationContext()->registry.assign<ecs::RoadNetworkComponent>(
        entity,
        m_roadNetwork
    );

    m_scene->GetApplicationContext()->registry.assign<ogl::ecs::component::TransformComponent>(
        entity,
        m_map->position,
        m_map->rotation,
        m_map->scale
    );
}

void sg::city::city::City::CreateBuildingsEntity()
{
    const auto entity{ m_scene->GetApplicationContext()->registry.create() };

    m_scene->GetApplicationContext()->registry.assign<ecs::BuildingsComponent>(
        entity,
        m_buildingGenerator
    );

    m_scene->GetApplicationContext()->registry.assign<ogl::ecs::component::TransformComponent>(
        entity,
        m_map->position,
        m_map->rotation,
        m_map->scale
    );
}
