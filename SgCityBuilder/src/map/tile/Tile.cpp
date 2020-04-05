// This file is part of the SgCityBuilder package.
// 
// Filename: Tile.cpp
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#include <Log.h>
#include "Tile.h"
#include "map/Map.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

sg::city::map::tile::Tile::Tile(const float t_mapX, const float t_mapZ, const TileType t_type, Map* t_map)
    : type{ t_type }
    , m_map{ t_map }
    , m_mapX{ t_mapX }
    , m_mapZ{ t_mapZ }
{
    Create();
}

sg::city::map::tile::Tile::~Tile() noexcept
{
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

int sg::city::map::tile::Tile::GetMapX() const
{
    return static_cast<int>(m_mapX);
}

int sg::city::map::tile::Tile::GetMapZ() const
{
    return static_cast<int>(m_mapZ);
}

float sg::city::map::tile::Tile::GetWorldX() const
{
    return m_bottomLeft.x;
}

float sg::city::map::tile::Tile::GetWorldZ() const
{
    return m_bottomLeft.z;
}

glm::vec3 sg::city::map::tile::Tile::GetWorldPosition() const
{
    return m_bottomLeft;
}

glm::vec3 sg::city::map::tile::Tile::GetWorldCenter() const
{
    return glm::vec3(
        (m_bottomLeft.x + m_bottomRight.x) * 0.5f,
        0.0f,
        (m_bottomLeft.z + m_topLeft.z) * 0.5f
    );
}

const sg::city::map::tile::Tile::VertexContainer& sg::city::map::tile::Tile::GetVertices() const noexcept
{
    return m_vertices;
}

sg::city::map::tile::Tile::VertexContainer& sg::city::map::tile::Tile::GetVertices() noexcept
{
    return m_vertices;
}

const sg::city::map::tile::Tile::NeighbourContainer& sg::city::map::tile::Tile::GetNeighbours() const noexcept
{
    return m_neighbours;
}

sg::city::map::tile::Tile::NeighbourContainer& sg::city::map::tile::Tile::GetNeighbours() noexcept
{
    return m_neighbours;
}

//-------------------------------------------------
// Setter
//-------------------------------------------------

void sg::city::map::tile::Tile::SetVertexPositions()
{
    m_vertices[BOTTOM_LEFT_POS_X_T1] = m_bottomLeft.x;
    m_vertices[BOTTOM_LEFT_POS_Y_T1] = m_bottomLeft.y;
    m_vertices[BOTTOM_LEFT_POS_Z_T1] = m_bottomLeft.z;

    m_vertices[BOTTOM_RIGHT_POS_X_T1] = m_bottomRight.x;
    m_vertices[BOTTOM_RIGHT_POS_Y_T1] = m_bottomRight.y;
    m_vertices[BOTTOM_RIGHT_POS_Z_T1] = m_bottomRight.z;

    m_vertices[TOP_LEFT_POS_X_T1] = m_topLeft.x;
    m_vertices[TOP_LEFT_POS_Y_T1] = m_topLeft.y;
    m_vertices[TOP_LEFT_POS_Z_T1] = m_topLeft.z;

    m_vertices[TOP_LEFT_POS_X_T2] = m_topLeft.x;
    m_vertices[TOP_LEFT_POS_Y_T2] = m_topLeft.y;
    m_vertices[TOP_LEFT_POS_Z_T2] = m_topLeft.z;

    m_vertices[BOTTOM_RIGHT_POS_X_T2] = m_bottomRight.x;
    m_vertices[BOTTOM_RIGHT_POS_Y_T2] = m_bottomRight.y;
    m_vertices[BOTTOM_RIGHT_POS_Z_T2] = m_bottomRight.z;

    m_vertices[TOP_RIGHT_POS_X_T2] = m_topRight.x;
    m_vertices[TOP_RIGHT_POS_Y_T2] = m_topRight.y;
    m_vertices[TOP_RIGHT_POS_Z_T2] = m_topRight.z;
}

void sg::city::map::tile::Tile::SetNormal(const glm::vec3& t_normal)
{
    m_vertices[BOTTOM_LEFT_NORMAL_X_T1] = t_normal.x;
    m_vertices[BOTTOM_LEFT_NORMAL_Y_T1] = t_normal.y;
    m_vertices[BOTTOM_LEFT_NORMAL_Z_T1] = t_normal.z;

    m_vertices[BOTTOM_RIGHT_NORMAL_X_T1] = t_normal.x;
    m_vertices[BOTTOM_RIGHT_NORMAL_Y_T1] = t_normal.y;
    m_vertices[BOTTOM_RIGHT_NORMAL_Z_T1] = t_normal.z;

    m_vertices[TOP_LEFT_NORMAL_X_T1] = t_normal.x;
    m_vertices[TOP_LEFT_NORMAL_Y_T1] = t_normal.y;
    m_vertices[TOP_LEFT_NORMAL_Z_T1] = t_normal.z;

    m_vertices[TOP_LEFT_NORMAL_X_T2] = t_normal.x;
    m_vertices[TOP_LEFT_NORMAL_Y_T2] = t_normal.y;
    m_vertices[TOP_LEFT_NORMAL_Z_T2] = t_normal.z;

    m_vertices[BOTTOM_RIGHT_NORMAL_X_T2] = t_normal.x;
    m_vertices[BOTTOM_RIGHT_NORMAL_Y_T2] = t_normal.y;
    m_vertices[BOTTOM_RIGHT_NORMAL_Z_T2] = t_normal.z;

    m_vertices[TOP_RIGHT_NORMAL_X_T2] = t_normal.x;
    m_vertices[TOP_RIGHT_NORMAL_Y_T2] = t_normal.y;
    m_vertices[TOP_RIGHT_NORMAL_Z_T2] = t_normal.z;
}

void sg::city::map::tile::Tile::SetColor(const glm::vec3& t_color)
{
    m_vertices[BOTTOM_LEFT_COLOR_X_T1] = t_color.x;
    m_vertices[BOTTOM_LEFT_COLOR_Y_T1] = t_color.y;
    m_vertices[BOTTOM_LEFT_COLOR_Z_T1] = t_color.z;

    m_vertices[BOTTOM_RIGHT_COLOR_X_T1] = t_color.x;
    m_vertices[BOTTOM_RIGHT_COLOR_Y_T1] = t_color.y;
    m_vertices[BOTTOM_RIGHT_COLOR_Z_T1] = t_color.z;

    m_vertices[TOP_LEFT_COLOR_X_T1] = t_color.x;
    m_vertices[TOP_LEFT_COLOR_Y_T1] = t_color.y;
    m_vertices[TOP_LEFT_COLOR_Z_T1] = t_color.z;

    m_vertices[TOP_LEFT_COLOR_X_T2] = t_color.x;
    m_vertices[TOP_LEFT_COLOR_Y_T2] = t_color.y;
    m_vertices[TOP_LEFT_COLOR_Z_T2] = t_color.z;

    m_vertices[BOTTOM_RIGHT_COLOR_X_T2] = t_color.x;
    m_vertices[BOTTOM_RIGHT_COLOR_Y_T2] = t_color.y;
    m_vertices[BOTTOM_RIGHT_COLOR_Z_T2] = t_color.z;

    m_vertices[TOP_RIGHT_COLOR_X_T2] = t_color.x;
    m_vertices[TOP_RIGHT_COLOR_Y_T2] = t_color.y;
    m_vertices[TOP_RIGHT_COLOR_Z_T2] = t_color.z;
}

void sg::city::map::tile::Tile::SetTexture(const float t_texture)
{
    m_vertices[BOTTOM_LEFT_TEXTURE_NR_T1] = t_texture;
    m_vertices[BOTTOM_RIGHT_TEXTURE_NR_T1] = t_texture;
    m_vertices[TOP_LEFT_TEXTURE_NR_T1] = t_texture;
    m_vertices[TOP_LEFT_TEXTURE_NR_T2] = t_texture;
    m_vertices[BOTTOM_RIGHT_TEXTURE_NR_T2] = t_texture;
    m_vertices[TOP_RIGHT_TEXTURE_NR_T2] = t_texture;
}

void sg::city::map::tile::Tile::SetUv(const glm::vec2& t_bL, const glm::vec2& t_bR, const glm::vec2& t_tL, const glm::vec2& t_tR)
{
    m_vertices[BOTTOM_LEFT_TEXTURE_X_T1] = t_bL.x;
    m_vertices[BOTTOM_LEFT_TEXTURE_Y_T1] = t_bL.y;

    m_vertices[BOTTOM_RIGHT_TEXTURE_X_T1] = t_bR.x;
    m_vertices[BOTTOM_RIGHT_TEXTURE_Y_T1] = t_bR.y;

    m_vertices[TOP_LEFT_TEXTURE_X_T1] = t_tL.x;
    m_vertices[TOP_LEFT_TEXTURE_Y_T1] = t_tL.y;

    m_vertices[TOP_LEFT_TEXTURE_X_T2] = t_tL.x;
    m_vertices[TOP_LEFT_TEXTURE_Y_T2] = t_tL.y;

    m_vertices[BOTTOM_RIGHT_TEXTURE_X_T2] = t_bR.x;
    m_vertices[BOTTOM_RIGHT_TEXTURE_Y_T2] = t_bR.y;

    m_vertices[TOP_RIGHT_TEXTURE_X_T2] = t_tR.x;
    m_vertices[TOP_RIGHT_TEXTURE_Y_T2] = t_tR.y;
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void sg::city::map::tile::Tile::Update()
{
    SG_OGL_LOG_INFO("[Tile::Update()] Start Tile update process.");

    // if the type has changed, the color and texture must also be changed

    // we use the TileType number as value for the texture
    SetTexture(static_cast<float>(type));

    // we use the TileType color as Tile color
    SetColor(TILE_TYPE_COLOR.at(type));

    // update Vbo
    m_map->UpdateMapVboByTileIndex(m_map->GetTileMapIndexByMapPosition(static_cast<int>(m_mapX), static_cast<int>(m_mapZ)));
}

//-------------------------------------------------
// To string
//-------------------------------------------------

std::string sg::city::map::tile::Tile::TileTypeToString(const TileType t_type)
{
    switch (t_type)
    {
    default:
    case TileType::NONE: return "None";
    case TileType::RESIDENTIAL: return "Residential Zone";
    case TileType::COMMERCIAL: return "Commercial Zone";
    case TileType::INDUSTRIAL: return "Industrial Zone";
    case TileType::TRAFFIC: return "Roads or rails";
    }
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void sg::city::map::tile::Tile::Create()
{
    /*
        tL       tR
        +--------+
        |  +   2 |
        |    +   |
        | 1    + |
        +--------+
        bL       bR
    */

    m_vertices.resize(FLOATS_PER_TILE);

    m_bottomLeft = glm::vec3(m_mapX, DEFAULT_HEIGHT, -m_mapZ);
    m_bottomRight = glm::vec3(m_mapX + 1.0f, DEFAULT_HEIGHT, -m_mapZ);
    m_topLeft = glm::vec3(m_mapX, DEFAULT_HEIGHT, -(m_mapZ + 1.0f));
    m_topRight = glm::vec3(m_mapX + 1.0f, DEFAULT_HEIGHT, -(m_mapZ + 1.0f));

    SetVertexPositions();
    SetNormal(DEFAULT_NORMAL);
    SetColor(TILE_TYPE_COLOR.at(type));
    SetTexture(static_cast<float>(type));
    SetUv();
}
