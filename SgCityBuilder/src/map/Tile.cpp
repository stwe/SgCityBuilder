#include "Tile.h"
#include "Map.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

sg::city::map::Tile::Tile(
    const float t_mapX,
    const float t_mapZ,
    const Type t_type
)
    : m_mapX{ t_mapX }
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

//-------------------------------------------------
// Setter
//-------------------------------------------------

void sg::city::map::Tile::SetParentMap(Map* t_map)
{
    m_map = t_map;
}

void sg::city::map::Tile::SetType(const Type t_type)
{
    m_type = t_type;

    m_vertices[6] = TYPE_COLOR[m_type].x;
    m_vertices[7] = TYPE_COLOR[m_type].y;
    m_vertices[8] = TYPE_COLOR[m_type].z;

    m_vertices[15] = TYPE_COLOR[m_type].x;
    m_vertices[16] = TYPE_COLOR[m_type].y;
    m_vertices[17] = TYPE_COLOR[m_type].z;

    m_vertices[24] = TYPE_COLOR[m_type].x;
    m_vertices[25] = TYPE_COLOR[m_type].y;
    m_vertices[26] = TYPE_COLOR[m_type].z;

    m_vertices[33] = TYPE_COLOR[m_type].x;
    m_vertices[34] = TYPE_COLOR[m_type].y;
    m_vertices[35] = TYPE_COLOR[m_type].z;

    m_vertices[42] = TYPE_COLOR[m_type].x;
    m_vertices[43] = TYPE_COLOR[m_type].y;
    m_vertices[44] = TYPE_COLOR[m_type].z;

    m_vertices[51] = TYPE_COLOR[m_type].x;
    m_vertices[52] = TYPE_COLOR[m_type].y;
    m_vertices[53] = TYPE_COLOR[m_type].z;
}

//-------------------------------------------------
// To string
//-------------------------------------------------

std::string sg::city::map::Tile::TileTypeToString(const Type t_type)
{
    switch (t_type)
    {
        default:
        case NONE: return "None";
        case RESIDENTIAL: return "Residential Zone";
        case COMMERCIAL: return "Commercial Zone";
        case INDUSTRIAL: return "Industrial Zone";
        case TRAFFIC_NETWORK: return "Road";
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

    // positions
    const auto bL{ glm::vec3(m_mapX, DEFAULT_HEIGHT, -m_mapZ) };
    const auto bR{ glm::vec3(m_mapX + 1.0f, DEFAULT_HEIGHT, -m_mapZ) };
    const auto tL{ glm::vec3(m_mapX, DEFAULT_HEIGHT, -(m_mapZ + 1.0f)) };
    const auto tR{ glm::vec3(m_mapX + 1.0f, DEFAULT_HEIGHT, -(m_mapZ + 1.0f)) };

    // the color of the type
    auto color{ TYPE_COLOR[m_type] };

    // red color for the first Tile
    if (m_mapX == 0.0f && m_mapZ == 0.0f)
    {
        color = glm::vec3(0.7f, 0.1f, 0.1f);
    }

    m_vertices = {
        // position        // normal                                              // color
        bL.x, bL.y, bL.z , DEFAULT_NORMAL.x, DEFAULT_NORMAL.y, DEFAULT_NORMAL.z , color.x, color.y, color.z, // first triangle
        bR.x, bR.y, bR.z , DEFAULT_NORMAL.x, DEFAULT_NORMAL.y, DEFAULT_NORMAL.z , color.x, color.y, color.z,
        tL.x, tL.y, tL.z , DEFAULT_NORMAL.x, DEFAULT_NORMAL.y, DEFAULT_NORMAL.z , color.x, color.y, color.z,

        tL.x, tL.y, tL.z , DEFAULT_NORMAL.x, DEFAULT_NORMAL.y, DEFAULT_NORMAL.z , color.x, color.y, color.z, // second triangle
        bR.x, bR.y, bR.z , DEFAULT_NORMAL.x, DEFAULT_NORMAL.y, DEFAULT_NORMAL.z , color.x, color.y, color.z,
        tR.x, tR.y, tR.z , DEFAULT_NORMAL.x, DEFAULT_NORMAL.y, DEFAULT_NORMAL.z , color.x, color.y, color.z,
    };
}
