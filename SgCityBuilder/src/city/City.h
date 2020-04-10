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
}

namespace sg::city::renderer
{
    class MapRenderer;
}

namespace sg::city::city
{
    class City
    {
    public:
        using MapSharedPtr = std::shared_ptr<map::Map>;
        using MapRendererUniquePtr = std::unique_ptr<renderer::MapRenderer>;

        using TileIndexContainer = std::vector<int>;

        using AutomataSharedPtr = std::shared_ptr<automata::Automata>;
        using AutomataContainer = std::list<AutomataSharedPtr>;

        //-------------------------------------------------
        // Const
        //-------------------------------------------------



        //-------------------------------------------------
        // Public member
        //-------------------------------------------------

        AutomataContainer automatas;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        City(std::string t_name, ogl::scene::Scene* t_scene, int t_mapSize = 64);

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

        void Update(double t_dt, TileIndexContainer& t_changedTiles);
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

    protected:

    private:
        /**
         * @brief The name of the City;
         */
        std::string m_name;

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

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        void Init(ogl::scene::Scene* t_scene, int t_mapSize);

        //-------------------------------------------------
        // Entity
        //-------------------------------------------------

        void CreateMapEntity();
        void CreateCarEntity();
    };
}
