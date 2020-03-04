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

uint32_t sg::city::map::RoadNetwork::GetTextureId(const Texture t_texture) const
{
    return m_textures.at(t_texture);
}

//-------------------------------------------------
// Add Road
//-------------------------------------------------

void sg::city::map::RoadNetwork::StoreRoadOnPosition(const glm::vec3& t_mapPoint)
{
    // get Tile index
    const auto tileIndex{ m_map->GetTileIndexByPosition(t_mapPoint) };

    // checks whether the Tile is already a Road and in the Vbo
    if (m_lookupTable[tileIndex] > 0)
    {
        return;
    }

    // get Tile
    auto& tile{ m_map->GetTileByIndex(tileIndex) };

    // get vertices of the Tile and make a copy
    auto vertices{ tile.GetVerticesContainer() };

    // we use the same vertices of the tile, but just a little bit higher
    vertices[1] = 0.001f;
    vertices[13] = 0.001f;
    vertices[25] = 0.001f;
    vertices[37] = 0.001f;
    vertices[49] = 0.001f;
    vertices[61] = 0.001f;

    // get the right texture
    const auto texture{ static_cast<float>(GetTexture(tile)) };
    vertices[9] = texture;
    vertices[21] = texture;
    vertices[33] = texture;
    vertices[45] = texture;
    vertices[57] = texture;
    vertices[69] = texture;

    // insert vertices at the end
    m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());

    // calculate the Tile number in m_vertices
    const auto nrTiles{ static_cast<int>(m_vertices.size()) / Tile::FLOATS_PER_TILE };

    // use Tile number to store where the road is in the Vbo
    m_lookupTable[tileIndex] = nrTiles;

    // update Vbo
    ogl::buffer::Vbo::BindVbo(m_vboId);
    glBufferSubData(GL_ARRAY_BUFFER, 0, nrTiles * Tile::SIZE_IN_BYTES_PER_TILE, m_vertices.data());
    ogl::buffer::Vbo::UnbindVbo();

    // update draw count
    m_roadNetworkMesh->GetVao().SetDrawCount(nrTiles * Tile::VERTICES_PER_TILE);

    // update neighbours
    UpdateNeighbours(tile);
}

//-------------------------------------------------
// Init
//-------------------------------------------------

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
    m_textures.emplace(Texture::WO, m_map->GetScene()->GetApplicationContext()->GetTextureManager().GetTextureIdFromPath("res/texture/road/wo.png"));
    m_textures.emplace(Texture::NS, m_map->GetScene()->GetApplicationContext()->GetTextureManager().GetTextureIdFromPath("res/texture/road/ns.png"));
    m_textures.emplace(Texture::NSO, m_map->GetScene()->GetApplicationContext()->GetTextureManager().GetTextureIdFromPath("res/texture/road/nso.png"));
    m_textures.emplace(Texture::NSW, m_map->GetScene()->GetApplicationContext()->GetTextureManager().GetTextureIdFromPath("res/texture/road/nsw.png"));
    m_textures.emplace(Texture::ALL, m_map->GetScene()->GetApplicationContext()->GetTextureManager().GetTextureIdFromPath("res/texture/road/all.png"));

    // Every Tile can be a Road. The lookup table stores the position of the Tile in the RoadNetwork Vbo.
    // The default value -1 means that the Tile is not a Road and is not in the Vbo.
    const auto lookupTableSize(m_map->GetMapSize() * m_map->GetMapSize());
    m_lookupTable.resize(lookupTableSize, -1);
}

sg::city::map::RoadNetwork::Texture sg::city::map::RoadNetwork::GetTexture(const Tile& t_tile)
{
    const auto& neighbours{ t_tile.GetNeighbours() };

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

    Texture texture;
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
    case 4:
    case 5: texture = Texture::NS;
        break;
    case 6:
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
    case 12:
    case 13: texture = Texture::NSW;
        break;
    case 14:
    case 15: texture = Texture::ALL;
        break;
    default: texture = Texture::WO;
    }

    return texture;
}

void sg::city::map::RoadNetwork::UpdateNeighbours(const Tile& t_tile)
{
    auto neighbours{ t_tile.GetNeighbours() };

    if (neighbours[static_cast<int>(Tile::Directions::NORTH)])
    {
        if (neighbours[static_cast<int>(Tile::Directions::NORTH)]->GetType() == Map::TileType::TRAFFIC_NETWORK)
        {
            UpdateExistingTexture(*neighbours[static_cast<int>(Tile::Directions::NORTH)]);
        }
    }

    if (neighbours[static_cast<int>(Tile::Directions::EAST)])
    {
        if (neighbours[static_cast<int>(Tile::Directions::EAST)]->GetType() == Map::TileType::TRAFFIC_NETWORK)
        {
            UpdateExistingTexture(*neighbours[static_cast<int>(Tile::Directions::EAST)]);
        }
    }

    if (neighbours[static_cast<int>(Tile::Directions::SOUTH)])
    {
        if (neighbours[static_cast<int>(Tile::Directions::SOUTH)]->GetType() == Map::TileType::TRAFFIC_NETWORK)
        {
            UpdateExistingTexture(*neighbours[static_cast<int>(Tile::Directions::SOUTH)]);
        }
    }

    if (neighbours[static_cast<int>(Tile::Directions::WEST)])
    {
        if (neighbours[static_cast<int>(Tile::Directions::WEST)]->GetType() == Map::TileType::TRAFFIC_NETWORK)
        {
            UpdateExistingTexture(*neighbours[static_cast<int>(Tile::Directions::WEST)]);
        }
    }
}

void sg::city::map::RoadNetwork::UpdateExistingTexture(const Tile& t_tile)
{
    // get the right texture
    const auto texture{ static_cast<float>(GetTexture(t_tile)) };

    // get Tile index
    const auto tileIndex{ m_map->GetTileIndexByPosition(static_cast<int>(t_tile.GetMapX()), static_cast<int>(t_tile.GetMapZ())) };

    // get the position in the Vbo
    const auto lookupIndex{ m_lookupTable[tileIndex] - 1 };

    // update texture
    const auto offset{ lookupIndex * Tile::FLOATS_PER_TILE };
    m_vertices[offset + 9] = texture;
    m_vertices[offset + 21] = texture;
    m_vertices[offset + 33] = texture;
    m_vertices[offset + 45] = texture;
    m_vertices[offset + 57] = texture;
    m_vertices[offset + 69] = texture;
}
