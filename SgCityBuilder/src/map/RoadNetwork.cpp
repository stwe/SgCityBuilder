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
#include "RoadNetwork.h"
#include "Tile.h"
#include "Map.h"
#include "city/City.h"
#include "automata/AutoNode.h"
#include "automata/AutoTrack.h"
#include "util/Debug.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

sg::city::map::RoadNetwork::RoadNetwork(city::City* t_city)
    : m_city{ t_city }
{
    Init();
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

void sg::city::map::RoadNetwork::StoreRoadOnPosition(const glm::vec3& t_mapPoint)
{
    // get Map
    auto& map{ m_city->GetMap() };

    // get Tile index by given map point
    //const auto tileIndex{ map.GetTileIndexByPosition(t_mapPoint) };

    // checks whether the Tile is already stored as a Road
    /*
    if (m_lookupTable[tileIndex] > 0)
    {
        return;
    }
    */

    // update TileType
    //map.ChangeTileTypeOnPosition(t_mapPoint, Map::TileType::TRAFFIC_NETWORK);

    /*

    // get the vertices of the Tile and make a !copy!
    auto vertices{ map.GetTileByIndex(tileIndex).GetVerticesContainer() };

    // we use the same vertices of the tile, but just a little bit higher (y = 0.001f)
    vertices[1] = 0.001f;
    vertices[13] = 0.001f;
    vertices[25] = 0.001f;
    vertices[37] = 0.001f;
    vertices[49] = 0.001f;
    vertices[61] = 0.001f;

    // set a default texture value
    const auto unused{ 0.0f };
    vertices[9] = unused;
    vertices[21] = unused;
    vertices[33] = unused;
    vertices[45] = unused;
    vertices[57] = unused;
    vertices[69] = unused;

    // set initial uv values
    vertices[10] /= TEXTURE_ATLAS_ROWS;
    vertices[11] /= TEXTURE_ATLAS_ROWS;

    vertices[22] /= TEXTURE_ATLAS_ROWS;
    vertices[23] /= TEXTURE_ATLAS_ROWS;

    vertices[34] /= TEXTURE_ATLAS_ROWS;
    vertices[35] /= TEXTURE_ATLAS_ROWS;

    vertices[46] /= TEXTURE_ATLAS_ROWS;
    vertices[47] /= TEXTURE_ATLAS_ROWS;

    vertices[58] /= TEXTURE_ATLAS_ROWS;
    vertices[59] /= TEXTURE_ATLAS_ROWS;

    vertices[70] /= TEXTURE_ATLAS_ROWS;
    vertices[71] /= TEXTURE_ATLAS_ROWS;

    // insert the Tile vertices at the end of the container with all vertices
    m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());

    // calculate the number of Tiles in m_vertices
    const auto nrTiles{ static_cast<int>(m_vertices.size()) / Tile::FLOATS_PER_TILE };

    // use the number of Tiles as offset
    m_lookupTable[tileIndex] = nrTiles;

    // update draw count
    m_roadNetworkMesh->GetVao().SetDrawCount(nrTiles * Tile::VERTICES_PER_TILE);

    // update the RoadNetwork
    Update();
    */
}

void sg::city::map::RoadNetwork::StoreRoadOnPosition(const int t_mapX, const int t_mapZ)
{
    StoreRoadOnPosition(glm::vec3(static_cast<float>(t_mapX), 0.0f, static_cast<float>(t_mapZ)));
}

//-------------------------------------------------
// Update
//-------------------------------------------------

// todo: make private
void sg::city::map::RoadNetwork::Update()
{
    // get Map
    auto& map{ m_city->GetMap() };

    auto tileIndex{ 0 };
    for (auto positionInVbo : m_lookupTable)
    {
        if (positionInVbo == NO_ROAD)
        {
            tileIndex++;
            continue; // skip entry
        }

        // get the Tile
        auto& tile{ map.GetTileByIndex(tileIndex) };

        // determine the RoadType depending on the road direction and the neighbors
        const auto roadType{ static_cast<int>(GetRoadType(tile)) };

        // update uv values
        const auto offset{ (positionInVbo - 1) * Tile::FLOATS_PER_TILE };

        const auto column{ roadType % static_cast<int>(TEXTURE_ATLAS_ROWS) };
        const auto xOffset{ static_cast<float>(column) / TEXTURE_ATLAS_ROWS };

        const auto row{ roadType / static_cast<int>(TEXTURE_ATLAS_ROWS) };
        const auto yOffset{ 1.0f - static_cast<float>(row) / TEXTURE_ATLAS_ROWS };

        // bl
        m_vertices[static_cast<size_t>(offset) + 10] = (0.0f / TEXTURE_ATLAS_ROWS) + xOffset;
        m_vertices[static_cast<size_t>(offset) + 11] = (0.0f / TEXTURE_ATLAS_ROWS) + yOffset;

        // br
        m_vertices[static_cast<size_t>(offset) + 22] = (1.0f / TEXTURE_ATLAS_ROWS) + xOffset;
        m_vertices[static_cast<size_t>(offset) + 23] = (0.0f / TEXTURE_ATLAS_ROWS) + yOffset;

        // tl
        m_vertices[static_cast<size_t>(offset) + 34] = (0.0f / TEXTURE_ATLAS_ROWS) + xOffset;
        m_vertices[static_cast<size_t>(offset) + 35] = (1.0f / TEXTURE_ATLAS_ROWS) + yOffset;

        // tl
        m_vertices[static_cast<size_t>(offset) + 46] = (0.0f / TEXTURE_ATLAS_ROWS) + xOffset;
        m_vertices[static_cast<size_t>(offset) + 47] = (1.0f / TEXTURE_ATLAS_ROWS) + yOffset;

        // br
        m_vertices[static_cast<size_t>(offset) + 58] = (1.0f / TEXTURE_ATLAS_ROWS) + xOffset;
        m_vertices[static_cast<size_t>(offset) + 59] = (0.0f / TEXTURE_ATLAS_ROWS) + yOffset;

        // tr
        m_vertices[static_cast<size_t>(offset) + 70] = (1.0f / TEXTURE_ATLAS_ROWS) + xOffset;
        m_vertices[static_cast<size_t>(offset) + 71] = (1.0f / TEXTURE_ATLAS_ROWS) + yOffset;

        tileIndex++;
    }

    UpdateVbo();

    UpdateAutoTracks();

    m_city->GetDebug().UpdateAutoNodesMesh(true);
    m_city->GetDebug().UpdateAutoTrackMeshes();
}

void sg::city::map::RoadNetwork::UpdateAutoTracks()
{
    // get Map
    auto& map{ m_city->GetMap() };

    auto tileIndex{ 0 };
    for (auto positionInVbo : m_lookupTable)
    {
        if (positionInVbo == NO_ROAD)
        {
            tileIndex++;
            continue; // skip entry
        }

        // get the Tile
        auto& tile{ map.GetTileByIndex(tileIndex) };

        // clear the list of AutoTracks
        tile.GetAutoTracks().clear();

        // add traffic auto tracks
        switch (GetRoadType(tile))
        {
        case RoadType::ROAD_H: AddAutoTrack(tile, 28, 34);
                               AddAutoTrack(tile, 14, 20, true);
                               break;
        case RoadType::ROAD_V: AddAutoTrack(tile, 44, 2, true);
                               AddAutoTrack(tile, 46, 4);
                               break;

        case RoadType::ROAD_C1: AddAutoTrack(tile, 2, 30);
                                AddAutoTrack(tile, 30, 34);
                                AddAutoTrack(tile, 4, 18);
                                AddAutoTrack(tile, 18, 20);
                                break;
        case RoadType::ROAD_C2: AddAutoTrack(tile, 28, 32);
                                AddAutoTrack(tile, 32, 4);
                                AddAutoTrack(tile, 14, 16);
                                AddAutoTrack(tile, 16, 2);
                                break;
        case RoadType::ROAD_C3: AddAutoTrack(tile, 44, 16);
                                AddAutoTrack(tile, 16, 20);
                                AddAutoTrack(tile, 46, 32);
                                AddAutoTrack(tile, 32, 34);
                                break;
        case RoadType::ROAD_C4: AddAutoTrack(tile, 44, 30);
                                AddAutoTrack(tile, 30, 28);
                                AddAutoTrack(tile, 46, 18);
                                AddAutoTrack(tile, 18, 14);
                                break;

        case RoadType::ROAD_T1: AddAutoTrack(tile, 28, 30);
                                AddAutoTrack(tile, 30, 32);
                                AddAutoTrack(tile, 32, 34);
                                AddAutoTrack(tile, 14, 16);
                                AddAutoTrack(tile, 16, 18);
                                AddAutoTrack(tile, 18, 20);
                                AddAutoTrack(tile, 30, 16);
                                AddAutoTrack(tile, 16, 2);
                                AddAutoTrack(tile, 32, 18);
                                AddAutoTrack(tile, 18, 4);
                                break;
        case RoadType::ROAD_T4: AddAutoTrack(tile, 28, 30);
                                AddAutoTrack(tile, 30, 32);
                                AddAutoTrack(tile, 32, 34);
                                AddAutoTrack(tile, 14, 16);
                                AddAutoTrack(tile, 16, 18);
                                AddAutoTrack(tile, 18, 20);
                                AddAutoTrack(tile, 30, 16);
                                AddAutoTrack(tile, 30, 44);
                                AddAutoTrack(tile, 32, 18);
                                AddAutoTrack(tile, 32, 46);
                                break;
        case RoadType::ROAD_T2: AddAutoTrack(tile, 44, 30);
                                AddAutoTrack(tile, 30, 16);
                                AddAutoTrack(tile, 16, 2);
                                AddAutoTrack(tile, 46, 32);
                                AddAutoTrack(tile, 32, 18);
                                AddAutoTrack(tile, 18, 4);
                                AddAutoTrack(tile, 30, 32);
                                AddAutoTrack(tile, 32, 34);
                                AddAutoTrack(tile, 16, 18);
                                AddAutoTrack(tile, 18, 20);
                                break;
        case RoadType::ROAD_T3: AddAutoTrack(tile, 44, 30);
                                AddAutoTrack(tile, 30, 16);
                                AddAutoTrack(tile, 16, 2);
                                AddAutoTrack(tile, 46, 32);
                                AddAutoTrack(tile, 32, 18);
                                AddAutoTrack(tile, 18, 4);
                                AddAutoTrack(tile, 28, 30);
                                AddAutoTrack(tile, 30, 32);
                                AddAutoTrack(tile, 14, 16);
                                AddAutoTrack(tile, 16, 18);
                                break;

        case RoadType::ROAD_X: AddAutoTrack(tile, 44, 30);
                               AddAutoTrack(tile, 30, 16);
                               AddAutoTrack(tile, 16, 2);
                               AddAutoTrack(tile, 46, 32);
                               AddAutoTrack(tile, 32, 18);
                               AddAutoTrack(tile, 18, 4);
                               AddAutoTrack(tile, 28, 30);
                               AddAutoTrack(tile, 30, 32);
                               AddAutoTrack(tile, 32, 34);
                               AddAutoTrack(tile, 14, 16);
                               AddAutoTrack(tile, 16, 18);
                               AddAutoTrack(tile, 18, 20);
                               break;

        default:;
        }

        tileIndex++;
    }
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

sg::city::map::RoadNetwork::RoadType sg::city::map::RoadNetwork::GetRoadType(const Tile& t_tile)
{
    const auto& neighbours{ t_tile.GetNeighbours() };

    uint8_t roadNeighbours{ 0 };

    /*
    if (neighbours[static_cast<int>(Tile::Direction::NORTH)])
    {
        if (neighbours[static_cast<int>(Tile::Direction::NORTH)]->GetType() == Map::TileType::TRAFFIC_NETWORK)
        {
            roadNeighbours = NORTH;
        }
    }

    if (neighbours[static_cast<int>(Tile::Direction::EAST)])
    {
        if (neighbours[static_cast<int>(Tile::Direction::EAST)]->GetType() == Map::TileType::TRAFFIC_NETWORK)
        {
            roadNeighbours |= EAST;
        }
    }

    if (neighbours[static_cast<int>(Tile::Direction::SOUTH)])
    {
        if (neighbours[static_cast<int>(Tile::Direction::SOUTH)]->GetType() == Map::TileType::TRAFFIC_NETWORK)
        {
            roadNeighbours |= SOUTH;
        }
    }

    if (neighbours[static_cast<int>(Tile::Direction::WEST)])
    {
        if (neighbours[static_cast<int>(Tile::Direction::WEST)]->GetType() == Map::TileType::TRAFFIC_NETWORK)
        {
            roadNeighbours |= WEST;
        }
    }

    RoadType roadType;
    switch (roadNeighbours)
    {
    case 0: roadType = RoadType::ROAD_V;   // keine Nachbarn
        break;
    case 1: roadType = RoadType::ROAD_V;   // Norden
        break;
    case 2: roadType = RoadType::ROAD_H;   // Osten
        break;
    case 3: roadType = RoadType::ROAD_C3;  // Norden - Osten
        break;
    case 4: roadType = RoadType::ROAD_V;   // Sueden
        break;
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
    */
    RoadType roadType;
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

//-------------------------------------------------
// Helper
//-------------------------------------------------

void sg::city::map::RoadNetwork::AddAutoTrack(Tile& t_tile, const int t_fromNodeIndex, const int t_toNodeIndex, const bool t_safeCarAutoTrack) const
{
    if (t_tile.GetNavigationNodes()[t_fromNodeIndex] && t_tile.GetNavigationNodes()[t_toNodeIndex])
    {
        auto track{ std::make_shared<automata::AutoTrack>() };
        track->startNode = t_tile.GetNavigationNodes()[t_fromNodeIndex];
        track->endNode = t_tile.GetNavigationNodes()[t_toNodeIndex];
        track->tile = &t_tile;

        t_tile.GetAutoTracks().push_back(track);

        if (t_safeCarAutoTrack)
        {
            t_tile.safeCarAutoTrack = track;
        }

        t_tile.GetNavigationNodes()[t_fromNodeIndex]->autoTracks.push_back(track);
        t_tile.GetNavigationNodes()[t_toNodeIndex]->autoTracks.push_back(track);
    }
}
