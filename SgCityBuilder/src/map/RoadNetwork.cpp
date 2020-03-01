#include <Application.h>
#include <scene/Scene.h>
#include <resource/Mesh.h>
#include <resource/TextureManager.h>
#include "RoadNetwork.h"
#include "Map.h"
#include "Tile.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

sg::city::map::RoadNetwork::RoadNetwork(Map* t_map)
    : m_map{ t_map }
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

uint32_t sg::city::map::RoadNetwork::GetWoTextureId() const
{
    return m_woTextureId;
}

uint32_t sg::city::map::RoadNetwork::GetNsTextureId() const
{
    return m_nsTextureId;
}

uint32_t sg::city::map::RoadNetwork::GetAllTextureId() const
{
    return m_allTextureId;
}

//-------------------------------------------------
// Create
//-------------------------------------------------

void sg::city::map::RoadNetwork::StoreRoad(const int t_tileIndex)
{
    // get vertices of the Road
    auto& vertices{ m_map->GetTiles()[t_tileIndex]->GetVerticesContainer() };

    // get neighbours of the Road
    const auto& neighbours{ m_map->GetTiles()[t_tileIndex]->GetNeighbours() };

    // todo: y-position
    vertices[1] = 0.001f;
    vertices[13] = 0.001f;
    vertices[25] = 0.001f;

    vertices[37] = 0.001f;
    vertices[49] = 0.001f;
    vertices[61] = 0.001f;

    /*

    __
    
    |
    __

    */


    // todo: orientation
    auto textureId{ 0 };
    if (neighbours[static_cast<int>(Tile::Directions::NORTH)])
    {
        if (neighbours[static_cast<int>(Tile::Directions::NORTH)]->GetType() == Map::TileType::TRAFFIC_NETWORK)
        {
            const auto w{ neighbours[static_cast<int>(Tile::Directions::NORTH)]->GetNeighbours()[static_cast<int>(Tile::Directions::WEST)]->GetType() == Map::TileType::TRAFFIC_NETWORK };
            const auto e{ neighbours[static_cast<int>(Tile::Directions::NORTH)]->GetNeighbours()[static_cast<int>(Tile::Directions::EAST)]->GetType() == Map::TileType::TRAFFIC_NETWORK };

            if (!w && !e)
            {
                // get tile index from North
                const auto tileIndex{ static_cast<int>(neighbours[static_cast<int>(Tile::Directions::NORTH)]->GetMapZ())* m_map->GetMapSize() + static_cast<int>(neighbours[static_cast<int>(Tile::Directions::NORTH)]->GetMapX()) };
                auto index{ m_lookupTable[tileIndex] - 1 };

                auto offset{ index * Tile::FLOATS_PER_TILE };

                m_vertices[offset + 9] = 1;
                m_vertices[offset + 21] = 1;
                m_vertices[offset + 33] = 1;
                m_vertices[offset + 45] = 1;
                m_vertices[offset + 57] = 1;
                m_vertices[offset + 69] = 1;

                // change neighbour to ns
                SG_OGL_LOG_DEBUG("North keine Nachbarn");
            }

            textureId = 1;
        }
    }

    if (neighbours[static_cast<int>(Tile::Directions::SOUTH)])
    {
        if (neighbours[static_cast<int>(Tile::Directions::SOUTH)]->GetType() == Map::TileType::TRAFFIC_NETWORK)
        {
            const auto w{ neighbours[static_cast<int>(Tile::Directions::SOUTH)]->GetNeighbours()[static_cast<int>(Tile::Directions::WEST)]->GetType() == Map::TileType::TRAFFIC_NETWORK };
            const auto e{ neighbours[static_cast<int>(Tile::Directions::SOUTH)]->GetNeighbours()[static_cast<int>(Tile::Directions::EAST)]->GetType() == Map::TileType::TRAFFIC_NETWORK };

            if (!w && !e)
            {
                // get tile index from South
                const auto tileIndex{ static_cast<int>(neighbours[static_cast<int>(Tile::Directions::SOUTH)]->GetMapZ())* m_map->GetMapSize() + static_cast<int>(neighbours[static_cast<int>(Tile::Directions::NORTH)]->GetMapX()) };
                auto index{ m_lookupTable[tileIndex] - 1 };

                auto offset{ index * Tile::FLOATS_PER_TILE };

                m_vertices[offset + 9] = 1;
                m_vertices[offset + 21] = 1;
                m_vertices[offset + 33] = 1;
                m_vertices[offset + 45] = 1;
                m_vertices[offset + 57] = 1;
                m_vertices[offset + 69] = 1;

                // change neighbour to ns
                SG_OGL_LOG_DEBUG("South keine Nachbarn");
            }


            textureId = 1;
        }
    }

    /*
    if (neighbours[static_cast<int>(Tile::Directions::NORTH)] &&
        neighbours[static_cast<int>(Tile::Directions::SOUTH)] &&
        neighbours[static_cast<int>(Tile::Directions::WEST)] &&
        neighbours[static_cast<int>(Tile::Directions::EAST)]
        )
    {
        if (neighbours[static_cast<int>(Tile::Directions::NORTH)]->GetType() == Map::TileType::TRAFFIC_NETWORK &&
            neighbours[static_cast<int>(Tile::Directions::SOUTH)]->GetType() == Map::TileType::TRAFFIC_NETWORK &&
            neighbours[static_cast<int>(Tile::Directions::WEST)]->GetType() == Map::TileType::TRAFFIC_NETWORK &&
            neighbours[static_cast<int>(Tile::Directions::EAST)]->GetType() == Map::TileType::TRAFFIC_NETWORK
            )
        {
            textureId = 2;
        }
    }
    */

    vertices[9] = static_cast<float>(textureId);
    vertices[21] = static_cast<float>(textureId);;
    vertices[33] = static_cast<float>(textureId);;
    vertices[45] = static_cast<float>(textureId);;
    vertices[57] = static_cast<float>(textureId);;
    vertices[69] = static_cast<float>(textureId);;

    // insert vertices at the end
    m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());

    const auto nrTiles{ static_cast<int>(m_vertices.size()) / Tile::FLOATS_PER_TILE };

    // stores where the road is in the street network container
    m_lookupTable[t_tileIndex] = nrTiles;

    ogl::buffer::Vbo::BindVbo(m_vboId);
    glBufferSubData(GL_ARRAY_BUFFER, 0, nrTiles * Tile::SIZE_IN_BYTES_PER_TILE, m_vertices.data());
    ogl::buffer::Vbo::UnbindVbo();

    m_roadNetworkMesh->GetVao().SetDrawCount(nrTiles * Tile::VERTICES_PER_TILE);
}

void sg::city::map::RoadNetwork::RemoveRoadFromVbo(int t_tileIndex)
{
    // update vertex count
}

void sg::city::map::RoadNetwork::Init()
{
    // create an bind a Vao
    m_roadNetworkMesh = std::make_unique<ogl::resource::Mesh>();
    m_roadNetworkMesh->GetVao().BindVao();

    // create Vbo
    CreateVbo();

    // unbind Vao
    ogl::buffer::Vao::UnbindVao();

    // load textures
    m_woTextureId = m_map->GetScene()->GetApplicationContext()->GetTextureManager().GetTextureIdFromPath("res/texture/road/wo.png");
    m_nsTextureId = m_map->GetScene()->GetApplicationContext()->GetTextureManager().GetTextureIdFromPath("res/texture/road/ns.png");
    m_allTextureId = m_map->GetScene()->GetApplicationContext()->GetTextureManager().GetTextureIdFromPath("res/texture/road/all.png");

    // setup lookup table
    m_lookupTable.resize(m_map->GetMapSize() * m_map->GetMapSize(), -1);
}

void sg::city::map::RoadNetwork::CreateVbo()
{
    m_vboId = ogl::buffer::Vbo::GenerateVbo();

    ogl::buffer::Vbo::InitEmpty(m_vboId, m_map->GetFloatCountOfMap(), GL_DYNAMIC_DRAW);

    ogl::buffer::Vbo::AddAttribute(m_vboId, 0, 3, Tile::FLOATS_PER_VERTEX, 0);  // 3x position
    ogl::buffer::Vbo::AddAttribute(m_vboId, 1, 3, Tile::FLOATS_PER_VERTEX, 3);  // 3x normal
    ogl::buffer::Vbo::AddAttribute(m_vboId, 2, 3, Tile::FLOATS_PER_VERTEX, 6);  // 3x color
    ogl::buffer::Vbo::AddAttribute(m_vboId, 3, 1, Tile::FLOATS_PER_VERTEX, 9);  // 1x texture
    ogl::buffer::Vbo::AddAttribute(m_vboId, 4, 2, Tile::FLOATS_PER_VERTEX, 10); // 2x uv
}
