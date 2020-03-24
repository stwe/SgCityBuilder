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
#include <Core.h>
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
    SG_OGL_CORE_ASSERT(t_scene, "[Map::Map()] Null pointer.")

    SG_OGL_LOG_DEBUG("[Map::Map()] Construct Map.");
}

sg::city::map::Map::~Map() noexcept
{
    SG_OGL_LOG_DEBUG("[Map::Map()] Destruct Map.");
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

const sg::city::map::Map::TileContainer& sg::city::map::Map::GetTiles() const noexcept
{
    return m_tiles;
}

sg::city::map::Map::TileContainer& sg::city::map::Map::GetTiles() noexcept
{
    return m_tiles;
}

const sg::city::map::Map::NavigationNodeContainer& sg::city::map::Map::GetNavigationNodes() const noexcept
{
    return m_navigationNodes;
}

sg::city::map::Map::NavigationNodeContainer& sg::city::map::Map::GetNavigationNodes() noexcept
{
    return m_navigationNodes;
}

const sg::ogl::resource::Mesh& sg::city::map::Map::GetMapMesh() const noexcept
{
    return *m_mapMesh;
}

sg::ogl::resource::Mesh& sg::city::map::Map::GetMapMesh() noexcept
{
    return *m_mapMesh;
}

int sg::city::map::Map::GetNumRegions() const
{
    return m_numRegions;
}

uint32_t sg::city::map::Map::GetFloatCountOfMap() const
{
    return static_cast<uint32_t>(m_tiles.size()) * Tile::FLOATS_PER_TILE;
}

//-------------------------------------------------
// Get Tile
//-------------------------------------------------

const sg::city::map::Tile& sg::city::map::Map::GetTileByIndex(const int t_tileIndex) const noexcept
{
    return *m_tiles[t_tileIndex];
}

sg::city::map::Tile& sg::city::map::Map::GetTileByIndex(const int t_tileIndex) noexcept
{
    return *m_tiles[t_tileIndex];
}

const sg::city::map::Tile& sg::city::map::Map::GetTileByMapPosition(const int t_mapX, const int t_mapZ) const noexcept
{
    return *m_tiles[GetTileMapIndexByMapPosition(t_mapX, t_mapZ)];
}

sg::city::map::Tile& sg::city::map::Map::GetTileByMapPosition(const int t_mapX, const int t_mapZ) noexcept
{
    return *m_tiles[GetTileMapIndexByMapPosition(t_mapX, t_mapZ)];
}

int sg::city::map::Map::GetTileMapIndexByMapPosition(const int t_mapX, const int t_mapZ) const
{
    return t_mapZ * m_mapSize + t_mapX;
}

//-------------------------------------------------
// Create Map
//-------------------------------------------------

void sg::city::map::Map::CreateMap(const int t_mapSize)
{
    m_mapSize = t_mapSize;

    StoreTileTypeTextures();
    CreateTiles();
    StoreTileNeighbours();
    CreateRandomColors();
    CreateNavigationNodes();
    //LinkTilesForNavigation();

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

void sg::city::map::Map::ChangeTileTypeOnMapPosition(const int t_mapX, const int t_mapZ, const TileType t_tileType)
{
    const auto tileIndex{ GetTileMapIndexByMapPosition(t_mapX, t_mapZ) };

    auto& tile{ *m_tiles[tileIndex] };

    if (tile.GetType() != t_tileType)
    {
        tile.SetType(t_tileType);
        UpdateMapVboByTileIndex(tileIndex);
    }
}

//-------------------------------------------------
// Regions
//-------------------------------------------------

/*
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
*/

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

    m_tileTypeTextures.emplace(TileType::NONE, n);
    m_tileTypeTextures.emplace(TileType::RESIDENTIAL, r);
    m_tileTypeTextures.emplace(TileType::COMMERCIAL, c);
    m_tileTypeTextures.emplace(TileType::INDUSTRIAL, i);
    m_tileTypeTextures.emplace(TileType::TRAFFIC_NETWORK, t);
}

void sg::city::map::Map::CreateTiles()
{
    SG_OGL_LOG_DEBUG("[Map::CreateTiles()] Create {}x{} Tiles for the map.", m_mapSize, m_mapSize);

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

            // converting unique_ptr to shared_ptr
            m_tiles.push_back(std::move(tile));
        }
    }
}

void sg::city::map::Map::StoreTileNeighbours()
{
    SG_OGL_CORE_ASSERT(!m_tiles.empty(), "[Map::StoreTileNeighbours()] No Tiles available.")

    SG_OGL_LOG_DEBUG("[Map::StoreTileNeighbours()] Store the neighbors to each Tile.");

    for (auto z{ 0 }; z < m_mapSize; ++z)
    {
        for (auto x{ 0 }; x < m_mapSize; ++x)
        {
            const auto tileIndex{ GetTileMapIndexByMapPosition(x, z) };

            // North since we render in -z direction ( mapSize = 6    :    z < 5 )
            if (z < m_mapSize - 1)
            {
                m_tiles[tileIndex]->GetNeighbours().emplace(Tile::Direction::NORTH, m_tiles[GetTileMapIndexByMapPosition(x, z + 1)]);
            }

            if (x < m_mapSize - 1)
            {
                m_tiles[tileIndex]->GetNeighbours().emplace(Tile::Direction::WEST, m_tiles[GetTileMapIndexByMapPosition(x + 1, z)]);
            }

            if (z > 0)
            {
                m_tiles[tileIndex]->GetNeighbours().emplace(Tile::Direction::SOUTH, m_tiles[GetTileMapIndexByMapPosition(x, z - 1)]);
            }

            if (x > 0)
            {
                m_tiles[tileIndex]->GetNeighbours().emplace(Tile::Direction::EAST, m_tiles[GetTileMapIndexByMapPosition(x - 1, z)]);
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

                auto navigationNode{ std::make_shared<automata::AutoNode>(glm::vec3(tile->GetWorldX() + xOffset, 0.0f, tile->GetWorldZ() + zOffset)) };

                // store the node as Tile node
                tile->GetNavigationNodes().push_back(navigationNode);

                // additionally store the node in a container with all nodes
                m_navigationNodes.push_back(navigationNode);
            }
        }
    }
}

/*
void sg::city::map::Map::LinkTilesForNavigation()
{
    for (auto& tile : m_tiles)
    {
        auto& navigationNodes{ tile->GetNavigationNodes() };

        // north
        if (static_cast<int>(tile->GetMapZ()) > 0)
        {
            for (auto i{ 0 }; i < 7; ++i)
            {
                //  0  1  2  3  4  5  6
                // 42 43 44 45 46 47 48
                
                // todo: use Bottom left instead map coords
                const auto idx{ GetTileIndexByPosition(static_cast<int>(tile->GetMapX()), static_cast<int>(tile->GetMapZ()) - 1) * 49 };
                navigationNodes[i] = m_navigationNodes[idx + 42 + i];
            }
        }
        else
        {
            for (auto i{ 0 }; i < 7; ++i)
            {
                navigationNodes[i].reset();
            }
        }

        // west
        if (static_cast<int>(tile->GetMapX()) > 0)
        {
            for (auto i{ 6 }; i >= 0; --i)
            {
                //
                //48  42
                //41  35
                //34  28
                //27  21
                //20  14
                //13   7
                 //6   0
                //

                const auto idx{ GetTileIndexByPosition(static_cast<int>(tile->GetMapX() - 1), static_cast<int>(tile->GetMapZ())) * 49 };
                navigationNodes[i * 7] = m_navigationNodes[idx + 6 + i * 7];
            }
        }
        else
        {
            for (auto i{ 6 }; i >= 0; --i)
            {
                navigationNodes[i * 7].reset();
            }
        }

        // south
        if (static_cast<int>(tile->GetMapZ()) < m_mapSize - 1)
        {
        }
        else
        {
            for (auto i{ 0 }; i < 7; ++i)
            {
                navigationNodes[42 + i].reset();
            }
        }

        // east
        if (static_cast<int>(tile->GetMapX()) < m_mapSize - 1)
        {
        }
        else
        {
            for (auto i{ 6 }; i >= 0; --i)
            {
                navigationNodes[6 + i * 7].reset();
            }
        }

        navigationNodes[37].reset();
        navigationNodes[39].reset();
        navigationNodes[29].reset();
        navigationNodes[33].reset();
        navigationNodes[15].reset();
        navigationNodes[19].reset();
        navigationNodes[9].reset();
        navigationNodes[11].reset();
        navigationNodes[42].reset();
        navigationNodes[48].reset();
        navigationNodes[0].reset();
        navigationNodes[6].reset();
    }
}
*/

//-------------------------------------------------
// Helper
//-------------------------------------------------

int32_t sg::city::map::Map::GetVerticesCountOfMap() const
{
    return static_cast<int32_t>(m_tiles.size()) * Tile::VERTICES_PER_TILE;
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

//-------------------------------------------------
// Regions
//-------------------------------------------------

/*
void sg::city::map::Map::DepthSearch(Tile& t_startTile, const int t_region)
{
    if (t_startTile.GetRegion() != NO_REGION)
    {
        return;
    }

    auto found{ false };

    for (auto tileType : REGION_TILE_TYPES)
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
    // todo: use Bottom left instead map coords ???????
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
*/
