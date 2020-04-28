// This file is part of the SgCityBuilder package.
// 
// Filename: City.h
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#pragma once

#include <string>
#include <memory>
#include "map/tile/Tile.h"

namespace sg::ogl::scene
{
    class Scene;
}

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

namespace sg::city::renderer
{
    class MapRenderer;
    class RoadNetworkRenderer;
    class BuildingsRenderer;
}

namespace sg::city::city
{
    class City
    {
    public:
        using MapSharedPtr = std::shared_ptr<map::Map>;
        using MapRendererUniquePtr = std::unique_ptr<renderer::MapRenderer>;

        using AutomataSharedPtr = std::shared_ptr<automata::Automata>;
        using AutomataContainer = std::list<AutomataSharedPtr>;

        using RoadNetworkSharedPtr = std::shared_ptr<map::RoadNetwork>;
        using RoadNetworkRendererUniquePtr = std::unique_ptr<renderer::RoadNetworkRenderer>;

        using BuildingGeneratorSharedPtr = std::shared_ptr<map::BuildingGenerator>;
        using BuildingsRendererUniquePtr = std::unique_ptr<renderer::BuildingsRenderer>;

        using TileIndexContainer = std::vector<int>;

        //-------------------------------------------------
        // Const
        //-------------------------------------------------

        static constexpr auto MAX_AUTOMATAS{ 8u };
        static constexpr auto ATTEMPS{ 12 };
        static constexpr auto STOP_PATTERN_SPEED{ 0.75f };

        //-------------------------------------------------
        // Public member
        //-------------------------------------------------

        AutomataContainer automatas;
        bool spawnCars{ false };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        City(std::string t_name, std::string t_mapFileName, ogl::scene::Scene* t_scene);

        City(const City& t_other) = delete;
        City(City&& t_other) noexcept = delete;
        City& operator=(const City& t_other) = delete;
        City& operator=(City&& t_other) noexcept = delete;

        ~City() noexcept;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        [[nodiscard]] const std::string& GetName() const noexcept;

        [[nodiscard]] const map::Map& GetMap() const noexcept;
        [[nodiscard]] map::Map& GetMap() noexcept;
        [[nodiscard]] MapSharedPtr GetMapSharedPtr() const;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        void Update(double t_dt, TileIndexContainer& t_tileIndexContainer);
        void Render() const;

        //-------------------------------------------------
        // Edit
        //-------------------------------------------------

        [[nodiscard]] auto ReplaceTile(int t_mapX, int t_mapZ, map::tile::TileType t_tileType) const -> std::tuple<int, bool>;

        //-------------------------------------------------
        // Spawn
        //-------------------------------------------------

        /**
         * @brief Tries to create an Automat and a Car Entity at the specified position on a RoadTile.
         * @param t_mapX Map-x position of the Tile in Object Space.
         * @param t_mapZ Map-z position of the Tile in Object Space.
         * @return True if the Automata was created successfully.
         */
        bool TrySpawnCarAtSafeTrack(int t_mapX, int t_mapZ);

        //-------------------------------------------------
        // Debug
        //-------------------------------------------------

        void RenderAutoTracks() const;

    protected:

    private:
        /**
         * @brief The name of the City;
         */
        std::string m_name;

        /**
         * @brief The name of the Map file.
         *        Contains different pixel colors for the different tile types.
         */
        std::string m_mapFileName;

        /**
         * @brief The parent Scene object.
         */
        ogl::scene::Scene* m_scene{ nullptr };

        /**
         * @brief The Map of the City holding all Tiles.
         */
        MapSharedPtr m_map;

        /**
         * @brief Renders the Map.
         */
        MapRendererUniquePtr m_mapRenderer;

        /**
         * @brief Is used as a timer to change the StopPattern.
         */
        float m_stopPatternTimer{ 0.0f };

        /**
         * @brief The RoadNetwork of the City.
         */
        RoadNetworkSharedPtr m_roadNetwork;

        /**
         * @brief Renders the RoadNetwork.
         */
        RoadNetworkRendererUniquePtr m_roadNetworkRenderer;

        /**
         * @brief Contains all buildings of the City.
         */
        BuildingGeneratorSharedPtr m_buildingGenerator;

        /**
         * @brief Renders all buildings.
         */
        BuildingsRendererUniquePtr m_buildingsRenderer;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        void Init();
        void StoreBuildings() const;
        void StoreRoads();
        void CreatePlants() const;

        //-------------------------------------------------
        // Entity
        //-------------------------------------------------

        void CreateMapEntity();
        void CreateCarEntity();
        void CreateRoadNetworkEntity();
        void CreateBuildingsEntity();
    };
}
