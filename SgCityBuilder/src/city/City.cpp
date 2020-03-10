#include <Application.h>
#include "City.h"
#include "map/Map.h"
#include "map/RoadNetwork.h"
#include "map/Astar.h"
#include "renderer/MapRenderer.h"
#include "renderer/RoadNetworkRenderer.h"

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

sg::city::map::Astar& sg::city::city::City::GetAstar() const noexcept
{
    return *m_astar;
}

int sg::city::city::City::GetDay() const
{
    return m_day;
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void sg::city::city::City::Update(const double t_dt)
{
    m_roadNetwork->UpdateDirections(); // todo: do this only when necessary

    m_currentTime += static_cast<float>(t_dt);

    if (m_currentTime < m_timePerDay)
    {
        return;
    }

    m_day++;
    m_currentTime = 0.0f;
}

void sg::city::city::City::RenderMap() const
{
    m_mapRenderer->Render();
}

void sg::city::city::City::RenderRoadNetwork() const
{
    m_roadNetworkRenderer->Render();
}

//-------------------------------------------------
// Path
//-------------------------------------------------

sg::city::city::City::PathPositionContainer sg::city::city::City::Path(const int t_fromMapX, const int t_fromMapZ, const int t_toMapX, const int t_toMapZ)
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
    m_map = std::make_shared<map::Map>(t_scene);
    m_map->CreateMap(t_mapSize);
    m_map->position = glm::vec3(0.0f);
    m_map->rotation = glm::vec3(0.0f);
    m_map->scale = glm::vec3(1.0f);

    m_roadNetwork = std::make_shared<map::RoadNetwork>(m_map.get());

    m_mapRenderer = std::make_unique<renderer::MapRenderer>(t_scene);
    m_roadNetworkRenderer = std::make_unique<renderer::RoadNetworkRenderer>(t_scene);

    m_astar = std::make_unique<map::Astar>(m_map.get());

    CreateMapEntity();
    CreateRoadNetworkEntity();
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
