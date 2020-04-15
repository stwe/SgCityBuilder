// This file is part of the SgCityBuilder package.
// 
// Filename: RoadNetwork.cpp
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#include <Log.h>
#include <Core.h>
#include <resource/Mesh.h>
#include "RoadNetwork.h"
#include "Map.h"
#include "city/City.h"

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
    return m_city->GetMap().GetRoadTextureAtlasId();
}

//-------------------------------------------------
// Create
//-------------------------------------------------

void sg::city::map::RoadNetwork::CreateRoadNetworkMesh() const
{
    VertexContainer roadNetworkVertices;

    for (auto& tile : m_city->GetMap().GetTiles())
    {
        if (tile->type == tile::TileType::TRAFFIC)
        {
            // !! copy !! Tile vertices
            auto vertices{ tile->GetVertices() };

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

            auto* roadTile{ dynamic_cast<tile::RoadTile*>(tile.get()) };

            const auto roadType{ static_cast<int>(roadTile->roadType) };

            const auto column{ roadType % static_cast<int>(TEXTURE_ATLAS_ROWS) };
            const auto xOffset{ static_cast<float>(column) / TEXTURE_ATLAS_ROWS };

            const auto row{ roadType / static_cast<int>(TEXTURE_ATLAS_ROWS) };
            const auto yOffset{ 1.0f - static_cast<float>(row) / TEXTURE_ATLAS_ROWS };

            // bl
            vertices[tile::Tile::BOTTOM_LEFT_TEXTURE_X_T1] = (0.0f / TEXTURE_ATLAS_ROWS) + xOffset;
            vertices[tile::Tile::BOTTOM_LEFT_TEXTURE_Y_T1] = (0.0f / TEXTURE_ATLAS_ROWS) + yOffset;

            // br
            vertices[tile::Tile::BOTTOM_RIGHT_TEXTURE_X_T1] = (1.0f / TEXTURE_ATLAS_ROWS) + xOffset;
            vertices[tile::Tile::BOTTOM_RIGHT_TEXTURE_Y_T1] = (0.0f / TEXTURE_ATLAS_ROWS) + yOffset;

            // tl
            vertices[tile::Tile::TOP_LEFT_TEXTURE_X_T1] = (0.0f / TEXTURE_ATLAS_ROWS) + xOffset;
            vertices[tile::Tile::TOP_LEFT_TEXTURE_Y_T1] = (1.0f / TEXTURE_ATLAS_ROWS) + yOffset;

            // tl
            vertices[tile::Tile::TOP_LEFT_TEXTURE_X_T2] = (0.0f / TEXTURE_ATLAS_ROWS) + xOffset;
            vertices[tile::Tile::TOP_LEFT_TEXTURE_Y_T2] = (1.0f / TEXTURE_ATLAS_ROWS) + yOffset;

            // br
            vertices[tile::Tile::BOTTOM_RIGHT_TEXTURE_X_T2] = (1.0f / TEXTURE_ATLAS_ROWS) + xOffset;
            vertices[tile::Tile::BOTTOM_RIGHT_TEXTURE_Y_T2] = (0.0f / TEXTURE_ATLAS_ROWS) + yOffset;

            // tr
            vertices[tile::Tile::TOP_RIGHT_TEXTURE_X_T2] = (1.0f / TEXTURE_ATLAS_ROWS) + xOffset;
            vertices[tile::Tile::TOP_RIGHT_TEXTURE_Y_T2] = (1.0f / TEXTURE_ATLAS_ROWS) + yOffset;

            // insert the RoadTile vertices at the end of the container with all vertices
            roadNetworkVertices.insert(roadNetworkVertices.end(), vertices.begin(), vertices.end());
        }
    }

    // calculate the number of RoadTiles
    const auto nrTiles{ static_cast<int>(roadNetworkVertices.size()) / tile::Tile::FLOATS_PER_TILE };

    if (nrTiles > 0)
    {
        // update draw count
        m_roadNetworkMesh->GetVao().SetDrawCount(nrTiles * tile::Tile::VERTICES_PER_TILE);

        // update Vbo
        ogl::buffer::Vbo::BindVbo(m_vboId);
        glBufferSubData(GL_ARRAY_BUFFER, 0, nrTiles * tile::Tile::SIZE_IN_BYTES_PER_TILE, roadNetworkVertices.data());
        ogl::buffer::Vbo::UnbindVbo();
    }
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

    // create an bind a Vao
    m_roadNetworkMesh = std::make_unique<ogl::resource::Mesh>();
    m_roadNetworkMesh->GetVao().BindVao();

    // create Vbo
    CreateVbo();

    // unbind Vao
    ogl::buffer::Vao::UnbindVao();
}
