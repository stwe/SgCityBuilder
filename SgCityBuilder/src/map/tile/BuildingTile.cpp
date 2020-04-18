// This file is part of the SgCityBuilder package.
// 
// Filename: BuildingTile.cpp
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#include <Log.h>
#include <Core.h>
#include "BuildingTile.h"
#include "map/Map.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

sg::city::map::tile::BuildingTile::BuildingTile(const float t_mapX, const float t_mapZ, const TileType t_type, Map* t_map)
    : Tile(t_mapX, t_mapZ, t_type, t_map)
{
    SG_OGL_ASSERT(t_type == TileType::RESIDENTIAL, "[BuildingTile::BuildingTile()] Invalid Tile Type.")
}

sg::city::map::tile::BuildingTile::~BuildingTile() noexcept
{
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void sg::city::map::tile::BuildingTile::Update()
{
    SG_OGL_LOG_INFO("[BuildingTile::Update()] Start BuildingTile update process.");
}
