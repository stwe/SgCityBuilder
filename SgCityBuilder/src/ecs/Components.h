#pragma once

#include <memory>
#include <stack>

namespace sg::city::map
{
    class Map;
    class RoadNetwork;
    class BuildingGenerator;
}

namespace sg::city::ecs
{
    struct MapComponent
    {
        std::shared_ptr<map::Map> map;
    };

    struct RoadNetworkComponent
    {
        std::shared_ptr<map::RoadNetwork> roadNetwork;
    };

    struct BuildingsComponent
    {
        std::shared_ptr<map::BuildingGenerator> buildingGenerator;
    };

    struct PathComponent
    {
        std::stack<glm::vec2> waypoints;
        float offsetX{ 0.0f };
        float offsetZ{ 0.0f };
    };
}
