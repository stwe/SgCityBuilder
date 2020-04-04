// This file is part of the SgCityBuilder package.
// 
// Filename: Map.cpp
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#include <random>
#include <Color.h>
#include <Log.h>
#include <Core.h>
#include <Application.h>
#include <scene/Scene.h>
#include <resource/Mesh.h>
#include <resource/ShaderManager.h>
#include <resource/TextureManager.h>
#include "Map.h"
#include "shader/LineShader.h"
#include "shader/NodeShader.h"
#include "automata/AutoNode.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

sg::city::map::Map::Map(ogl::scene::Scene* t_scene)
    : m_scene{ t_scene }
{
    SG_OGL_CORE_ASSERT(t_scene, "[Map::Map()] Null pointer.")

    SG_OGL_LOG_DEBUG("[Map::Map()] Construct Map.");

    // shader needed for debug
    m_scene->GetApplicationContext()->GetShaderManager().AddShaderProgram<shader::NodeShader>();
    m_scene->GetApplicationContext()->GetShaderManager().AddShaderProgram<shader::LineShader>();
}

sg::city::map::Map::~Map() noexcept
{
    SG_OGL_LOG_DEBUG("[Map::~Map()] Destruct Map.");
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

sg::ogl::scene::Scene* sg::city::map::Map::GetScene() const
{
    return m_scene;
}

int sg::city::map::Map::GetMapSize() const
{
    return m_mapSize;
}

const sg::city::map::Map::TileTypeTextureContainer& sg::city::map::Map::GetTileTypeTextures() const noexcept
{
    return m_tileTypeTextures;
}

sg::city::map::Map::TileTypeTextureContainer& sg::city::map::Map::GetTileTypeTextures() noexcept
{
    return m_tileTypeTextures;
}

uint32_t sg::city::map::Map::GetRoadTextureAtlasId() const
{
    return m_roadTextureAtlasId;
}

const sg::city::map::Map::TileContainer& sg::city::map::Map::GetTiles() const noexcept
{
    return m_tiles;
}

sg::city::map::Map::TileContainer& sg::city::map::Map::GetTiles() noexcept
{
    return m_tiles;
}

const sg::ogl::resource::Mesh& sg::city::map::Map::GetMapMesh() const noexcept
{
    return *m_mapMesh;
}

sg::ogl::resource::Mesh& sg::city::map::Map::GetMapMesh() noexcept
{
    return *m_mapMesh;
}

uint32_t sg::city::map::Map::GetFloatCountOfMap() const
{
    return static_cast<uint32_t>(m_tiles.size()) * tile::Tile::FLOATS_PER_TILE;
}

//-------------------------------------------------
// Get Tile
//-------------------------------------------------

const sg::city::map::tile::Tile& sg::city::map::Map::GetTileByMapPosition(const int t_mapX, const int t_mapZ) const noexcept
{
    return *m_tiles[GetTileMapIndexByMapPosition(t_mapX, t_mapZ)];
}

sg::city::map::tile::Tile& sg::city::map::Map::GetTileByMapPosition(const int t_mapX, const int t_mapZ) noexcept
{
    return *m_tiles[GetTileMapIndexByMapPosition(t_mapX, t_mapZ)];
}

int sg::city::map::Map::GetTileMapIndexByMapPosition(const int t_mapX, const int t_mapZ) const
{
    SG_OGL_CORE_ASSERT(t_mapX < m_mapSize, "[Map::GetTileMapIndexByMapPosition()] Invalid x position.")
    SG_OGL_CORE_ASSERT(t_mapZ < m_mapSize, "[Map::GetTileMapIndexByMapPosition()] Invalid z position.")

    return t_mapZ * m_mapSize + t_mapX;
}

//-------------------------------------------------
// Create
//-------------------------------------------------

void sg::city::map::Map::CreateMap(const int t_mapSize)
{
    SG_OGL_CORE_ASSERT(t_mapSize, "[Map::CreateMap()] Invalid Map size.")

    m_mapSize = t_mapSize;

    StoreTileTypeTextures();
    CreateTiles();
    StoreTileNeighbours();
    CreateRandomColors();
    CreateNavigationNodes();
    LinkTileNavigationNodes();

    // create an bind a new Vao
    m_mapMesh = std::make_unique<ogl::resource::Mesh>();
    m_mapMesh->GetVao().BindVao();

    // create a new Vbo and store Tiles
    CreateVbo();
    StoreTilesInVbo();

    // unbind Vao
    ogl::buffer::Vao::UnbindVao();

    // set draw count
    m_mapMesh->GetVao().SetDrawCount(GetVerticesCountOfMap());
}

//-------------------------------------------------
// Update
//-------------------------------------------------

void sg::city::map::Map::UpdateMapVboByTileIndex(const int t_tileIndex) const
{
    ogl::buffer::Vbo::BindVbo(m_vboId);
    glBufferSubData(GL_ARRAY_BUFFER, t_tileIndex * tile::Tile::SIZE_IN_BYTES_PER_TILE, tile::Tile::SIZE_IN_BYTES_PER_TILE, m_tiles[t_tileIndex]->GetVertices().data());
    ogl::buffer::Vbo::UnbindVbo();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void sg::city::map::Map::StoreTileTypeTextures()
{
    SG_OGL_LOG_DEBUG("[Map::LoadAndStoreTileTypeTextures()] Load a texture for each Tile Type.");

    const auto n{ m_scene->GetApplicationContext()->GetTextureManager().GetTextureIdFromPath("res/texture/tileTypes/grass.jpg") };
    const auto r{ m_scene->GetApplicationContext()->GetTextureManager().GetTextureIdFromPath("res/texture/tileTypes/r.png") };
    const auto c{ m_scene->GetApplicationContext()->GetTextureManager().GetTextureIdFromPath("res/texture/tileTypes/c.png") };
    const auto i{ m_scene->GetApplicationContext()->GetTextureManager().GetTextureIdFromPath("res/texture/tileTypes/i.png") };
    const auto t{ m_scene->GetApplicationContext()->GetTextureManager().GetTextureIdFromPath("res/texture/tileTypes/traffic.jpg") };

    m_tileTypeTextures.emplace(tile::TileType::NONE, n);
    m_tileTypeTextures.emplace(tile::TileType::RESIDENTIAL, r);
    m_tileTypeTextures.emplace(tile::TileType::COMMERCIAL, c);
    m_tileTypeTextures.emplace(tile::TileType::INDUSTRIAL, i);
    m_tileTypeTextures.emplace(tile::TileType::TRAFFIC, t);

    SG_OGL_LOG_DEBUG("[Map::LoadAndStoreTileTypeTextures()] Load road texture atlas.");

    m_roadTextureAtlasId = m_scene->GetApplicationContext()->GetTextureManager().GetTextureIdFromPath("res/texture/road/roads.png");
}

void sg::city::map::Map::CreateTiles()
{
    SG_OGL_LOG_DEBUG("[Map::CreateTiles()] Create {}x{} Tiles for the map.", m_mapSize, m_mapSize);

    for (auto z{ 0 }; z < m_mapSize; ++z)
    {
        for (auto x{ 0 }; x < m_mapSize; ++x)
        {
            auto tile{ std::make_unique<tile::Tile>(
                    static_cast<float>(x),
                    static_cast<float>(z),
                    tile::TileType::NONE,
                    this
                )
            };

            m_tiles.push_back(std::move(tile));
        }
    }
}

void sg::city::map::Map::StoreTileNeighbours()
{
    SG_OGL_CORE_ASSERT(!m_tiles.empty(), "[Map::StoreTileNeighbours()] No Tiles available.")

    SG_OGL_LOG_DEBUG("[Map::StoreTileNeighbours()] Store Tile neighbors.");

    for (auto z{ 0 }; z < m_mapSize; ++z)
    {
        for (auto x{ 0 }; x < m_mapSize; ++x)
        {
            const auto tileIndex{ GetTileMapIndexByMapPosition(x, z) };

            if (z < m_mapSize - 1)
            {
                m_tiles[tileIndex]->GetNeighbours().emplace(tile::Direction::NORTH, GetTileMapIndexByMapPosition(x, z + 1));
            }

            if (x < m_mapSize - 1)
            {
                m_tiles[tileIndex]->GetNeighbours().emplace(tile::Direction::EAST, GetTileMapIndexByMapPosition(x + 1, z));
            }

            if (z > 0)
            {
                m_tiles[tileIndex]->GetNeighbours().emplace(tile::Direction::SOUTH, GetTileMapIndexByMapPosition(x, z - 1));
            }

            if (x > 0)
            {
                m_tiles[tileIndex]->GetNeighbours().emplace(tile::Direction::WEST, GetTileMapIndexByMapPosition(x - 1, z));
            }
        }
    }
}

void sg::city::map::Map::CreateRandomColors()
{
    SG_OGL_LOG_DEBUG("[Map::CreateRandomColors()] Create {} random Colors.", MAX_REGION_COLORS);

    std::random_device seeder;
    std::mt19937 engine(seeder());

    const std::uniform_int_distribution<unsigned int> r(7, 164);
    const std::uniform_int_distribution<unsigned int> g(1, 160);
    const std::uniform_int_distribution<unsigned int> b(124, 254);

    for (auto i{ 0 }; i < MAX_REGION_COLORS; ++i)
    {
        m_randomColors.emplace(i, ogl::Color(r(engine), g(engine), b(engine)));
    }
}

void sg::city::map::Map::CreateNavigationNodes()
{
    SG_OGL_CORE_ASSERT(!m_tiles.empty(), "[Map::CreateNavigationNodes()] No Tiles available.")

    SG_OGL_LOG_DEBUG("[Map::CreateNavigationNodes()] Create navigation nodes for the Tiles.");

    for (auto& tile : m_tiles)
    {
        SG_OGL_CORE_ASSERT(tile->GetNavigationNodes().empty(), "[Map::CreateNavigationNodes()] The navigation nodes already exist.")

        for (auto z{ 0 }; z < 7; ++z)
        {
            auto zOffset{ 0.0f };
            switch (z)
            {
            case 0: zOffset =  0.000f; break;
            case 1: zOffset = -0.083f; break;
            case 2: zOffset = -0.333f; break;
            case 3: zOffset = -0.500f; break;
            case 4: zOffset = -0.667f; break;
            case 5: zOffset = -0.917f; break;
            case 6: zOffset = -1.000f; break;
            default:;
            }

            for (auto x{ 0 }; x < 7; ++x)
            {
                auto xOffset{ 0.0f };
                switch (x)
                {
                case 0: xOffset = 0.000f; break;
                case 1: xOffset = 0.083f; break;
                case 2: xOffset = 0.333f; break;
                case 3: xOffset = 0.500f; break;
                case 4: xOffset = 0.667f; break;
                case 5: xOffset = 0.917f; break;
                case 6: xOffset = 1.000f; break;
                default:;
                }

                // converting unique_ptr to shared_ptr
                tile->GetNavigationNodes().push_back(std::make_unique<automata::AutoNode>(glm::vec3(
                    tile->GetWorldX() + xOffset,
                    0.0f,
                    tile->GetWorldZ() + zOffset))
                );
            }
        }
    }
}

void sg::city::map::Map::LinkTileNavigationNodes()
{
    SG_OGL_LOG_DEBUG("[Map::LinkTileNavigationNodes()] Link neighboring navigation nodes.");

    for (auto z{ 0 }; z < m_mapSize; ++z)
    {
        for (auto x{ 0 }; x < m_mapSize; ++x)
        {
            auto& currentTile{ m_tiles[GetTileMapIndexByMapPosition(x, z)] };

            if (z < m_mapSize - 1)
            {
                const auto northIndex{ currentTile->GetNeighbours().at(tile::Direction::NORTH) };

                currentTile->GetNavigationNodes()[42] = m_tiles[northIndex]->GetNavigationNodes()[0];
                currentTile->GetNavigationNodes()[43] = m_tiles[northIndex]->GetNavigationNodes()[1];
                currentTile->GetNavigationNodes()[44] = m_tiles[northIndex]->GetNavigationNodes()[2];
                currentTile->GetNavigationNodes()[45] = m_tiles[northIndex]->GetNavigationNodes()[3];
                currentTile->GetNavigationNodes()[46] = m_tiles[northIndex]->GetNavigationNodes()[4];
                currentTile->GetNavigationNodes()[47] = m_tiles[northIndex]->GetNavigationNodes()[5];
                currentTile->GetNavigationNodes()[48] = m_tiles[northIndex]->GetNavigationNodes()[6];
            }

            if (x < m_mapSize - 1)
            {
                const auto eastIndex{ currentTile->GetNeighbours().at(tile::Direction::EAST) };

                currentTile->GetNavigationNodes()[48] = m_tiles[eastIndex]->GetNavigationNodes()[42];
                currentTile->GetNavigationNodes()[41] = m_tiles[eastIndex]->GetNavigationNodes()[35];
                currentTile->GetNavigationNodes()[34] = m_tiles[eastIndex]->GetNavigationNodes()[28];
                currentTile->GetNavigationNodes()[27] = m_tiles[eastIndex]->GetNavigationNodes()[21];
                currentTile->GetNavigationNodes()[20] = m_tiles[eastIndex]->GetNavigationNodes()[14];
                currentTile->GetNavigationNodes()[13] = m_tiles[eastIndex]->GetNavigationNodes()[7];
                currentTile->GetNavigationNodes()[6] = m_tiles[eastIndex]->GetNavigationNodes()[0];
            }
        }
    }

    for (auto& tile : m_tiles)
    {
        auto& navigationNodes{ tile->GetNavigationNodes() };

        navigationNodes[37].reset();
        navigationNodes[39].reset();
        navigationNodes[29].reset();
        navigationNodes[33].reset();
        navigationNodes[15].reset();
        navigationNodes[19].reset();
        navigationNodes[9].reset();
        navigationNodes[11].reset();

        // the 4 corners
        navigationNodes[42].reset();
        navigationNodes[48].reset();
        navigationNodes[0].reset();
        navigationNodes[6].reset();
    }
}

void sg::city::map::Map::CreateNavigationNodesMeshes()
{
    SG_OGL_LOG_DEBUG("[Map::CreateNavigationNodesMeshes()] Create navigation nodes meshes.");

    for (auto& tile : m_tiles)
    {
        tile->CreateNavigationNodesMesh();
    }
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

int32_t sg::city::map::Map::GetVerticesCountOfMap() const
{
    return static_cast<int32_t>(m_tiles.size()) * tile::Tile::VERTICES_PER_TILE;
}

//-------------------------------------------------
// Vbo
//-------------------------------------------------

void sg::city::map::Map::CreateVbo()
{
    m_vboId = ogl::buffer::Vbo::GenerateVbo();

    ogl::buffer::Vbo::InitEmpty(m_vboId, GetFloatCountOfMap(), GL_DYNAMIC_DRAW);

    ogl::buffer::Vbo::AddAttribute(m_vboId, 0, 3, tile::Tile::FLOATS_PER_VERTEX, 0);  // 3x position
    ogl::buffer::Vbo::AddAttribute(m_vboId, 1, 3, tile::Tile::FLOATS_PER_VERTEX, 3);  // 3x normal
    ogl::buffer::Vbo::AddAttribute(m_vboId, 2, 3, tile::Tile::FLOATS_PER_VERTEX, 6);  // 3x color
    ogl::buffer::Vbo::AddAttribute(m_vboId, 3, 1, tile::Tile::FLOATS_PER_VERTEX, 9);  // 1x texture
    ogl::buffer::Vbo::AddAttribute(m_vboId, 4, 2, tile::Tile::FLOATS_PER_VERTEX, 10); // 2x uv
}

void sg::city::map::Map::StoreTilesInVbo()
{
    ogl::buffer::Vbo::BindVbo(m_vboId);

    auto offset{ 0 };
    for (const auto& tile : m_tiles)
    {
        glBufferSubData(GL_ARRAY_BUFFER, offset * tile::Tile::SIZE_IN_BYTES_PER_TILE, tile::Tile::SIZE_IN_BYTES_PER_TILE, tile->GetVertices().data());
        offset++;
    }

    ogl::buffer::Vbo::UnbindVbo();
}
