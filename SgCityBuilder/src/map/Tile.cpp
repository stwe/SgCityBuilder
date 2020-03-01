#include "Tile.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

sg::city::map::Tile::Tile(const float t_mapX, const float t_mapZ, const Map::TileType t_type)
    : m_mapX{ t_mapX }
    , m_mapZ{ t_mapZ }
    , m_type{ t_type }
{
    Create();
}

sg::city::map::Tile::~Tile() noexcept
{
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

const sg::city::map::Tile::VertexContainer& sg::city::map::Tile::GetVerticesContainer() const noexcept
{
    return m_vertices;
}

sg::city::map::Tile::VertexContainer& sg::city::map::Tile::GetVerticesContainer() noexcept
{
    return m_vertices;
}

float sg::city::map::Tile::GetMapX() const
{
    return m_mapX;
}

float sg::city::map::Tile::GetMapZ() const
{
    return m_mapZ;
}

sg::city::map::Map::TileType sg::city::map::Tile::GetType() const
{
    return m_type;
}

const sg::city::map::Tile::NeighbourContainer& sg::city::map::Tile::GetNeighbours() const noexcept
{
    return m_neighbours;
}

sg::city::map::Tile::NeighbourContainer& sg::city::map::Tile::GetNeighbours() noexcept
{
    return m_neighbours;
}

//-------------------------------------------------
// Setter
//-------------------------------------------------

void sg::city::map::Tile::SetVertexPositions(const glm::vec3& t_bL, const glm::vec3& t_bR, const glm::vec3& t_tL, const glm::vec3& t_tR)
{
    m_vertices[0] = t_bL.x;
    m_vertices[1] = t_bL.y;
    m_vertices[2] = t_bL.z;

    m_vertices[12] = t_bR.x;
    m_vertices[13] = t_bR.y;
    m_vertices[14] = t_bR.z;

    m_vertices[24] = t_tL.x;
    m_vertices[25] = t_tL.y;
    m_vertices[26] = t_tL.z;

    m_vertices[36] = t_tL.x;
    m_vertices[37] = t_tL.y;
    m_vertices[38] = t_tL.z;

    m_vertices[48] = t_bR.x;
    m_vertices[49] = t_bR.y;
    m_vertices[50] = t_bR.z;

    m_vertices[60] = t_tR.x;
    m_vertices[61] = t_tR.y;
    m_vertices[62] = t_tR.z;
}

void sg::city::map::Tile::SetNormal(const glm::vec3& t_normal)
{
    m_vertices[3] = t_normal.x;
    m_vertices[4] = t_normal.y;
    m_vertices[5] = t_normal.z;

    m_vertices[15] = t_normal.x;
    m_vertices[16] = t_normal.y;
    m_vertices[17] = t_normal.z;

    m_vertices[27] = t_normal.x;
    m_vertices[28] = t_normal.y;
    m_vertices[29] = t_normal.z;

    m_vertices[39] = t_normal.x;
    m_vertices[40] = t_normal.y;
    m_vertices[41] = t_normal.z;

    m_vertices[51] = t_normal.x;
    m_vertices[52] = t_normal.y;
    m_vertices[53] = t_normal.z;

    m_vertices[63] = t_normal.x;
    m_vertices[64] = t_normal.y;
    m_vertices[65] = t_normal.z;
}

void sg::city::map::Tile::SetColor(const glm::vec3& t_color)
{
    m_vertices[6] = t_color.x;
    m_vertices[7] = t_color.y;
    m_vertices[8] = t_color.z;

    m_vertices[18] = t_color.x;
    m_vertices[19] = t_color.y;
    m_vertices[20] = t_color.z;

    m_vertices[30] = t_color.x;
    m_vertices[31] = t_color.y;
    m_vertices[32] = t_color.z;

    m_vertices[42] = t_color.x;
    m_vertices[43] = t_color.y;
    m_vertices[44] = t_color.z;

    m_vertices[54] = t_color.x;
    m_vertices[55] = t_color.y;
    m_vertices[56] = t_color.z;

    m_vertices[66] = t_color.x;
    m_vertices[67] = t_color.y;
    m_vertices[68] = t_color.z;
}

void sg::city::map::Tile::SetTexture(const float t_texture)
{
    m_vertices[9] = t_texture;
    m_vertices[21] = t_texture;
    m_vertices[33] = t_texture;
    m_vertices[45] = t_texture;
    m_vertices[57] = t_texture;
    m_vertices[69] = t_texture;
}

void sg::city::map::Tile::SetUv(const glm::vec2& t_bL, const glm::vec2& t_bR, const glm::vec2& t_tL, const glm::vec2& t_tR)
{
    m_vertices[10] = t_bL.x;
    m_vertices[11] = t_bL.y;

    m_vertices[22] = t_bR.x;
    m_vertices[23] = t_bR.y;

    m_vertices[34] = t_tL.x;
    m_vertices[35] = t_tL.y;

    m_vertices[46] = t_tL.x;
    m_vertices[47] = t_tL.y;

    m_vertices[58] = t_bR.x;
    m_vertices[59] = t_bR.y;

    m_vertices[70] = t_tR.x;
    m_vertices[71] = t_tR.y;
}

void sg::city::map::Tile::SetType(const Map::TileType t_type)
{
    m_type = t_type;

    // we use the TileType number as value for the texture
    SetTexture(static_cast<float>(static_cast<int>(m_type)));

    // we use the TileType color as Tile color
    SetColor(Map::TILE_TYPE_COLOR[static_cast<int>(m_type)]);
}

//-------------------------------------------------
// To string
//-------------------------------------------------

std::string sg::city::map::Tile::TileTypeToString(const Map::TileType t_type)
{
    switch (t_type)
    {
        default:
        case Map::TileType::NONE: return "None";
        case Map::TileType::RESIDENTIAL: return "Residential Zone";
        case Map::TileType::COMMERCIAL: return "Commercial Zone";
        case Map::TileType::INDUSTRIAL: return "Industrial Zone";
        case Map::TileType::TRAFFIC_NETWORK: return "Roads or rails";
    }
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void sg::city::map::Tile::Create()
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

    const auto bL{ glm::vec3(m_mapX, DEFAULT_HEIGHT, -m_mapZ) };
    const auto bR{ glm::vec3(m_mapX + 1.0f, DEFAULT_HEIGHT, -m_mapZ) };
    const auto tL{ glm::vec3(m_mapX, DEFAULT_HEIGHT, -(m_mapZ + 1.0f)) };
    const auto tR{ glm::vec3(m_mapX + 1.0f, DEFAULT_HEIGHT, -(m_mapZ + 1.0f)) };

    SetVertexPositions(bL, bR, tL, tR);
    SetNormal(DEFAULT_NORMAL);
    SetColor(Map::TILE_TYPE_COLOR[static_cast<int>(m_type)]);
    SetTexture(static_cast<float>(static_cast<int>(m_type)));
    SetUv();
}
