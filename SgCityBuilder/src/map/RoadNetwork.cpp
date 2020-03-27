// This file is part of the SgCityBuilder package.
// 
// Filename: RoadNetwork.cpp
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#include <Application.h>
#include <scene/Scene.h>
#include <resource/Mesh.h>
#include <resource/TextureManager.h>
#include <Core.h>
#include "RoadNetwork.h"
#include "Tile.h"
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
    map.ChangeTileTypeOnMapPosition(t_mapX, t_mapZ, Map::TileType::TRAFFIC_NETWORK);

    // get the vertices of the Tile and make a !copy!
    auto vertices{ map.GetTileByIndex(tileIndex).GetVerticesContainer() };

    // we use the same vertices as for the tile, but just a little bit higher (y = 0.001f)
    vertices[Tile::BOTTOM_LEFT_POS_Y_T1] = ROAD_VERTICES_HEIGHT;
    vertices[Tile::BOTTOM_RIGHT_POS_Y_T1] = ROAD_VERTICES_HEIGHT;
    vertices[Tile::TOP_LEFT_POS_Y_T1] = ROAD_VERTICES_HEIGHT;

    vertices[Tile::TOP_LEFT_POS_Y_T2] = ROAD_VERTICES_HEIGHT;
    vertices[Tile::BOTTOM_RIGHT_POS_Y_T2] = ROAD_VERTICES_HEIGHT;
    vertices[Tile::TOP_RIGHT_POS_Y_T2] = ROAD_VERTICES_HEIGHT;

    // set a default texture number - the value is unused
    const auto unused{ 0.0f };
    vertices[Tile::BOTTOM_LEFT_TEXTURE_NR_T1] = unused;
    vertices[Tile::BOTTOM_RIGHT_TEXTURE_NR_T1] = unused;
    vertices[Tile::TOP_LEFT_TEXTURE_NR_T1] = unused;

    vertices[Tile::TOP_LEFT_TEXTURE_NR_T2] = unused;
    vertices[Tile::BOTTOM_RIGHT_TEXTURE_NR_T2] = unused;
    vertices[Tile::TOP_RIGHT_TEXTURE_NR_T2] = unused;

    // set initial uv values
    vertices[Tile::BOTTOM_LEFT_TEXTURE_X_T1] /= TEXTURE_ATLAS_ROWS;
    vertices[Tile::BOTTOM_LEFT_TEXTURE_Y_T1] /= TEXTURE_ATLAS_ROWS;

    vertices[Tile::BOTTOM_RIGHT_TEXTURE_X_T1] /= TEXTURE_ATLAS_ROWS;
    vertices[Tile::BOTTOM_RIGHT_TEXTURE_Y_T1] /= TEXTURE_ATLAS_ROWS;

    vertices[Tile::TOP_LEFT_TEXTURE_X_T1] /= TEXTURE_ATLAS_ROWS;
    vertices[Tile::TOP_LEFT_TEXTURE_Y_T1] /= TEXTURE_ATLAS_ROWS;

    vertices[Tile::TOP_LEFT_TEXTURE_X_T2] /= TEXTURE_ATLAS_ROWS;
    vertices[Tile::TOP_LEFT_TEXTURE_Y_T2] /= TEXTURE_ATLAS_ROWS;

    vertices[Tile::BOTTOM_RIGHT_TEXTURE_X_T2] /= TEXTURE_ATLAS_ROWS;
    vertices[Tile::BOTTOM_RIGHT_TEXTURE_Y_T2] /= TEXTURE_ATLAS_ROWS;

    vertices[Tile::TOP_RIGHT_TEXTURE_X_T2] /= TEXTURE_ATLAS_ROWS;
    vertices[Tile::TOP_RIGHT_TEXTURE_Y_T2] /= TEXTURE_ATLAS_ROWS;

    // insert the Tile vertices at the end of the container with all vertices
    m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());

    // calculate the number of Tiles in m_vertices
    const auto nrTiles{ static_cast<int>(m_vertices.size()) / Tile::FLOATS_PER_TILE };

    // use the number of Tiles as offset
    m_lookupTable[tileIndex] = nrTiles;

    // update draw count
    m_roadNetworkMesh->GetVao().SetDrawCount(nrTiles * Tile::VERTICES_PER_TILE);

    // update all stored roads
    UpdateStoredRoads();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void sg::city::map::RoadNetwork::CreateVbo()
{
    m_vboId = ogl::buffer::Vbo::GenerateVbo();

    ogl::buffer::Vbo::InitEmpty(m_vboId, m_city->GetMap().GetFloatCountOfMap(), GL_DYNAMIC_DRAW);

    ogl::buffer::Vbo::AddAttribute(m_vboId, 0, 3, Tile::FLOATS_PER_VERTEX, 0);  // 3x position
    ogl::buffer::Vbo::AddAttribute(m_vboId, 1, 3, Tile::FLOATS_PER_VERTEX, 3);  // 3x normal
    ogl::buffer::Vbo::AddAttribute(m_vboId, 2, 3, Tile::FLOATS_PER_VERTEX, 6);  // 3x color
    ogl::buffer::Vbo::AddAttribute(m_vboId, 3, 1, Tile::FLOATS_PER_VERTEX, 9);  // 1x texture
    ogl::buffer::Vbo::AddAttribute(m_vboId, 4, 2, Tile::FLOATS_PER_VERTEX, 10); // 2x uv
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
}

//-------------------------------------------------
// Update
//-------------------------------------------------

sg::city::map::RoadNetwork::RoadType sg::city::map::RoadNetwork::DetermineRoadType(const Tile& t_tile)
{
    const auto& neighbours{ t_tile.GetNeighbours() };

    uint8_t roadNeighbours{ 0 };

    if (neighbours.count(Tile::Direction::NORTH))
    {
        if (neighbours.at(Tile::Direction::NORTH)->GetType() == Map::TileType::TRAFFIC_NETWORK)
        {
            roadNeighbours = NORTH;
        }
    }

    if (neighbours.count(Tile::Direction::EAST))
    {
        if (neighbours.at(Tile::Direction::EAST)->GetType() == Map::TileType::TRAFFIC_NETWORK)
        {
            roadNeighbours |= EAST;
        }
    }

    if (neighbours.count(Tile::Direction::SOUTH))
    {
        if (neighbours.at(Tile::Direction::SOUTH)->GetType() == Map::TileType::TRAFFIC_NETWORK)
        {
            roadNeighbours |= SOUTH;
        }
    }

    if (neighbours.count(Tile::Direction::WEST))
    {
        if (neighbours.at(Tile::Direction::WEST)->GetType() == Map::TileType::TRAFFIC_NETWORK)
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
    const auto nrTiles{ static_cast<int>(m_vertices.size()) / Tile::FLOATS_PER_TILE };

    // update Vbo
    if (nrTiles > 0)
    {
        ogl::buffer::Vbo::BindVbo(m_vboId);
        glBufferSubData(GL_ARRAY_BUFFER, 0, nrTiles * Tile::SIZE_IN_BYTES_PER_TILE, m_vertices.data());
        ogl::buffer::Vbo::UnbindVbo();
    }
}

void sg::city::map::RoadNetwork::UpdateAutoTracks(const int t_tileIndex, const RoadType t_roadType) const
{
    // clear the existing list of AutoTracks
    m_city->GetMap().GetTileByIndex(t_tileIndex).GetAutoTracks().clear();

    // add auto tracks
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
}

void sg::city::map::RoadNetwork::UpdateStoredRoads()
{
    // get Map
    auto& map{ m_city->GetMap() };

    // update textures && auto tracks

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

        // update uv values

        const auto offset{ (static_cast<size_t>(positionInVbo) - 1) * Tile::FLOATS_PER_TILE };

        const auto column{ roadType % static_cast<int>(TEXTURE_ATLAS_ROWS) };
        const auto xOffset{ static_cast<float>(column) / TEXTURE_ATLAS_ROWS };

        const auto row{ roadType / static_cast<int>(TEXTURE_ATLAS_ROWS) };
        const auto yOffset{ 1.0f - static_cast<float>(row) / TEXTURE_ATLAS_ROWS };

        // bl
        m_vertices[offset + Tile::BOTTOM_LEFT_TEXTURE_X_T1] = (0.0f / TEXTURE_ATLAS_ROWS) + xOffset;
        m_vertices[offset + Tile::BOTTOM_LEFT_TEXTURE_Y_T1] = (0.0f / TEXTURE_ATLAS_ROWS) + yOffset;

        // br
        m_vertices[offset + Tile::BOTTOM_RIGHT_TEXTURE_X_T1] = (1.0f / TEXTURE_ATLAS_ROWS) + xOffset;
        m_vertices[offset + Tile::BOTTOM_RIGHT_TEXTURE_Y_T1] = (0.0f / TEXTURE_ATLAS_ROWS) + yOffset;

        // tl
        m_vertices[offset + Tile::TOP_LEFT_TEXTURE_X_T1] = (0.0f / TEXTURE_ATLAS_ROWS) + xOffset;
        m_vertices[offset + Tile::TOP_LEFT_TEXTURE_Y_T1] = (1.0f / TEXTURE_ATLAS_ROWS) + yOffset;

        // tl
        m_vertices[offset + Tile::TOP_LEFT_TEXTURE_X_T2] = (0.0f / TEXTURE_ATLAS_ROWS) + xOffset;
        m_vertices[offset + Tile::TOP_LEFT_TEXTURE_Y_T2] = (1.0f / TEXTURE_ATLAS_ROWS) + yOffset;

        // br
        m_vertices[offset + Tile::BOTTOM_RIGHT_TEXTURE_X_T2] = (1.0f / TEXTURE_ATLAS_ROWS) + xOffset;
        m_vertices[offset + Tile::BOTTOM_RIGHT_TEXTURE_Y_T2] = (0.0f / TEXTURE_ATLAS_ROWS) + yOffset;

        // tr
        m_vertices[offset + Tile::TOP_RIGHT_TEXTURE_X_T2] = (1.0f / TEXTURE_ATLAS_ROWS) + xOffset;
        m_vertices[offset + Tile::TOP_RIGHT_TEXTURE_Y_T2] = (1.0f / TEXTURE_ATLAS_ROWS) + yOffset;

        // update auto tracks

        UpdateAutoTracks(tileIndex, roadTypeEnum);
        tile.CreateAutoTracksMesh(); // for debug only

        // next

        tileIndex++;
    }

    // write the new vertex data in the Vbo
    UpdateVbo();
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

        // store the auto track in the Tile
        tile->GetAutoTracks().push_back(track);

        if (t_safeCarAutoTrack)
        {
            tile->safeCarAutoTrack = track; // todo RoadTile
        }

        // store the auto track in the nodes
        tile->GetNavigationNodes()[t_fromNodeIndex]->autoTracks.push_back(track);
        tile->GetNavigationNodes()[t_toNodeIndex]->autoTracks.push_back(track);
    }
}
