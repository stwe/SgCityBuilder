#include "Tile.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

sg::city::map::Tile::Tile(
    Map* t_map,
    const float t_mapX,
    const float t_mapZ,
    const Map::TileType t_type
)
    : m_map{ t_map }
    , m_mapX{ t_mapX }
    , m_mapZ{ t_mapZ }
    , m_type{ t_type }
{
    Init();
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

//-------------------------------------------------
// Setter
//-------------------------------------------------

void sg::city::map::Tile::ChangeTypeTo(const Map::TileType t_type)
{
    m_type = t_type;

    //m_vertices[6] = Map::TILE_TYPE_COLOR[static_cast<int>(m_type)].x;
    //m_vertices[7] = Map::TILE_TYPE_COLOR[static_cast<int>(m_type)].y;
    //m_vertices[8] = Map::TILE_TYPE_COLOR[static_cast<int>(m_type)].z;
    m_vertices[9] = static_cast<float>(static_cast<int>(m_type));

    //m_vertices[18] = Map::TILE_TYPE_COLOR[static_cast<int>(m_type)].x;
    //m_vertices[19] = Map::TILE_TYPE_COLOR[static_cast<int>(m_type)].y;
    //m_vertices[20] = Map::TILE_TYPE_COLOR[static_cast<int>(m_type)].z;
    m_vertices[21] = static_cast<float>(static_cast<int>(m_type));

    //m_vertices[30] = Map::TILE_TYPE_COLOR[static_cast<int>(m_type)].x;
    //m_vertices[31] = Map::TILE_TYPE_COLOR[static_cast<int>(m_type)].y;
    //m_vertices[32] = Map::TILE_TYPE_COLOR[static_cast<int>(m_type)].z;
    m_vertices[33] = static_cast<float>(static_cast<int>(m_type));

    //m_vertices[42] = Map::TILE_TYPE_COLOR[static_cast<int>(m_type)].x;
    //m_vertices[43] = Map::TILE_TYPE_COLOR[static_cast<int>(m_type)].y;
    //m_vertices[44] = Map::TILE_TYPE_COLOR[static_cast<int>(m_type)].z;
    m_vertices[45] = static_cast<float>(static_cast<int>(m_type));

    //m_vertices[54] = Map::TILE_TYPE_COLOR[static_cast<int>(m_type)].x;
    //m_vertices[55] = Map::TILE_TYPE_COLOR[static_cast<int>(m_type)].y;
    //m_vertices[56] = Map::TILE_TYPE_COLOR[static_cast<int>(m_type)].z;
    m_vertices[57] = static_cast<float>(static_cast<int>(m_type));

    //m_vertices[66] = Map::TILE_TYPE_COLOR[static_cast<int>(m_type)].x;
    //m_vertices[67] = Map::TILE_TYPE_COLOR[static_cast<int>(m_type)].y;
    //m_vertices[68] = Map::TILE_TYPE_COLOR[static_cast<int>(m_type)].z;
    m_vertices[69] = static_cast<float>(static_cast<int>(m_type));
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

void sg::city::map::Tile::Init()
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

    // todo: stbi_set_flip_vertically_on_load(true);

    // positions
    const auto bL{ glm::vec3(m_mapX, DEFAULT_HEIGHT, -m_mapZ) };
    const auto bR{ glm::vec3(m_mapX + 1.0f, DEFAULT_HEIGHT, -m_mapZ) };
    const auto tL{ glm::vec3(m_mapX, DEFAULT_HEIGHT, -(m_mapZ + 1.0f)) };
    const auto tR{ glm::vec3(m_mapX + 1.0f, DEFAULT_HEIGHT, -(m_mapZ + 1.0f)) };

    // the color of the Tile by type
    const auto color{ Map::TILE_TYPE_COLOR[static_cast<int>(m_type)] };

    // the texture index value in the array of tile textures (m_map->GetTileTypeTextures()) is the sames as m_type
    const auto textureIndex{ static_cast<float>(static_cast<int>(m_type)) };

    m_vertices = {
        // position        // normal                                              // color                   // texture    // uv
        bL.x, bL.y, bL.z , DEFAULT_NORMAL.x, DEFAULT_NORMAL.y, DEFAULT_NORMAL.z , color.x, color.y, color.z, textureIndex, 0.0f, 0.0f, // first triangle
        bR.x, bR.y, bR.z , DEFAULT_NORMAL.x, DEFAULT_NORMAL.y, DEFAULT_NORMAL.z , color.x, color.y, color.z, textureIndex, 1.0f, 0.0f,
        tL.x, tL.y, tL.z , DEFAULT_NORMAL.x, DEFAULT_NORMAL.y, DEFAULT_NORMAL.z , color.x, color.y, color.z, textureIndex, 0.0f, 1.0f,

        tL.x, tL.y, tL.z , DEFAULT_NORMAL.x, DEFAULT_NORMAL.y, DEFAULT_NORMAL.z , color.x, color.y, color.z, textureIndex, 0.0f, 1.0f, // second triangle
        bR.x, bR.y, bR.z , DEFAULT_NORMAL.x, DEFAULT_NORMAL.y, DEFAULT_NORMAL.z , color.x, color.y, color.z, textureIndex, 1.0f, 0.0f,
        tR.x, tR.y, tR.z , DEFAULT_NORMAL.x, DEFAULT_NORMAL.y, DEFAULT_NORMAL.z , color.x, color.y, color.z, textureIndex, 1.0f, 1.0f
    };
}
