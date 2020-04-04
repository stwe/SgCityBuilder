// This file is part of the SgCityBuilder package.
// 
// Filename: RoadNetwork.cpp
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#include <algorithm>
#include <string>
#include <Core.h>
#include <Application.h>
#include <scene/Scene.h>
#include <resource/Mesh.h>
#include <resource/TextureManager.h>
#include "RoadNetwork.h"
#include "Map.h"
#include "city/City.h"
#include "automata/AutoNode.h"
#include "automata/AutoTrack.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

sg::city::map::RoadNetwork::RoadNetwork(city::City* t_city)
    : m_city{ t_city }
{
    SG_OGL_CORE_ASSERT(t_city, "[RoadNetwork::RoadNetwork()] Null pointer.")

    SG_OGL_LOG_DEBUG("[RoadNetwork::RoadNetwork()] Construct RoadNetwork.");

    Init();
}

sg::city::map::RoadNetwork::~RoadNetwork() noexcept
{
    SG_OGL_LOG_DEBUG("RoadNetwork::~RoadNetwork()] Destruct RoadNetwork.");
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

const sg::ogl::resource::Mesh& sg::city::map::RoadNetwork::GetMesh() const noexcept
{
    return *m_roadNetworkMesh;
}

sg::ogl::resource::Mesh& sg::city::map::RoadNetwork::GetMesh() noexcept
{
    return *m_roadNetworkMesh;
}

uint32_t sg::city::map::RoadNetwork::GetRoadTextureAtlasId() const
{
    return m_roadTextureAtlasId;
}

//-------------------------------------------------
// Add Road
//-------------------------------------------------

void sg::city::map::RoadNetwork::StoreRoadOnMapPosition(const int t_mapX, const int t_mapZ)
{
    // get Map
    auto& map{ m_city->GetMap() };

    // get Tile index by given map point
    const auto tileIndex{ map.GetTileMapIndexByMapPosition(t_mapX, t_mapZ) };

    // checks whether the Tile is already stored as a Road
    if (m_lookupTable[tileIndex] > 0)
    {
        SG_OGL_LOG_INFO("[RoadNetwork::StoreRoadOnMapPosition()] The Tile {} is already stored as a road", tileIndex);

        return;
    }

    // update TileType
    map.ChangeTileTypeOnMapPosition(t_mapX, t_mapZ, tile::TileType::TRAFFIC);

    // get the vertices of the Tile and make a !copy!
    auto vertices{ map.GetTileByIndex(tileIndex).GetVerticesContainer() };

    // we use the same vertices as for the tile, but just a little bit higher (y = 0.001f)
    vertices[tile::Tile::BOTTOM_LEFT_POS_Y_T1] = ROAD_VERTICES_HEIGHT;
    vertices[tile::Tile::BOTTOM_RIGHT_POS_Y_T1] = ROAD_VERTICES_HEIGHT;
    vertices[tile::Tile::TOP_LEFT_POS_Y_T1] = ROAD_VERTICES_HEIGHT;

    vertices[tile::Tile::TOP_LEFT_POS_Y_T2] = ROAD_VERTICES_HEIGHT;
    vertices[tile::Tile::BOTTOM_RIGHT_POS_Y_T2] = ROAD_VERTICES_HEIGHT;
    vertices[tile::Tile::TOP_RIGHT_POS_Y_T2] = ROAD_VERTICES_HEIGHT;

    // set a default texture number - the value is unused
    const auto unused{ 0.0f };
    vertices[tile::Tile::BOTTOM_LEFT_TEXTURE_NR_T1] = unused;
    vertices[tile::Tile::BOTTOM_RIGHT_TEXTURE_NR_T1] = unused;
    vertices[tile::Tile::TOP_LEFT_TEXTURE_NR_T1] = unused;

    vertices[tile::Tile::TOP_LEFT_TEXTURE_NR_T2] = unused;
    vertices[tile::Tile::BOTTOM_RIGHT_TEXTURE_NR_T2] = unused;
    vertices[tile::Tile::TOP_RIGHT_TEXTURE_NR_T2] = unused;

    // set initial uv values
    vertices[tile::Tile::BOTTOM_LEFT_TEXTURE_X_T1] /= TEXTURE_ATLAS_ROWS;
    vertices[tile::Tile::BOTTOM_LEFT_TEXTURE_Y_T1] /= TEXTURE_ATLAS_ROWS;

    vertices[tile::Tile::BOTTOM_RIGHT_TEXTURE_X_T1] /= TEXTURE_ATLAS_ROWS;
    vertices[tile::Tile::BOTTOM_RIGHT_TEXTURE_Y_T1] /= TEXTURE_ATLAS_ROWS;

    vertices[tile::Tile::TOP_LEFT_TEXTURE_X_T1] /= TEXTURE_ATLAS_ROWS;
    vertices[tile::Tile::TOP_LEFT_TEXTURE_Y_T1] /= TEXTURE_ATLAS_ROWS;

    vertices[tile::Tile::TOP_LEFT_TEXTURE_X_T2] /= TEXTURE_ATLAS_ROWS;
    vertices[tile::Tile::TOP_LEFT_TEXTURE_Y_T2] /= TEXTURE_ATLAS_ROWS;

    vertices[tile::Tile::BOTTOM_RIGHT_TEXTURE_X_T2] /= TEXTURE_ATLAS_ROWS;
    vertices[tile::Tile::BOTTOM_RIGHT_TEXTURE_Y_T2] /= TEXTURE_ATLAS_ROWS;

    vertices[tile::Tile::TOP_RIGHT_TEXTURE_X_T2] /= TEXTURE_ATLAS_ROWS;
    vertices[tile::Tile::TOP_RIGHT_TEXTURE_Y_T2] /= TEXTURE_ATLAS_ROWS;

    // insert the Tile vertices at the end of the container with all vertices
    m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());

    // calculate the number of Tiles in m_vertices
    const auto nrTiles{ static_cast<int>(m_vertices.size()) / tile::Tile::FLOATS_PER_TILE };

    // use the number of Tiles as offset
    m_lookupTable[tileIndex] = nrTiles;

    // update draw count
    m_roadNetworkMesh->GetVao().SetDrawCount(nrTiles * tile::Tile::VERTICES_PER_TILE);

    // todo: the next two methods can later be combined into one
    UpdateRoadsTextures();
    UpdateNavigation();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void sg::city::map::RoadNetwork::CreateVbo()
{
    m_vboId = ogl::buffer::Vbo::GenerateVbo();

    ogl::buffer::Vbo::InitEmpty(m_vboId, m_city->GetMap().GetFloatCountOfMap(), GL_DYNAMIC_DRAW);

    ogl::buffer::Vbo::AddAttribute(m_vboId, 0, 3, tile::Tile::FLOATS_PER_VERTEX, 0);  // 3x position
    ogl::buffer::Vbo::AddAttribute(m_vboId, 1, 3, tile::Tile::FLOATS_PER_VERTEX, 3);  // 3x normal
    ogl::buffer::Vbo::AddAttribute(m_vboId, 2, 3, tile::Tile::FLOATS_PER_VERTEX, 6);  // 3x color
    ogl::buffer::Vbo::AddAttribute(m_vboId, 3, 1, tile::Tile::FLOATS_PER_VERTEX, 9);  // 1x texture
    ogl::buffer::Vbo::AddAttribute(m_vboId, 4, 2, tile::Tile::FLOATS_PER_VERTEX, 10); // 2x uv
}

void sg::city::map::RoadNetwork::Init()
{
    SG_OGL_LOG_DEBUG("[RoadNetwork::Init()] Initialize RoadNetwork.");

    // get Map
    auto& map{ m_city->GetMap() };

    // create an bind a Vao
    m_roadNetworkMesh = std::make_unique<ogl::resource::Mesh>();
    m_roadNetworkMesh->GetVao().BindVao();

    // create Vbo
    CreateVbo();

    // unbind Vao
    ogl::buffer::Vao::UnbindVao();

    // load texture atlas
    m_roadTextureAtlasId = map.GetScene()->GetApplicationContext()->GetTextureManager().GetTextureIdFromPath("res/texture/road/roads.png");

    // Every Tile can be a Road. The lookup table stores the position of the Tile in the RoadNetwork Vbo.
    // The default value -1 (NO_ROAD) means that the Tile is currently not a Road and is therefore not in the Vbo.
    const auto lookupTableSize(map.GetMapSize() * map.GetMapSize());
    m_lookupTable.resize(lookupTableSize, NO_ROAD);

    // Almost the same as above, if the value is not NO_ROAD, the value is the current RoadType.
    m_tileRoadTypes.resize(lookupTableSize, NO_ROAD);
}

//-------------------------------------------------
// Update
//-------------------------------------------------

sg::city::map::RoadNetwork::RoadType sg::city::map::RoadNetwork::DetermineRoadType(const tile::RoadTile& t_tile)
{
    const auto& neighbours{ t_tile.GetNeighbours() };

    uint8_t roadNeighbours{ 0 };

    if (neighbours.count(tile::Direction::NORTH))
    {
        if (neighbours.at(tile::Direction::NORTH)->type == tile::TileType::TRAFFIC)
        {
            roadNeighbours = NORTH;
        }
    }

    if (neighbours.count(tile::Direction::EAST))
    {
        if (neighbours.at(tile::Direction::EAST)->type == tile::TileType::TRAFFIC)
        {
            roadNeighbours |= EAST;
        }
    }

    if (neighbours.count(tile::Direction::SOUTH))
    {
        if (neighbours.at(tile::Direction::SOUTH)->type == tile::TileType::TRAFFIC)
        {
            roadNeighbours |= SOUTH;
        }
    }

    if (neighbours.count(tile::Direction::WEST))
    {
        if (neighbours.at(tile::Direction::WEST)->type == tile::TileType::TRAFFIC)
        {
            roadNeighbours |= WEST;
        }
    }

    RoadType roadType;
    switch (roadNeighbours)
    {
    case 0:                                // keine Nachbarn
    case 1: roadType = RoadType::ROAD_V;   // Norden
        break;
    case 2: roadType = RoadType::ROAD_H;   // Osten
        break;
    case 3: roadType = RoadType::ROAD_C3;  // Norden - Osten
        break;
    case 4:                                // Sueden
    case 5: roadType = RoadType::ROAD_V;   // Sueden - Norden
        break;
    case 6: roadType = RoadType::ROAD_C1;  // Sueden - Osten
        break;
    case 7: roadType = RoadType::ROAD_T2;  // Norden - Osten - Sueden
        break;
    case 8: roadType = RoadType::ROAD_H;   // Westen
        break;
    case 9: roadType = RoadType::ROAD_C4;  // Westen - Norden
        break;
    case 10: roadType = RoadType::ROAD_H;  // Westen - Osten
        break;
    case 11: roadType = RoadType::ROAD_T4; // Westen - Osten - Norden
        break;
    case 12: roadType = RoadType::ROAD_C2; // Westen - Sueden
        break;
    case 13: roadType = RoadType::ROAD_T3; // Westen - Sueden - Norden
        break;
    case 14: roadType = RoadType::ROAD_T1; // Westen - Sueden - Osten
        break;
    case 15: roadType = RoadType::ROAD_X;
        break;
    default: roadType = RoadType::ROAD_V;
    }

    return roadType;
}

void sg::city::map::RoadNetwork::UpdateVbo()
{
    // calculate the number of Tiles in m_vertices
    const auto nrTiles{ static_cast<int>(m_vertices.size()) / tile::Tile::FLOATS_PER_TILE };

    // update Vbo
    if (nrTiles > 0)
    {
        ogl::buffer::Vbo::BindVbo(m_vboId);
        glBufferSubData(GL_ARRAY_BUFFER, 0, nrTiles * tile::Tile::SIZE_IN_BYTES_PER_TILE, m_vertices.data());
        ogl::buffer::Vbo::UnbindVbo();
    }
}

void sg::city::map::RoadNetwork::UpdateAutoTracks(const int t_tileIndex, const RoadType t_roadType) const
{
    switch (t_roadType)
    {
    case RoadType::ROAD_H: AddAutoTrack(t_tileIndex, 34, 28);
                           AddAutoTrack(t_tileIndex, 14, 20, true);
                           break;
    case RoadType::ROAD_V: AddAutoTrack(t_tileIndex, 44, 2, true);
                           AddAutoTrack(t_tileIndex, 4, 46);
                           break;

                           /*
                            *  --------->
                            * |
                            * |
                            * |
                            */
    case RoadType::ROAD_C1: AddAutoTrack(t_tileIndex, 34, 30);
                            AddAutoTrack(t_tileIndex, 30, 2);
                            AddAutoTrack(t_tileIndex, 4, 18);
                            AddAutoTrack(t_tileIndex, 18, 20);
                            break;

                            /*
                             *  <---------
                             *            |
                             *            |
                             *            |
                             */
    case RoadType::ROAD_C2: AddAutoTrack(t_tileIndex, 4, 32);
                            AddAutoTrack(t_tileIndex, 32, 28);
                            AddAutoTrack(t_tileIndex, 14, 16);
                            AddAutoTrack(t_tileIndex, 16, 2);
                            break;

                            /*
                             * |
                             * |
                             * |
                             *  --------->
                             */
    case RoadType::ROAD_C3: AddAutoTrack(t_tileIndex, 44, 16);
                            AddAutoTrack(t_tileIndex, 16, 20);
                            AddAutoTrack(t_tileIndex, 34, 32);
                            AddAutoTrack(t_tileIndex, 32, 46);
                            break;

                            /*
                             *           |
                             *           |
                             *           |
                             * <---------
                             */
    case RoadType::ROAD_C4: AddAutoTrack(t_tileIndex, 44, 30);
                            AddAutoTrack(t_tileIndex, 30, 28);
                            AddAutoTrack(t_tileIndex, 14, 18);
                            AddAutoTrack(t_tileIndex, 18, 46);
                            break;

                            /*
                             *  <--------->
                             *       |
                             *       |
                             *       |
                             */
    case RoadType::ROAD_T1: AddAutoTrack(t_tileIndex, 34, 32);
                            AddAutoTrack(t_tileIndex, 32, 30);
                            AddAutoTrack(t_tileIndex, 30, 28);

                            AddAutoTrack(t_tileIndex, 14, 16);
                            AddAutoTrack(t_tileIndex, 16, 18);
                            AddAutoTrack(t_tileIndex, 18, 20);

                            AddAutoTrack(t_tileIndex, 30, 16);
                            AddAutoTrack(t_tileIndex, 16, 2);

                            AddAutoTrack(t_tileIndex, 4, 18);
                            AddAutoTrack(t_tileIndex, 18, 32);
                            break;

                            /*
                             *       |
                             *       |
                             *       |
                             *  <--------->
                             */
    case RoadType::ROAD_T4: AddAutoTrack(t_tileIndex, 34, 32);
                            AddAutoTrack(t_tileIndex, 32, 30);
                            AddAutoTrack(t_tileIndex, 30, 28);

                            AddAutoTrack(t_tileIndex, 14, 16);
                            AddAutoTrack(t_tileIndex, 16, 18);
                            AddAutoTrack(t_tileIndex, 18, 20);

                            AddAutoTrack(t_tileIndex, 44, 30);
                            AddAutoTrack(t_tileIndex, 30, 16);

                            AddAutoTrack(t_tileIndex, 18, 32);
                            AddAutoTrack(t_tileIndex, 32, 46);
                            break;

                            /*
                             * |
                             * |________
                             * |
                             * |
                             */
    case RoadType::ROAD_T2: AddAutoTrack(t_tileIndex, 44, 30);
                            AddAutoTrack(t_tileIndex, 30, 16);
                            AddAutoTrack(t_tileIndex, 16, 2);

                            AddAutoTrack(t_tileIndex, 4, 18);
                            AddAutoTrack(t_tileIndex, 18, 32);
                            AddAutoTrack(t_tileIndex, 32, 46);

                            AddAutoTrack(t_tileIndex, 34, 32);
                            AddAutoTrack(t_tileIndex, 32, 30);

                            AddAutoTrack(t_tileIndex, 16, 18);
                            AddAutoTrack(t_tileIndex, 18, 20);
                            break;

                            /*
                             *         |
                             * ________|
                             *         |
                             *         |
                             */
    case RoadType::ROAD_T3: AddAutoTrack(t_tileIndex, 44, 30);
                            AddAutoTrack(t_tileIndex, 30, 16);
                            AddAutoTrack(t_tileIndex, 16, 2);

                            AddAutoTrack(t_tileIndex, 4, 18);
                            AddAutoTrack(t_tileIndex, 18, 32);
                            AddAutoTrack(t_tileIndex, 32, 46);

                            AddAutoTrack(t_tileIndex, 32, 30);
                            AddAutoTrack(t_tileIndex, 30, 28);

                            AddAutoTrack(t_tileIndex, 14, 16);
                            AddAutoTrack(t_tileIndex, 16, 18);
                            break;

    case RoadType::ROAD_X: AddAutoTrack(t_tileIndex, 44, 30);
                           AddAutoTrack(t_tileIndex, 30, 16);
                           AddAutoTrack(t_tileIndex, 16, 2);

                           AddAutoTrack(t_tileIndex, 4, 18);
                           AddAutoTrack(t_tileIndex, 18, 32);
                           AddAutoTrack(t_tileIndex, 32, 46);

                           AddAutoTrack(t_tileIndex, 34, 32);
                           AddAutoTrack(t_tileIndex, 32, 30);
                           AddAutoTrack(t_tileIndex, 30, 28);

                           AddAutoTrack(t_tileIndex, 14, 16);
                           AddAutoTrack(t_tileIndex, 16, 18);
                           AddAutoTrack(t_tileIndex, 18, 20);
                           break;

    default:;
    }

    auto& tile{ m_city->GetMap().GetTileByIndex(t_tileIndex) };

    switch (t_roadType)
    {
    case RoadType::ROAD_H:
    case RoadType::ROAD_V:
    case RoadType::ROAD_C1:
    case RoadType::ROAD_C2:
    case RoadType::ROAD_C3:
    case RoadType::ROAD_C4:
        break; // Allow all

/*
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                   ". . X . X . ."
                   ". . . X . . ."
                   "X . X . X . X"
                   ". X . . . X ."
                   "X . X . X . X"
                   ". . . X . . ."
                   ". . X . X . ."));
*/

    case RoadType::ROAD_X:
        // Allow West Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . X . O . ."
                   ". . . X . . ."
                   "X . X . O . X"
                   ". X . . . X ."
                   "O . O . O . O"
                   ". . . X . . ."
                   ". . O . X . ."));

        // Stop West Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . X . O . ."
                   ". . . X . . ."
                   "X . X . O . X"
                   ". X . . . X ."
                   "X . O . O . O"
                   ". . . X . . ."
                   ". . O . X . ."));

        // Stop North Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . O . X . ."
                   ". . . X . . ."
                   "O . O . X . X"
                   ". X . . . X ."
                   "X . O . O . O"
                   ". . . X . . ."
                   ". . O . X . ."));

        // Stop North Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . X . X . ."
                   ". . . X . . ."
                   "O . O . X . X"
                   ". X . . . X ."
                   "X . O . O . O"
                   ". . . X . . ."
                   ". . O . X . ."));

        // Allow East Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . X . O . ."
                   ". . . X . . ."
                   "O . O . O . O"
                   ". X . . . X ."
                   "X . O . X . X"
                   ". . . X . . ."
                   ". . O . X . ."));

        // Stop East Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . X . O . ."
                   ". . . X . . ."
                   "O . O . O . X"
                   ". X . . . X ."
                   "X . O . X . X"
                   ". . . X . . ."
                   ". . O . X . ."));

        // Allow South Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . X . O . ."
                   ". . . X . . ."
                   "O . O . O . X"
                   ". X . . . X ."
                   "X . X . O . O"
                   ". . . X . . ."
                   ". . X . O . ."));

        // Allow South Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . X . O . ."
                   ". . . X . . ."
                   "O . O . O . X"
                   ". X . . . X ."
                   "X . X . O . O"
                   ". . . X . . ."
                   ". . X . X . ."));

        break;

    case RoadType::ROAD_T1:
        // Allow West Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . X . X . ."
                   ". . . X . . ."
                   "X . X . X . X"
                   ". X . . . X ."
                   "O . O . O . O"
                   ". . . X . . ."
                   ". . O . X . ."));

        // Stop West Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . X . X . ."
                   ". . . X . . ."
                   "X . X . X . X"
                   ". X . . . X ."
                   "X . O . O . O"
                   ". . . X . . ."
                   ". . O . X . ."));

        // Allow East Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . X . X . ."
                   ". . . X . . ."
                   "O . O . O . O"
                   ". X . . . X ."
                   "X . O . X . X"
                   ". . . X . . ."
                   ". . O . X . ."));

        // Stop East Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . X . X . ."
                   ". . . X . . ."
                   "O . O . O . X"
                   ". X . . . X ."
                   "X . O . X . X"
                   ". . . X . . ."
                   ". . O . X . ."));

        // Allow South Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . X . X . ."
                   ". . . X . . ."
                   "O . O . O . X"
                   ". X . . . X ."
                   "X . X . O . O"
                   ". . . X . . ."
                   ". . X . O . ."));

        // Stop South Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . X . X . ."
                   ". . . X . . ."
                   "O . O . O . X"
                   ". X . . . X ."
                   "X . X . O . O"
                   ". . . X . . ."
                   ". . X . X . ."));

        break;

    case RoadType::ROAD_T2:
        // Allow North Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . O . X . ."
                   ". . . X . . ."
                   "X . O . X . X"
                   ". . . . . X ."
                   "X . O . O . O"
                   ". . . X . . ."
                   ". . O . X . ."));

        // Stop North Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . X . X . ."
                   ". . . X . . ."
                   "X . O . X . X"
                   ". . . . . X ."
                   "X . O . O . O"
                   ". . . X . . ."
                   ". . O . X . ."));

        // Allow East Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . X . O . ."
                   ". . . X . . ."
                   "X . O . O . O"
                   ". . . . . X ."
                   "X . O . X . X"
                   ". . . X . . ."
                   ". . O . X . ."));

        // Stop East Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . X . O . ."
                   ". . . X . . ."
                   "X . O . O . X"
                   ". . . . . X ."
                   "X . O . X . X"
                   ". . . X . . ."
                   ". . O . X . ."));

        // Allow South Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . X . O . ."
                   ". . . X . . ."
                   "X . X . O . X"
                   ". . . . . X ."
                   "X . X . O . O"
                   ". . . X . . ."
                   ". . X . O . ."));

        // Stop South Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . X . O . ."
                   ". . . X . . ."
                   "X . X . O . X"
                   ". . . . . X ."
                   "X . X . O . O"
                   ". . . X . . ."
                   ". . X . X . ."));

        break;

    case RoadType::ROAD_T3:
        // Allow West Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . X . O . ."
                   ". . . X . . ."
                   "X . X . O . X"
                   ". X . . . X ."
                   "O . O . O . X"
                   ". . . X . . ."
                   ". . O . X . ."));

        // Stop West Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . X . O . ."
                   ". . . X . . ."
                   "X . X . O . X"
                   ". X . . . X ."
                   "X . O . O . X"
                   ". . . X . . ."
                   ". . O . X . ."));

        // Allow North Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . O . X . ."
                   ". . . X . . ."
                   "O . O . X . X"
                   ". X . . . X ."
                   "X . O . X . X"
                   ". . . X . . ."
                   ". . O . X . ."));

        // Stop North Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . X . X . ."
                   ". . . X . . ."
                   "O . O . X . X"
                   ". X . . . X ."
                   "X . O . X . X"
                   ". . . X . . ."
                   ". . O . X . ."));

        // Allow South Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . X . O . ."
                   ". . . X . . ."
                   "O . O . O . X"
                   ". X . . . X ."
                   "X . X . O . X"
                   ". . . X . . ."
                   ". . X . O . ."));

        // Stop South Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . X . O . ."
                   ". . . X . . ."
                   "O . O . O . X"
                   ". X . . . X ."
                   "X . X . O . X"
                   ". . . X . . ."
                   ". . X . X . ."));

        break;

    case RoadType::ROAD_T4: // w n e
        // Allow West Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . X . O . ."
                   ". . . X . . ."
                   "X . X . O . X"
                   ". X . . . X ."
                   "O . O . O . O"
                   ". . . X . . ."
                   ". . X . X . ."));

        // Stop West Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . X . O . ."
                   ". . . X . . ."
                   "X . X . O . X"
                   ". X . . . X ."
                   "X . O . O . O"
                   ". . . X . . ."
                   ". . X . X . ."));

        // Allow North Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . O . X . ."
                   ". . . X . . ."
                   "O . O . X . X"
                   ". X . . . X ."
                   "X . O . O . O"
                   ". . . X . . ."
                   ". . X . X . ."));

        // Stop North Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . X . X . ."
                   ". . . X . . ."
                   "O . O . X . X"
                   ". X . . . X ."
                   "X . O . O . O"
                   ". . . X . . ."
                   ". . X . X . ."));

        // Allow East Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . X . O . ."
                   ". . . X . . ."
                   "O . O . O . O"
                   ". X . . . X ."
                   "X . X . X . X"
                   ". . . X . . ."
                   ". . X . X . ."));

        // Stop East Traffic
        tile.GetStopPatterns().push_back(
            CreateStopPattern(
                ". . X . O . ."
                   ". . . X . . ."
                   "O . O . O . X"
                   ". X . . . X ."
                   "X . X . X . X"
                   ". . . X . . ."
                   ". . X . X . ."));

        break;

    default:;
    }

    if (!tile.GetStopPatterns().empty())
    {
        auto i{ 0 };
        for (auto& node : tile.GetNavigationNodes())
        {
            if (node)
            {
                node->block = tile.GetStopPatterns()[0][i];
            }
            i++;
        }
    }

    tile.CreateNavigationNodesMesh();
}

void sg::city::map::RoadNetwork::UpdateRoadsTextures()
{
    // get Map
    auto& map{ m_city->GetMap() };

    // update RoadType and the uv values for texturing
    auto tileIndex{ 0 };
    for (auto positionInVbo : m_lookupTable)
    {
        // only update road Tiles that are already in the Vbo
        if (positionInVbo == NO_ROAD)
        {
            tileIndex++;
            continue; // skip entry
        }

        // get the Tile
        auto& tile{ map.GetTileByIndex(tileIndex) };

        // determine the RoadType depending on the road direction and the neighbors
        const auto roadTypeEnum{ DetermineRoadType(tile) };
        const auto roadType{ static_cast<int>(roadTypeEnum) };

        // store the RoadType
        m_tileRoadTypes[tileIndex] = roadType;

        // update uv values

        const auto offset{ (static_cast<size_t>(positionInVbo) - 1) * tile::Tile::FLOATS_PER_TILE };

        const auto column{ roadType % static_cast<int>(TEXTURE_ATLAS_ROWS) };
        const auto xOffset{ static_cast<float>(column) / TEXTURE_ATLAS_ROWS };

        const auto row{ roadType / static_cast<int>(TEXTURE_ATLAS_ROWS) };
        const auto yOffset{ 1.0f - static_cast<float>(row) / TEXTURE_ATLAS_ROWS };

        // bl
        m_vertices[offset + tile::Tile::BOTTOM_LEFT_TEXTURE_X_T1] = (0.0f / TEXTURE_ATLAS_ROWS) + xOffset;
        m_vertices[offset + tile::Tile::BOTTOM_LEFT_TEXTURE_Y_T1] = (0.0f / TEXTURE_ATLAS_ROWS) + yOffset;

        // br
        m_vertices[offset + tile::Tile::BOTTOM_RIGHT_TEXTURE_X_T1] = (1.0f / TEXTURE_ATLAS_ROWS) + xOffset;
        m_vertices[offset + tile::Tile::BOTTOM_RIGHT_TEXTURE_Y_T1] = (0.0f / TEXTURE_ATLAS_ROWS) + yOffset;

        // tl
        m_vertices[offset + tile::Tile::TOP_LEFT_TEXTURE_X_T1] = (0.0f / TEXTURE_ATLAS_ROWS) + xOffset;
        m_vertices[offset + tile::Tile::TOP_LEFT_TEXTURE_Y_T1] = (1.0f / TEXTURE_ATLAS_ROWS) + yOffset;

        // tl
        m_vertices[offset + tile::Tile::TOP_LEFT_TEXTURE_X_T2] = (0.0f / TEXTURE_ATLAS_ROWS) + xOffset;
        m_vertices[offset + tile::Tile::TOP_LEFT_TEXTURE_Y_T2] = (1.0f / TEXTURE_ATLAS_ROWS) + yOffset;

        // br
        m_vertices[offset + tile::Tile::BOTTOM_RIGHT_TEXTURE_X_T2] = (1.0f / TEXTURE_ATLAS_ROWS) + xOffset;
        m_vertices[offset + tile::Tile::BOTTOM_RIGHT_TEXTURE_Y_T2] = (0.0f / TEXTURE_ATLAS_ROWS) + yOffset;

        // tr
        m_vertices[offset + tile::Tile::TOP_RIGHT_TEXTURE_X_T2] = (1.0f / TEXTURE_ATLAS_ROWS) + xOffset;
        m_vertices[offset + tile::Tile::TOP_RIGHT_TEXTURE_Y_T2] = (1.0f / TEXTURE_ATLAS_ROWS) + yOffset;

        // next
        tileIndex++;
    }

    // write the new vertex data in the Vbo
    UpdateVbo();
}

void sg::city::map::RoadNetwork::UpdateNavigation()
{
    // clear all auto tracks from the Tile and Navigation Nodes
    for (auto& tile : m_city->GetMap().GetTiles())
    {
        tile->GetAutoTracks().clear();

        for (auto& node : tile->GetNavigationNodes())
        {
            if (node)
            {
                node->autoTracks.clear();
            }
        }
    }

    auto tileIndex{ 0 };
    for (auto positionInVbo : m_lookupTable)
    {
        // only update road Tiles that are already in the Vbo
        if (positionInVbo == NO_ROAD)
        {
            tileIndex++;
            continue; // skip entry
        }

        UpdateAutoTracks(tileIndex, static_cast<RoadType>(m_tileRoadTypes[tileIndex]));
        //m_city->GetMap().GetTiles()[tileIndex]->CreateAutoTracksMesh(); // for debug only

        // next
        tileIndex++;
    }
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

void sg::city::map::RoadNetwork::AddAutoTrack(const int t_tileIndex, const int t_fromNodeIndex, const int t_toNodeIndex, const bool t_safeCarAutoTrack) const
{
    SG_OGL_CORE_ASSERT(t_fromNodeIndex >= 0 && t_fromNodeIndex <= 48, "[RoadNetwork::AddAutoTrack()] Invalid from index.")
    SG_OGL_CORE_ASSERT(t_toNodeIndex >= 0 && t_toNodeIndex <= 48, "[RoadNetwork::AddAutoTrack()] Invalid to index.")

    // get Tile shared_ptr
    auto tile{ m_city->GetMap().GetTilePtrByIndex(t_tileIndex) };

    if (tile->GetNavigationNodes()[t_fromNodeIndex] && tile->GetNavigationNodes()[t_toNodeIndex])
    {
        // generate a new auto track
        auto track{ std::make_shared<automata::AutoTrack>() };
        track->startNode = tile->GetNavigationNodes()[t_fromNodeIndex];
        track->endNode = tile->GetNavigationNodes()[t_toNodeIndex];
        track->tile = tile;
        track->trackLength = length(track->startNode->position - track->endNode->position);

        // inserting at end of Tile track list
        tile->GetAutoTracks().push_back(track);

        if (t_safeCarAutoTrack)
        {
            tile->safeCarAutoTrack = track; // todo RoadTile
        }

        /*
        SG_OGL_LOG_INFO("---------------------");
        SG_OGL_LOG_INFO("| New Track created |");
        SG_OGL_LOG_INFO("---------------------");
        SG_OGL_LOG_INFO("Tileindex: {}", t_tileIndex);
        SG_OGL_LOG_INFO("Track address: {}", (void*)track.get());
        SG_OGL_LOG_INFO("Track is safe: {}", t_safeCarAutoTrack);
        SG_OGL_LOG_INFO("Track from {} --> to {}", t_fromNodeIndex, t_toNodeIndex);
        SG_OGL_LOG_INFO("Track length: {}", track->trackLength);
        SG_OGL_LOG_INFO("New number of Tracks in the Tile: {}", tile->GetAutoTracks().size());
        */

        // store the auto track in the nodes
        auto& from{ tile->GetNavigationNodes()[t_fromNodeIndex] };
        auto& to{ tile->GetNavigationNodes()[t_toNodeIndex] };

        from->autoTracks.push_back(tile->GetAutoTracks().back());
        to->autoTracks.push_back(tile->GetAutoTracks().back());

        /*
        SG_OGL_LOG_INFO("");

        for (auto& trackf : from->autoTracks)
        {
            SG_OGL_LOG_INFO("Track address: {} is added to node: {}", (void*)trackf.get(), t_fromNodeIndex);

            SG_OGL_LOG_INFO("node From {}  - the track start at x: {}", t_fromNodeIndex, trackf->startNode->position.x);
            SG_OGL_LOG_INFO("node From {}  - the track start at z: {}", t_fromNodeIndex, trackf->startNode->position.z);

            SG_OGL_LOG_INFO("node From {}  - the track end at x: {}", t_fromNodeIndex, trackf->endNode->position.x);
            SG_OGL_LOG_INFO("node From {}  - the track end at z: {}", t_fromNodeIndex, trackf->endNode->position.z);

            SG_OGL_LOG_INFO("");
        }

        for (auto& trackt : to->autoTracks)
        {
            SG_OGL_LOG_INFO("Track address: {} is added to node: {}", (void*)trackt.get(), t_toNodeIndex);

            SG_OGL_LOG_INFO("node To {}  - track start x: {}", t_toNodeIndex, trackt->startNode->position.x);
            SG_OGL_LOG_INFO("node To {}  - track start z: {}", t_toNodeIndex, trackt->startNode->position.z);

            SG_OGL_LOG_INFO("node To {}  - track end x: {}", t_toNodeIndex, trackt->endNode->position.x);
            SG_OGL_LOG_INFO("node To {}  - track end z: {}", t_toNodeIndex, trackt->endNode->position.z);

            SG_OGL_LOG_INFO("");
        }

        SG_OGL_LOG_INFO("node From {} count tracks: {} ", t_fromNodeIndex, from->autoTracks.size());
        SG_OGL_LOG_INFO("node To {} count tracks: {} ", t_toNodeIndex, to->autoTracks.size());

        SG_OGL_LOG_INFO("");
        SG_OGL_LOG_INFO("");
        */
    }
}

sg::city::map::tile::RoadTile::StopPattern sg::city::map::RoadNetwork::CreateStopPattern(std::string t_s) const
{
    t_s.erase(std::remove(t_s.begin(), t_s.end(), ' '), t_s.end());

    SG_OGL_ASSERT(t_s.size() == NODES_PER_TILE, "[RoadNetwork::CreateStopPattern()] Invalid string size.");

    tile::RoadTile::StopPattern p;
    p.resize(NODES_PER_TILE, false);

    auto i{ 0 };
    for (auto z{ 6 }; z >= 0; --z)
    {
        for (auto x{ 0 }; x < 7; ++x)
        {
            p[i] = t_s[z * 7 + x] == STOP;
            i++;
        }
    }

    return p;
}
