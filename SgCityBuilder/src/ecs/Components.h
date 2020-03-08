#pragma once

#include <memory>
#include <stack>

namespace sg::city::map
{
    class Map;
    class RoadNetwork;
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

    struct PathComponent
    {
        std::stack<glm::vec2> waypoints;
        float offsetX{ 0.0f };
        float offsetZ{ 0.0f };
    };
}
