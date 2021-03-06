// This file is part of the SgCityBuilder package.
// 
// Filename: Components.h
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#pragma once

#include <memory>
#include <stack>

namespace sg::city::automata
{
    class Automata;
}

namespace sg::city::map
{
    class Map;
    class RoadNetwork;
    class BuildingGenerator;
}

namespace sg::city::ecs
{
    struct AutomataComponent
    {
        std::shared_ptr<automata::Automata> automata;
    };

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
