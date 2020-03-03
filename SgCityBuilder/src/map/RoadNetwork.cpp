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

uint32_t sg::city::map::RoadNetwork::GetNsoTextureId() const
{
    return m_nsoTextureId;
}

uint32_t sg::city::map::RoadNetwork::GetNswTextureId() const
{
    return m_nswTextureId;
}

uint32_t sg::city::map::RoadNetwork::GetAllTextureId() const
{
    return m_allTextureId;
}

//-------------------------------------------------
// Create
//-------------------------------------------------

void sg::city::map::RoadNetwork::StoreRoadOnPosition(const glm::vec3& t_mapPoint)
{
    // get Tile index
    const auto tileIndex{ m_map->GetTileIndexByPosition(t_mapPoint) };

    // check if a Road already exists
    if (m_lookupTable[tileIndex] > 0)
    {
        return;
    }

    SG_OGL_LOG_INFO("Add a Road to the RoadNetwork");

    // get Tile
    auto& tile{ m_map->GetTileByIndex(tileIndex) };

    // get vertices of the Tile (make a copy)
    auto vertices{ tile.GetVerticesContainer() };

    // get neighbours of the Tile
    const auto& neighbours{ tile.GetNeighbours() };

    // we use the same vertices of the tiles, but just a little bit higher
    vertices[1] = 0.001f;
    vertices[13] = 0.001f;
    vertices[25] = 0.001f;

    vertices[37] = 0.001f;
    vertices[49] = 0.001f;
    vertices[61] = 0.001f;

    // set the default texture
    auto texture{ Texture::WO };

    uint8_t roadNeighbours{ 0 };

    if (neighbours[static_cast<int>(Tile::Directions::NORTH)])
    {
        if (neighbours[static_cast<int>(Tile::Directions::NORTH)]->GetType() == Map::TileType::TRAFFIC_NETWORK)
        {
            roadNeighbours = NORTH;
            SG_OGL_LOG_DEBUG("Nachbar im Norden +1 = {}", roadNeighbours);
        }
    }

    if (neighbours[static_cast<int>(Tile::Directions::EAST)])
    {
        if (neighbours[static_cast<int>(Tile::Directions::EAST)]->GetType() == Map::TileType::TRAFFIC_NETWORK)
        {
            roadNeighbours |= EAST;
            SG_OGL_LOG_DEBUG("Nachbar im Osten +2 = {}", roadNeighbours);
        }
    }

    if (neighbours[static_cast<int>(Tile::Directions::SOUTH)])
    {
        if (neighbours[static_cast<int>(Tile::Directions::SOUTH)]->GetType() == Map::TileType::TRAFFIC_NETWORK)
        {
            roadNeighbours |= SOUTH;
            SG_OGL_LOG_DEBUG("Nachbar im Sueden +4 = {}", roadNeighbours);
        }
    }

    if (neighbours[static_cast<int>(Tile::Directions::WEST)])
    {
        if (neighbours[static_cast<int>(Tile::Directions::WEST)]->GetType() == Map::TileType::TRAFFIC_NETWORK)
        {
            roadNeighbours |= WEST;
            SG_OGL_LOG_DEBUG("Nachbar im Westen +8 = {}", roadNeighbours);
        }
    }

    SG_OGL_LOG_DEBUG("Nachbarn gesamt {}", roadNeighbours);

    switch (roadNeighbours)
    {
    case 0: texture = Texture::WO;
            break;
    case 1: texture = Texture::NS;
            break;
    case 2: texture = Texture::WO;
            break;
    case 3: texture = Texture::NSO;
            break;
    case 4: texture = Texture::NS;
            break;
    case 5: texture = Texture::NS;
            break;
    case 6: texture = Texture::NSO;
            break;
    case 7: texture = Texture::NSO;
            break;
    case 8: texture = Texture::WO;
            break;
    case 9: texture = Texture::NSW;
            break;
    case 10: texture = Texture::WO;
            break;
    case 11: texture = Texture::ALL;
             break;
    case 12: texture = Texture::NSW;
             break;
    case 13: texture = Texture::NSW;
             break;
    case 14: texture = Texture::ALL;
             break;
    case 15: texture = Texture::ALL;
             break;
    default: texture = Texture::WO;
    }

    /*
    if (neighbours[static_cast<int>(Tile::Directions::NORTH)])
    {
        if (neighbours[static_cast<int>(Tile::Directions::NORTH)]->GetType() == Map::TileType::TRAFFIC_NETWORK)
        {
            const auto w{ neighbours[static_cast<int>(Tile::Directions::NORTH)]->GetNeighbours()[static_cast<int>(Tile::Directions::WEST)]->GetType() == Map::TileType::TRAFFIC_NETWORK };
            const auto e{ neighbours[static_cast<int>(Tile::Directions::NORTH)]->GetNeighbours()[static_cast<int>(Tile::Directions::EAST)]->GetType() == Map::TileType::TRAFFIC_NETWORK };

            if (!w && !e)
            {
                const auto northTileIndex{ m_map->GetTileIndexByPosition(
                    static_cast<int>(neighbours[static_cast<int>(Tile::Directions::NORTH)]->GetMapX()),
                    static_cast<int>(neighbours[static_cast<int>(Tile::Directions::NORTH)]->GetMapZ()))
                };

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

            texture = Texture::NS;
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


            texture = Texture::NS;
        }
    }
    */

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

    vertices[9] = static_cast<float>(texture);
    vertices[21] = static_cast<float>(texture);;
    vertices[33] = static_cast<float>(texture);;
    vertices[45] = static_cast<float>(texture);;
    vertices[57] = static_cast<float>(texture);;
    vertices[69] = static_cast<float>(texture);;

    // insert vertices at the end
    m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());

    const auto nrTiles{ static_cast<int>(m_vertices.size()) / Tile::FLOATS_PER_TILE };

    // stores where the road is in the street network container
    m_lookupTable[tileIndex] = nrTiles;

    ogl::buffer::Vbo::BindVbo(m_vboId);
    glBufferSubData(GL_ARRAY_BUFFER, 0, nrTiles * Tile::SIZE_IN_BYTES_PER_TILE, m_vertices.data());
    ogl::buffer::Vbo::UnbindVbo();

    m_roadNetworkMesh->GetVao().SetDrawCount(nrTiles * Tile::VERTICES_PER_TILE);
}

void sg::city::map::RoadNetwork::RemoveRoadFromVbo(int t_tileIndex)
{
    // update vertex count
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
    m_nsoTextureId = m_map->GetScene()->GetApplicationContext()->GetTextureManager().GetTextureIdFromPath("res/texture/road/nso.png");
    m_nswTextureId = m_map->GetScene()->GetApplicationContext()->GetTextureManager().GetTextureIdFromPath("res/texture/road/nsw.png");
    m_allTextureId = m_map->GetScene()->GetApplicationContext()->GetTextureManager().GetTextureIdFromPath("res/texture/road/all.png");

    // setup lookup table
    m_lookupTable.resize(m_map->GetMapSize() * m_map->GetMapSize(), -1);
}
