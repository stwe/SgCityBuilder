#pragma once

#include <memory>

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
}
