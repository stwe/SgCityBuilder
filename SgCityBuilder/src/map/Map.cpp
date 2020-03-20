// This file is part of the SgCityBuilder package.
// 
// Filename: Map.cpp
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#include <Application.h>
#include <Color.h>
#include <scene/Scene.h>
#include <resource/Mesh.h>
#include <resource/TextureManager.h>
#include <random>
#include "Map.h"
#include "Tile.h"
#include "automata/AutoNode.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

sg::city::map::Map::Map(ogl::scene::Scene* t_scene)
    : m_scene{ t_scene }
{
}

sg::city::map::Map::~Map() noexcept
{
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

sg::ogl::scene::Scene* sg::city::map::Map::GetScene() const
{
    return m_scene;
}

const sg::city::map::Map::TileTypeTextureContainer& sg::city::map::Map::GetTileTypeTextures() const noexcept
{
    return m_tileTypeTextures;
}

sg::city::map::Map::TileTypeTextureContainer& sg::city::map::Map::GetTileTypeTextures() noexcept
{
    return m_tileTypeTextures;
}

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

const sg::city::map::Map::AutoNodeContainer& sg::city::map::Map::GetAutoNodes() const noexcept
{
    return m_autoNodes;
}

sg::city::map::Map::AutoNodeContainer& sg::city::map::Map::GetAutoNodes() noexcept
{
    return m_autoNodes;
}

const sg::city::map::Tile& sg::city::map::Map::GetTileByIndex(const int t_tileIndex) const noexcept
{
    return *m_tiles[t_tileIndex];
}

sg::city::map::Tile& sg::city::map::Map::GetTileByIndex(const int t_tileIndex) noexcept
{
    return *m_tiles[t_tileIndex];
}

const sg::city::map::Tile& sg::city::map::Map::GetTileByPosition(const glm::vec3& t_mapPoint) const noexcept
{
    return *m_tiles[GetTileIndexByPosition(t_mapPoint)];
}

sg::city::map::Tile& sg::city::map::Map::GetTileByPosition(const glm::vec3& t_mapPoint) noexcept
{
    return *m_tiles[GetTileIndexByPosition(t_mapPoint)];
}

int sg::city::map::Map::GetMapSize() const
{
    return m_mapSize;
}

int sg::city::map::Map::GetNumRegions() const
{
    return m_numRegions;
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

    LoadAndStoreTileTypeTextures();

    CreateTiles();
    CreateAutoNodes();

    LinkTiles();

    StoreNeighbours();

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

    // create random colors
    CreateRandomColors();
}

//-------------------------------------------------
// Update
//-------------------------------------------------

void sg::city::map::Map::ChangeTileTypeOnPosition(const glm::vec3& t_mapPoint, const TileType t_tileType)
{
    auto& tile{ GetTileByPosition(t_mapPoint) };
    if (tile.GetType() != t_tileType)
    {
        tile.SetType(t_tileType);
        UpdateMapVboByPosition(t_mapPoint);
    }
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

uint32_t sg::city::map::Map::GetFloatCountOfMap() const
{
    return static_cast<uint32_t>(m_tiles.size()) * Tile::FLOATS_PER_TILE;
}

int sg::city::map::Map::GetTileIndexByPosition(const glm::vec3& t_mapPoint) const
{
    return static_cast<int>(t_mapPoint.z) * m_mapSize + static_cast<int>(t_mapPoint.x);
}

int sg::city::map::Map::GetTileIndexByPosition(const int t_x, int const t_z) const
{
    return t_z * m_mapSize + t_x;
}

//-------------------------------------------------
// Regions
//-------------------------------------------------

void sg::city::map::Map::FindConnectedRegions()
{
    auto regions{ 0 };

    // delete the regions Id from all Tiles
    for (auto& tile : m_tiles)
    {
        tile->SetRegion(NO_REGION);
    }

    for (auto z{ 0 }; z < m_mapSize; ++z)
    {
        for (auto x{ 0 }; x < m_mapSize; ++x)
        {
            auto found{ false };
            const auto tileIndex{ GetTileIndexByPosition(x, z) };

            for (auto tileType : m_tileTypes)
            {
                if (tileType == m_tiles[tileIndex]->GetType())
                {
                    found = true;
                    break;
                }
            }

            if (m_tiles[tileIndex]->GetRegion() == NO_REGION && found)
            {
                regions++;
                DepthSearch(*m_tiles[tileIndex], regions);
            }
        }
    }

    m_numRegions = regions;
}

int32_t sg::city::map::Map::GetVerticesCountOfMap() const
{
    return static_cast<int32_t>(m_tiles.size())* Tile::VERTICES_PER_TILE;
}

void sg::city::map::Map::LoadAndStoreTileTypeTextures()
{
    const auto n{ m_scene->GetApplicationContext()->GetTextureManager().GetTextureIdFromPath("res/texture/tileTypes/grass.jpg") };
    const auto r{ m_scene->GetApplicationContext()->GetTextureManager().GetTextureIdFromPath("res/texture/tileTypes/r.png") };
    const auto c{ m_scene->GetApplicationContext()->GetTextureManager().GetTextureIdFromPath("res/texture/tileTypes/c.png") };
    const auto i{ m_scene->GetApplicationContext()->GetTextureManager().GetTextureIdFromPath("res/texture/tileTypes/i.png") };
    const auto t{ m_scene->GetApplicationContext()->GetTextureManager().GetTextureIdFromPath("res/texture/tileTypes/traffic.jpg") };

    m_tileTypeTextures[static_cast<int>(TileType::NONE)] = n;
    m_tileTypeTextures[static_cast<int>(TileType::RESIDENTIAL)] = r;
    m_tileTypeTextures[static_cast<int>(TileType::COMMERCIAL)] = c;
    m_tileTypeTextures[static_cast<int>(TileType::INDUSTRIAL)] = i;
    m_tileTypeTextures[static_cast<int>(TileType::TRAFFIC_NETWORK)] = t;
}

void sg::city::map::Map::CreateTiles()
{
    for (auto z{ 0 }; z < m_mapSize; ++z)
    {
        for (auto x{ 0 }; x < m_mapSize; ++x)
        {
            auto tile{ std::make_unique<Tile>(
                static_cast<float>(x),
                static_cast<float>(z),
                TileType::NONE
                )
            };

            m_tiles.push_back(std::move(tile));
        }
    }
}

void sg::city::map::Map::CreateAutoNodes()
{
    for (auto& tile : m_tiles)
    {
        for (auto z{ 0 }; z < 7; ++z)
        {
            auto zOffset{ 0.0f };
            switch (z)
            {
            case 0: zOffset = 0.000f; break;
            case 1: zOffset = 0.083f; break;
            case 2: zOffset = 0.333f; break;
            case 3: zOffset = 0.500f; break;
            case 4: zOffset = 0.667f; break;
            case 5: zOffset = 0.917f; break;
            case 6: zOffset = 1.000f; break;
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

                const auto position{ glm::vec3(tile->GetMapX() + xOffset, 0.0f, -tile->GetMapZ() + -zOffset) };
                auto autoNode{ std::make_shared<automata::AutoNode>(position) };
                m_autoNodes.push_back(autoNode);

                tile->GetNavigationNodes().push_back(autoNode);
            }
        }
    }
}

void sg::city::map::Map::LinkTiles()
{
    // todo

    for (auto& tile : m_tiles)
    {
        auto& navigationNodes{ tile->GetNavigationNodes() };

        // link north
        if (static_cast<int>(tile->GetMapZ()) > 0)
        {
            //SG_OGL_LOG_INFO("Map Z: {}", static_cast<int>(tile->GetMapZ()));

            for (auto i{ 0 }; i < 7; ++i)
            {
                /*
                  0  1  2  3  4  5  6 ^
                 42 43 44 45 46 47 48 |
                */

                const auto index{ ((static_cast<int>(tile->GetMapZ()) - 1) * 128 + (static_cast<int>(tile->GetMapX()))) * 49 };

                //SG_OGL_LOG_INFO("nav: {}, auto: {}", 42 + i, index + i);

                navigationNodes[42 + i] = m_autoNodes[index + i];
            }
        }
        else
        {
            for (auto i{ 0 }; i < 7; ++i)
            {
                navigationNodes[42 + i].reset();
            }
        }

        // link west
        if (static_cast<int>(tile->GetMapX()) > 0)
        {
            for (auto i{ 6 }; i >= 0 ; --i)
            {
                /*
                    <---
                48  42
                41  35
                34  28
                27  21
                20  14
                13   7
                 6   0
                */

                const auto index{ (static_cast<int>(tile->GetMapZ()) * 128 + (static_cast<int>(tile->GetMapX()) - 1)) * 49 };

                //SG_OGL_LOG_INFO("nav: {}, auto: {}", i * 7, index + 6 + i * 7);

                navigationNodes[i * 7] = m_autoNodes[index + 6 + i * 7];
            }
        }
        else
        {
            for (auto i{ 6 }; i >= 0; --i)
            {
                navigationNodes[i * 7].reset();
            }
        }

        // link south
        if (static_cast<int>(tile->GetMapZ()) < m_mapSize - 1)
        {
        }
        else
        {
            for (auto i{ 0 }; i < 7; ++i)
            {
                navigationNodes[i].reset();
            }
        }

        // link east
        if (static_cast<int>(tile->GetMapX()) < m_mapSize - 1)
        {
        }
        else
        {
            for (auto i{ 6 }; i >= 0; --i)
            {
                const auto index{ 6 + i * 7 };
                navigationNodes[index].reset();
            }
        }

        navigationNodes[9].reset();
        navigationNodes[11].reset();
        navigationNodes[15].reset();
        navigationNodes[19].reset();
        navigationNodes[29].reset();
        navigationNodes[33].reset();
        navigationNodes[37].reset();
        navigationNodes[39].reset();
        navigationNodes[0].reset();
        navigationNodes[6].reset();
        navigationNodes[42].reset();
        navigationNodes[48].reset();
    }
}

void sg::city::map::Map::StoreNeighbours()
{
    for (auto z{ 0 }; z < m_mapSize; ++z)
    {
        for (auto x{ 0 }; x < m_mapSize; ++x)
        {
            const auto tileIndex{ GetTileIndexByPosition(x, z) };

            if (z < m_mapSize - 1)
            {
                m_tiles[tileIndex]->GetNeighbours()[static_cast<int>(Tile::Directions::NORTH)] = m_tiles[GetTileIndexByPosition(x, z + 1)].get();
            }

            if (x < m_mapSize - 1)
            {
                m_tiles[tileIndex]->GetNeighbours()[static_cast<int>(Tile::Directions::EAST)] = m_tiles[GetTileIndexByPosition(x + 1, z)].get();
            }

            if (z > 0)
            {
                m_tiles[tileIndex]->GetNeighbours()[static_cast<int>(Tile::Directions::SOUTH)] = m_tiles[GetTileIndexByPosition(x, z - 1)].get();
            }

            if (x > 0)
            {
                m_tiles[tileIndex]->GetNeighbours()[static_cast<int>(Tile::Directions::WEST)] = m_tiles[GetTileIndexByPosition(x - 1, z)].get();
            }
        }
    }
}

void sg::city::map::Map::CreateRandomColors()
{
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
// Vbo
//-------------------------------------------------

void sg::city::map::Map::CreateVbo()
{
    m_vboId = ogl::buffer::Vbo::GenerateVbo();

    ogl::buffer::Vbo::InitEmpty(m_vboId, GetFloatCountOfMap(), GL_DYNAMIC_DRAW);

    ogl::buffer::Vbo::AddAttribute(m_vboId, 0, 3, Tile::FLOATS_PER_VERTEX, 0);  // 3x position
    ogl::buffer::Vbo::AddAttribute(m_vboId, 1, 3, Tile::FLOATS_PER_VERTEX, 3);  // 3x normal
    ogl::buffer::Vbo::AddAttribute(m_vboId, 2, 3, Tile::FLOATS_PER_VERTEX, 6);  // 3x color
    ogl::buffer::Vbo::AddAttribute(m_vboId, 3, 1, Tile::FLOATS_PER_VERTEX, 9);  // 1x texture
    ogl::buffer::Vbo::AddAttribute(m_vboId, 4, 2, Tile::FLOATS_PER_VERTEX, 10); // 2x uv
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

void sg::city::map::Map::UpdateMapVboByTileIndex(const int t_tileIndex) const
{
    ogl::buffer::Vbo::BindVbo(m_vboId);
    glBufferSubData(GL_ARRAY_BUFFER, t_tileIndex * Tile::SIZE_IN_BYTES_PER_TILE, Tile::SIZE_IN_BYTES_PER_TILE, m_tiles[t_tileIndex]->GetVerticesContainer().data());
    ogl::buffer::Vbo::UnbindVbo();
}

void sg::city::map::Map::UpdateMapVboByPosition(const glm::vec3& t_mapPoint) const
{
    UpdateMapVboByTileIndex(GetTileIndexByPosition(t_mapPoint));
}

//-------------------------------------------------
// Regions
//-------------------------------------------------

void sg::city::map::Map::DepthSearch(Tile& t_startTile, const int t_region)
{
    if (t_startTile.GetRegion() != NO_REGION)
    {
        return;
    }

    auto found{ false };

    for (auto tileType : m_tileTypes)
    {
        if (tileType == t_startTile.GetType())
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        return;
    }

    t_startTile.SetRegion(t_region);

    // changing the color needs also a Vbo update
    t_startTile.SetColor(static_cast<glm::vec3>(m_randomColors[t_region - 1]));
    UpdateMapVboByTileIndex(GetTileIndexByPosition(static_cast<int>(t_startTile.GetMapX()), static_cast<int>(t_startTile.GetMapZ())));

    for (auto* neighbour : t_startTile.GetNeighbours())
    {
        if (neighbour)
        {
            DepthSearch(*neighbour, t_region);
        }
    }
}
