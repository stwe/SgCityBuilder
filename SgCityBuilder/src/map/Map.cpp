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
#include <Application.h>
#include <Window.h>
#include <camera/Camera.h>
#include <scene/Scene.h>
#include <resource/Mesh.h>
#include <resource/ShaderManager.h>
#include <resource/TextureManager.h>
#include <math/Transform.h>
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

int sg::city::map::Map::GetNrOfAllTiles() const
{
    return m_mapSize * m_mapSize;
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

uint32_t sg::city::map::Map::GetBuildingTextureAtlasId() const
{
    return m_buildingTextureAtlasId;
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

const sg::city::map::Map::TileNavigationNodeContainer& sg::city::map::Map::GetNavigationNodes() const noexcept
{
    return m_tileNavigationNodes;
}

sg::city::map::Map::TileNavigationNodeContainer& sg::city::map::Map::GetNavigationNodes() noexcept
{
    return m_tileNavigationNodes;
}

const sg::city::map::Map::NavigationNodeContainer& sg::city::map::Map::GetNavigationNodes(const int t_index) const noexcept
{
    return m_tileNavigationNodes[t_index];
}

sg::city::map::Map::NavigationNodeContainer& sg::city::map::Map::GetNavigationNodes(const int t_index) noexcept
{
    return m_tileNavigationNodes[t_index];
}

int sg::city::map::Map::GetNumRegions() const
{
    return m_numRegions;
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
    StoreTiles();
    StoreTileNeighbours();
    StoreRandomColors();
    StoreTileNavigationNodes();
    LinkTileNavigationNodes();

    // for debug
    CreateNavigationNodesMesh();

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
// Regions
//-------------------------------------------------

void sg::city::map::Map::FindConnectedRegions()
{
    // todo: in city update

    auto regions{ 0 };

    // delete the regions Id from all Tiles
    for (auto& tile : m_tiles)
    {
        tile->region = tile::Tile::NO_REGION;
    }

    for (auto& tile : m_tiles)
    {
        auto found{ false };
        const auto tileIndex{ tile->GetMapIndex() };

        for (auto tileType : tile::Tile::REGION_TILE_TYPES)
        {
            if (tileType == m_tiles[tileIndex]->type)
            {
                found = true;
                break;
            }
        }

        if (m_tiles[tileIndex]->region == tile::Tile::NO_REGION && found)
        {
            regions++;
            DepthSearch(*m_tiles[tileIndex], regions);
        }
    }

    m_numRegions = regions;
}

//-------------------------------------------------
// Debug
//-------------------------------------------------

void sg::city::map::Map::CreateNavigationNodesMesh()
{
    SG_OGL_CORE_ASSERT(!m_tileNavigationNodes.empty(), "[Map::CreateNavigationNodesMesh()] No Navigation Nodes available.")

    VertexContainer vertexContainer;

    for (auto tileIndex{ 0 }; tileIndex < GetNrOfAllTiles(); ++tileIndex)
    {
        if (m_tiles[tileIndex]->type != tile::TileType::TRAFFIC)
        {
            continue;
        }

        for (auto& node : m_tileNavigationNodes[tileIndex])
        {
            // some nodes are nullptr
            if (node)
            {
                // position
                vertexContainer.push_back(node->position.x);
                vertexContainer.push_back(VERTEX_HEIGHT);
                vertexContainer.push_back(node->position.z);

                // color
                if (node->block)
                {
                    // red
                    vertexContainer.push_back(1.0f);
                    vertexContainer.push_back(0.0f);
                    vertexContainer.push_back(0.0f);
                }
                else
                {
                    // green
                    vertexContainer.push_back(0.0f);
                    vertexContainer.push_back(1.0f);
                    vertexContainer.push_back(0.0f);
                }
            }
        }
    }

    if (vertexContainer.empty())
    {
        return;
    }

    if (m_navigationNodesMesh)
    {
        m_navigationNodesMesh.reset();
    }

    m_navigationNodesMesh = std::make_unique<ogl::resource::Mesh>();

    const ogl::buffer::BufferLayout bufferLayout{
        { ogl::buffer::VertexAttributeType::POSITION, "aPosition" },
        { ogl::buffer::VertexAttributeType::COLOR, "aColor" },
    };

    m_navigationNodesMesh->GetVao().AddVertexDataVbo(vertexContainer.data(), static_cast<int32_t>(vertexContainer.size()) / 6, bufferLayout);
}

void sg::city::map::Map::RenderNavigationNodes() const
{
    if (!m_navigationNodesMesh)
    {
        SG_OGL_LOG_WARN("[Map::RenderNavigationNodes()] Mesh was not created.");

        return;
    }

    ogl::math::Transform t;
    t.position = position;
    t.rotation = rotation;
    t.scale = scale;

    auto& shader{ m_scene->GetApplicationContext()->GetShaderManager().GetShaderProgram<shader::NodeShader>() };
    shader.Bind();

    const auto projectionMatrix{ m_scene->GetApplicationContext()->GetWindow().GetProjectionMatrix() };
    const auto mvp{ projectionMatrix * m_scene->GetCurrentCamera().GetViewMatrix() * static_cast<glm::mat4>(t) };

    shader.SetUniform("mvpMatrix", mvp);

    glPointSize(POINT_SIZE);

    m_navigationNodesMesh->InitDraw();
    m_navigationNodesMesh->DrawPrimitives(GL_POINTS);
    m_navigationNodesMesh->EndDraw();

    ogl::resource::ShaderProgram::Unbind();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void sg::city::map::Map::StoreTileTypeTextures()
{
    SG_OGL_LOG_DEBUG("[Map::StoreTileTypeTextures()] Load all textures.");

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

    m_roadTextureAtlasId = m_scene->GetApplicationContext()->GetTextureManager().GetTextureIdFromPath("res/texture/road/roads.png");
    m_buildingTextureAtlasId = m_scene->GetApplicationContext()->GetTextureManager().GetTextureIdFromPath("res/texture/sc.png", true);
}

void sg::city::map::Map::StoreTiles()
{
    SG_OGL_LOG_DEBUG("[Map::StoreTiles()] Create ans store {}x{} Tiles for the map.", m_mapSize, m_mapSize);

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

void sg::city::map::Map::StoreTileNavigationNodes()
{
    SG_OGL_CORE_ASSERT(!m_tiles.empty(), "[Map::StoreTileNavigationNodes()] No Tiles available.")
    SG_OGL_CORE_ASSERT(m_tileNavigationNodes.empty(), "[Map::StoreTileNavigationNodes()] Navigation Nodes already exists.")

    SG_OGL_LOG_DEBUG("[Map::StoreTileNavigationNodes()] Store Navigation Nodes for the Tiles.");

    m_tileNavigationNodes.resize(GetNrOfAllTiles());

    auto tileIndex{ 0 };
    for (auto& tile : m_tiles)
    {
        NavigationNodeContainer navigationNodes;

        for (auto z{ 0 }; z < 7; ++z)
        {
            auto zOffset{ 0.0f };
            switch (z)
            {
            case 0: zOffset = 0.000f; break;
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
                navigationNodes.push_back(std::make_unique<automata::AutoNode>(glm::vec3(
                        tile->GetWorldX() + xOffset,
                        0.0f,
                        tile->GetWorldZ() + zOffset)
                    )
                );
            }
        }

        m_tileNavigationNodes[tileIndex] = navigationNodes;
        tileIndex++;
    }
}

void sg::city::map::Map::LinkTileNavigationNodes()
{
    SG_OGL_CORE_ASSERT(!m_tiles.empty(), "[Map::LinkTileNavigationNodes()] No Tiles available.")
    SG_OGL_CORE_ASSERT(!m_tileNavigationNodes.empty(), "[Map::LinkTileNavigationNodes()] No Navigation Nodes available.")

    SG_OGL_LOG_DEBUG("[Map::LinkTileNavigationNodes()] Link neighboring Navigation Nodes.");

    for (auto z{ 0 }; z < m_mapSize; ++z)
    {
        for (auto x{ 0 }; x < m_mapSize; ++x)
        {
            const auto currentTileIndex{ GetTileMapIndexByMapPosition(x, z) };
            auto& currentTile{ m_tiles[currentTileIndex] };

            if (z < m_mapSize - 1)
            {
                const auto northTileIndex{ currentTile->GetNeighbours().at(tile::Direction::NORTH) };

                m_tileNavigationNodes[currentTileIndex][42] = m_tileNavigationNodes[northTileIndex][0];
                m_tileNavigationNodes[currentTileIndex][43] = m_tileNavigationNodes[northTileIndex][1];
                m_tileNavigationNodes[currentTileIndex][44] = m_tileNavigationNodes[northTileIndex][2];
                m_tileNavigationNodes[currentTileIndex][45] = m_tileNavigationNodes[northTileIndex][3];
                m_tileNavigationNodes[currentTileIndex][46] = m_tileNavigationNodes[northTileIndex][4];
                m_tileNavigationNodes[currentTileIndex][47] = m_tileNavigationNodes[northTileIndex][5];
                m_tileNavigationNodes[currentTileIndex][48] = m_tileNavigationNodes[northTileIndex][6];
            }

            if (x < m_mapSize - 1)
            {
                const auto eastTileIndex{ currentTile->GetNeighbours().at(tile::Direction::EAST) };

                m_tileNavigationNodes[currentTileIndex][48] = m_tileNavigationNodes[eastTileIndex][42];
                m_tileNavigationNodes[currentTileIndex][41] = m_tileNavigationNodes[eastTileIndex][35];
                m_tileNavigationNodes[currentTileIndex][34] = m_tileNavigationNodes[eastTileIndex][28];
                m_tileNavigationNodes[currentTileIndex][27] = m_tileNavigationNodes[eastTileIndex][21];
                m_tileNavigationNodes[currentTileIndex][20] = m_tileNavigationNodes[eastTileIndex][14];
                m_tileNavigationNodes[currentTileIndex][13] = m_tileNavigationNodes[eastTileIndex][7];
                m_tileNavigationNodes[currentTileIndex][6] = m_tileNavigationNodes[eastTileIndex][0];
            }
        }
    }

    for (auto tileIndex{ 0 }; tileIndex < GetNrOfAllTiles(); ++tileIndex)
    {
        m_tileNavigationNodes[tileIndex][37].reset();
        m_tileNavigationNodes[tileIndex][39].reset();
        m_tileNavigationNodes[tileIndex][29].reset();
        m_tileNavigationNodes[tileIndex][33].reset();
        m_tileNavigationNodes[tileIndex][15].reset();
        m_tileNavigationNodes[tileIndex][19].reset();
        m_tileNavigationNodes[tileIndex][9].reset();
        m_tileNavigationNodes[tileIndex][11].reset();

        // the 4 corners
        m_tileNavigationNodes[tileIndex][42].reset();
        m_tileNavigationNodes[tileIndex][48].reset();
        m_tileNavigationNodes[tileIndex][0].reset();
        m_tileNavigationNodes[tileIndex][6].reset();
    }
}

void sg::city::map::Map::StoreRandomColors()
{
    SG_OGL_LOG_DEBUG("[Map::StoreRandomColors()] Store {} random Colors.", MAX_REGION_COLORS);

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

//-------------------------------------------------
// Helper
//-------------------------------------------------

int32_t sg::city::map::Map::GetVerticesCountOfMap() const
{
    return static_cast<int32_t>(m_tiles.size()) * tile::Tile::VERTICES_PER_TILE;
}

void sg::city::map::Map::DepthSearch(tile::Tile& t_startTile, const int t_region)
{
    if (t_startTile.region != tile::Tile::NO_REGION)
    {
        return;
    }

    auto found{ false };

    for (auto tileType : tile::Tile::REGION_TILE_TYPES)
    {
        if (tileType == t_startTile.type)
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        return;
    }

    t_startTile.region = t_region;

    // changing the color needs also a Vbo update
    t_startTile.SetColor(static_cast<glm::vec3>(m_randomColors[t_region - 1]));
    UpdateMapVboByTileIndex(t_startTile.GetMapIndex());

    for (auto& neighbour : t_startTile.GetNeighbours())
    {
        DepthSearch(*m_tiles[neighbour.second], t_region);
    }
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
