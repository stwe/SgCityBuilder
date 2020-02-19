#pragma once

#include <memory>

namespace sg::city::map
{
    class Map;
}

namespace sg::city::ecs
{
    struct MapComponent
    {
        std::shared_ptr<map::Map> map;
    };
}
