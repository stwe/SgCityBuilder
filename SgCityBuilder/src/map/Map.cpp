#include <resource/Mesh.h>
#include "Map.h"
#include "Tile.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

sg::city::map::Map::Map()
{
}

sg::city::map::Map::~Map() noexcept
{
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

const sg::ogl::resource::Mesh& sg::city::map::Map::GetMapMesh() const noexcept
{
    return *m_mapMesh;
}

sg::ogl::resource::Mesh& sg::city::map::Map::GetMapMesh() noexcept
{
    return *m_mapMesh;
}

const sg::city::map::Map::TileContainer& sg::city::map::Map::GetTiles() const noexcept
{
    return m_tiles;
}

sg::city::map::Map::TileContainer& sg::city::map::Map::GetTiles() noexcept
{
    return m_tiles;
}

int sg::city::map::Map::GetMapSize() const
{
    return m_mapSize;
}

//-------------------------------------------------
// Create
//-------------------------------------------------


/*


(0, -2) ------ (1, -2) ------ (2, -2)
 |               |              |
 |    0, 1       |   1, 1       |
 |               |              |
(0, -1) ------ (1, -1) ------ (2, -1)
 |               |              |
 |    0, 0       |   1, 0       |
 |               |              |
(0,  0) ------ (1,  0) ------ (2, 0)


 */


void sg::city::map::Map::CreateMap(const int t_mapSize)
{
    m_mapSize = t_mapSize;

    // create Tiles
    for (auto z{ 0 }; z < m_mapSize; ++z)
    {
        for (auto x{ 0 }; x < m_mapSize; ++x)
        {
            auto tile{ std::make_unique<Tile>(
                static_cast<float>(x),
                static_cast<float>(z),
                Tile::Type::NONE
                )
            };

            tile->SetParentMap(this);

            m_tiles.push_back(std::move(tile));
        }
    }

    // create an bind a Vao
    m_mapMesh = std::make_unique<ogl::resource::Mesh>();
    m_mapMesh->GetVao().BindVao();

    // create Vbo and store Tiles
    CreateVbo();
    StoreTilesInVbo();

    // unbind Vao
    ogl::buffer::Vao::UnbindVao();

    // set draw count
    m_mapMesh->GetVao().SetDrawCount(GetVerticesCountOfMap());
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

uint32_t sg::city::map::Map::GetFloatCountOfMap() const
{
    return static_cast<uint32_t>(m_tiles.size()) * Tile::FLOATS_PER_TILE;
}

int32_t sg::city::map::Map::GetVerticesCountOfMap() const
{
    return static_cast<int32_t>(m_tiles.size())* Tile::VERTICES_PER_TILE;
}

void sg::city::map::Map::CreateVbo()
{
    m_vboId = ogl::buffer::Vbo::GenerateVbo();

    ogl::buffer::Vbo::InitEmpty(m_vboId, GetFloatCountOfMap(), GL_DYNAMIC_DRAW);

    ogl::buffer::Vbo::AddAttribute(m_vboId, 0, 3, Tile::FLOATS_PER_VERTEX, 0);
    ogl::buffer::Vbo::AddAttribute(m_vboId, 1, 3, Tile::FLOATS_PER_VERTEX, 3);
    ogl::buffer::Vbo::AddAttribute(m_vboId, 2, 3, Tile::FLOATS_PER_VERTEX, 6);
}

void sg::city::map::Map::StoreTilesInVbo()
{
    ogl::buffer::Vbo::BindVbo(m_vboId);

    auto offset{ 0 };
    for (const auto& tile : m_tiles)
    {
        glBufferSubData(GL_ARRAY_BUFFER, offset * Tile::SIZE_IN_BYTES_PER_TILE, Tile::SIZE_IN_BYTES_PER_TILE, tile->GetVerticesContainer().data());
        offset++;
    }

    ogl::buffer::Vbo::UnbindVbo();
}
