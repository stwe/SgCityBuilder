// This file is part of the SgCityBuilder package.
// 
// Filename: City.cpp
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#include <Application.h>
#include "City.h"
#include "map/Map.h"
#include "map/Tile.h"
#include "map/RoadNetwork.h"
#include "map/BuildingGenerator.h"
#include "map/Astar.h"
#include "renderer/MapRenderer.h"
#include "renderer/RoadNetworkRenderer.h"
#include "renderer/BuildingsRenderer.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

sg::city::city::City::City(std::string t_name, ogl::scene::Scene* t_scene, const int t_mapSize)
    : m_name{ std::move(t_name) }
    , m_scene{ t_scene }
{
    Init(m_scene, t_mapSize);
}

sg::city::city::City::~City() noexcept
{
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

sg::city::city::City::MapSharedPtr sg::city::city::City::GetMapPtr() const
{
    return m_map;
}

const sg::city::map::RoadNetwork& sg::city::city::City::GetRoadNetwork() const noexcept
{
    return *m_roadNetwork;
}

sg::city::map::RoadNetwork& sg::city::city::City::GetRoadNetwork() noexcept
{
    return *m_roadNetwork;
}

sg::city::city::City::RoadNetworkSharedPtr sg::city::city::City::GetRoadNetworkPtr() const
{
    return m_roadNetwork;
}

const sg::city::map::BuildingGenerator& sg::city::city::City::GetBuildingGenerator() const noexcept
{
    return *m_buildingGenerator;
}

sg::city::map::BuildingGenerator& sg::city::city::City::GetBuildingGenerator() noexcept
{
    return *m_buildingGenerator;
}

sg::city::city::City::BuildingGeneratorSharedPtr sg::city::city::City::GetBuildingGeneratorPtr() const
{
    return m_buildingGenerator;
}

sg::city::map::Astar& sg::city::city::City::GetAstar() const noexcept
{
    return *m_astar;
}

float& sg::city::city::City::GetTimePerDay()
{
    return m_timePerDay;
}

int sg::city::city::City::GetDay() const
{
    return m_day;
}

float sg::city::city::City::GetPopulationPool() const
{
    return m_populationPool;
}

float sg::city::city::City::GetPopulation() const
{
    return m_population;
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void sg::city::city::City::Update(const double t_dt)
{
    m_roadNetwork->Update(); // todo: do this only when necessary
    m_buildingGenerator->Update();

    m_currentTime += static_cast<float>(t_dt);

    if (m_currentTime < m_timePerDay)
    {
        return;
    }

    m_day++;
    m_currentTime = 0.0f;

    auto popTotal{ 0.0f };

    for (auto& tile : m_map->GetTiles())
    {
        if (tile->GetType() == map::Map::TileType::RESIDENTIAL)
        {
            DistributePool(m_populationPool, *tile, BIRTH_RATE - DEATH_RATE);
            popTotal += tile->population;
        }

        tile->Update();
    }

    m_populationPool += m_populationPool * (BIRTH_RATE - DEATH_RATE);
    popTotal += m_populationPool;

    m_population = popTotal;
}

void sg::city::city::City::RenderMap() const
{
    m_mapRenderer->Render();
}

void sg::city::city::City::RenderRoadNetwork() const
{
    m_roadNetworkRenderer->Render();
}

void sg::city::city::City::RenderBuildings() const
{
    m_buildingsRenderer->Render();
}

//-------------------------------------------------
// Path
//-------------------------------------------------

sg::city::city::City::PathPositionContainer sg::city::city::City::Path(const int t_fromMapX, const int t_fromMapZ, const int t_toMapX, const int t_toMapZ) const
{
    const auto fromTileIndex{ m_map->GetTileIndexByPosition(t_fromMapX, t_fromMapZ) };
    const auto toTileIndex{ m_map->GetTileIndexByPosition(t_toMapX, t_toMapZ) };

    return m_astar->FindPath(fromTileIndex, toTileIndex);
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

    // create RoadNetwork
    m_roadNetwork = std::make_shared<map::RoadNetwork>(m_map.get());

    // create BuildingGenerator
    m_buildingGenerator = std::make_shared<map::BuildingGenerator>(m_map.get());

    // create Renderer
    m_mapRenderer = std::make_unique<renderer::MapRenderer>(t_scene);
    m_roadNetworkRenderer = std::make_unique<renderer::RoadNetworkRenderer>(t_scene);
    m_buildingsRenderer = std::make_unique<renderer::BuildingsRenderer>(t_scene);

    // cretae an Astar instance
    m_astar = std::make_unique<map::Astar>(m_map.get());

    // create entities
    CreateMapEntity();
    CreateRoadNetworkEntity();
    CreateBuildingsEntity();
}

void sg::city::city::City::CreateMapEntity()
{
    const auto entity{ m_scene->GetApplicationContext()->registry.create() };

    m_scene->GetApplicationContext()->registry.assign<ecs::MapComponent>(
        entity,
        m_map
    );

    m_scene->GetApplicationContext()->registry.assign<ogl::ecs::component::TransformComponent>(
        entity,
        m_map->position,
        m_map->rotation,
        m_map->scale
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

//-------------------------------------------------
// Distribute
//-------------------------------------------------

void sg::city::city::City::DistributePool(float& t_pool, map::Tile& t_tile, const float t_rate)
{
    const auto maxTilePopulation{ map::Tile::MAX_POPULATION };

    if (t_pool > 0)
    {
        auto moving{ maxTilePopulation - static_cast<int>(t_tile.population) };

        if (moving > MOVE_POPULATION_RATE)
        {
            moving = MOVE_POPULATION_RATE;
        }

        t_pool -= moving;

        if (t_pool < 0.0f)
        {
            t_pool = 0.0f;
        }

        t_tile.population += moving;
    }

    // adjust the tile population for births and deaths
    t_tile.population += t_tile.population * t_rate;

    // move population that cannot be sustained by the tile into the pool
    if (t_tile.population > maxTilePopulation)
    {
        t_pool += t_tile.population - maxTilePopulation;
        t_tile.population = maxTilePopulation;
    }
}
