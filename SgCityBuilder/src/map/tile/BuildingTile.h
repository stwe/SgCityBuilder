// This file is part of the SgCityBuilder package.
// 
// Filename: BuildingTile.h
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#pragma once

#include "Tile.h"

namespace sg::city::map::tile
{
    class BuildingTile : public Tile
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        BuildingTile() = delete;

        BuildingTile(float t_mapX, float t_mapZ, TileType t_type, Map* t_map);

        BuildingTile(const BuildingTile& t_other) = delete;
        BuildingTile(BuildingTile&& t_other) noexcept = delete;
        BuildingTile& operator=(const BuildingTile& t_other) = delete;
        BuildingTile& operator=(BuildingTile&& t_other) noexcept = delete;

        virtual ~BuildingTile() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        void Update() override;

    protected:

    private:

    };
}
