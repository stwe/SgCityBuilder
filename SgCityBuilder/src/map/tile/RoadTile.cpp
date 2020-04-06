// This file is part of the SgCityBuilder package.
// 
// Filename: RoadTile.cpp
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#include <Application.h>
#include <Window.h>
#include <camera/Camera.h>
#include <resource/Mesh.h>
#include <resource/ShaderManager.h>
#include <scene/Scene.h>
#include <math/Transform.h>
#include "RoadTile.h"
#include "map/Map.h"
#include "automata/AutoNode.h"
#include "automata/AutoTrack.h"
#include "shader/LineShader.h"
#include "shader/NodeShader.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

sg::city::map::tile::RoadTile::RoadTile(const float t_mapX, const float t_mapZ, const TileType t_type, Map* t_map)
    : Tile(t_mapX, t_mapZ, t_type, t_map)
{
    SG_OGL_CORE_ASSERT(t_type == TileType::TRAFFIC, "[RoadTile::RoadTile()] Invalid Tile Type.")
}

sg::city::map::tile::RoadTile::~RoadTile() noexcept
{
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

const sg::city::map::tile::RoadTile::NavigationNodeContainer& sg::city::map::tile::RoadTile::GetNavigationNodes() const noexcept
{
    return m_navigationNodes;
}

sg::city::map::tile::RoadTile::NavigationNodeContainer& sg::city::map::tile::RoadTile::GetNavigationNodes() noexcept
{
    return m_navigationNodes;
}

const sg::city::map::tile::RoadTile::AutoTrackContainer& sg::city::map::tile::RoadTile::GetAutoTracks() const noexcept
{
    return m_autoTracks;
}

sg::city::map::tile::RoadTile::AutoTrackContainer& sg::city::map::tile::RoadTile::GetAutoTracks() noexcept
{
    return m_autoTracks;
}

const sg::city::map::tile::RoadTile::StopPatternContainer& sg::city::map::tile::RoadTile::GetStopPatterns() const noexcept
{
    return m_stopPatterns;
}

sg::city::map::tile::RoadTile::StopPatternContainer& sg::city::map::tile::RoadTile::GetStopPatterns() noexcept
{
    return m_stopPatterns;
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void sg::city::map::tile::RoadTile::Update()
{
    SG_OGL_LOG_INFO("[RoadTile::Update()] Start RoadTile update process.");

    // update the RoadType (orientation of the road)
    const auto typeHasChanged{ DetermineRoadType() };

    LinkTileNavigationNodes();
    Clear();
    CreateAutoTracks();

    if (typeHasChanged)
    {
        CreateStopPatterns();
        ApplyStopPattern(0);

        CreateAutoTracksMesh();
        CreateNavigationNodesMesh();
    }
}

//-------------------------------------------------
// Debug
//-------------------------------------------------

void sg::city::map::tile::RoadTile::CreateNavigationNodesMesh()
{
    SG_OGL_CORE_ASSERT(!m_navigationNodes.empty(), "[RoadTile::CreateNavigationNodesMesh()] No Navigation Nodes available.")

    VertexContainer vertexContainer;

    for (auto& node : m_navigationNodes)
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

void sg::city::map::tile::RoadTile::RenderNavigationNodes() const
{
    SG_OGL_CORE_ASSERT(m_navigationNodesMesh, "[RoadTile::RenderNavigationNodes()] Null pointer.")

    ogl::math::Transform t;
    t.position = m_map->position;
    t.rotation = m_map->rotation;
    t.scale = m_map->scale;

    auto& shader{ m_map->GetScene()->GetApplicationContext()->GetShaderManager().GetShaderProgram<shader::NodeShader>() };
    shader.Bind();

    const auto projectionMatrix{ m_map->GetScene()->GetApplicationContext()->GetWindow().GetProjectionMatrix() };
    const auto mvp{ projectionMatrix * m_map->GetScene()->GetCurrentCamera().GetViewMatrix() * static_cast<glm::mat4>(t) };

    shader.SetUniform("mvpMatrix", mvp);

    glPointSize(POINT_SIZE);

    m_navigationNodesMesh->InitDraw();
    m_navigationNodesMesh->DrawPrimitives(GL_POINTS);
    m_navigationNodesMesh->EndDraw();

    ogl::resource::ShaderProgram::Unbind();
}

void sg::city::map::tile::RoadTile::CreateAutoTracksMesh()
{
    SG_OGL_CORE_ASSERT(!m_autoTracks.empty(), "[RoadTile::CreateAutoTracksMesh()] No Auto Tracks available.")

    VertexContainer vertexContainer;

    for (auto& autoTrack : m_autoTracks)
    {
        // start
        vertexContainer.push_back(autoTrack->startNode->position.x);
        vertexContainer.push_back(VERTEX_HEIGHT);
        vertexContainer.push_back(autoTrack->startNode->position.z);

        // color
        vertexContainer.push_back(0.0f);
        vertexContainer.push_back(0.0f);
        vertexContainer.push_back(1.0f);

        // end
        vertexContainer.push_back(autoTrack->endNode->position.x);
        vertexContainer.push_back(VERTEX_HEIGHT);
        vertexContainer.push_back(autoTrack->endNode->position.z);

        // color
        vertexContainer.push_back(0.0f);
        vertexContainer.push_back(0.0f);
        vertexContainer.push_back(1.0f);
    }

    if (m_autoTracksMesh)
    {
        m_autoTracksMesh.reset();
    }

    m_autoTracksMesh = std::make_unique<ogl::resource::Mesh>();

    const ogl::buffer::BufferLayout bufferLayout{
        { ogl::buffer::VertexAttributeType::POSITION, "aPosition" },
        { ogl::buffer::VertexAttributeType::COLOR, "aColor" },
    };

    m_autoTracksMesh->GetVao().AddVertexDataVbo(vertexContainer.data(), static_cast<int32_t>(m_autoTracks.size()) * 2, bufferLayout);
}

void sg::city::map::tile::RoadTile::RenderAutoTracks() const
{
    SG_OGL_CORE_ASSERT(m_autoTracksMesh, "[RoadTile::RenderAutoTracks()] Null pointer.")

    ogl::math::Transform t;
    t.position = m_map->position;
    t.rotation = m_map->rotation;
    t.scale = m_map->scale;

    auto& shader{ m_map->GetScene()->GetApplicationContext()->GetShaderManager().GetShaderProgram<shader::LineShader>() };
    shader.Bind();

    const auto projectionMatrix{ m_map->GetScene()->GetApplicationContext()->GetWindow().GetProjectionMatrix() };
    const auto mvp{ projectionMatrix * m_map->GetScene()->GetCurrentCamera().GetViewMatrix() * static_cast<glm::mat4>(t) };

    shader.SetUniform("mvpMatrix", mvp);

    m_autoTracksMesh->InitDraw();
    m_autoTracksMesh->DrawPrimitives(GL_LINES);
    m_autoTracksMesh->EndDraw();

    ogl::resource::ShaderProgram::Unbind();
}

void sg::city::map::tile::RoadTile::Init()
{
    SG_OGL_CORE_ASSERT(!m_neighbours.empty(), "[RoadTile::Init()] No neighbours available. Call this function after creating the Tile.")

    SG_OGL_CORE_ASSERT(m_navigationNodes.empty(), "[RoadTile::Init()] Call this function after creating the Tile.")
    SG_OGL_CORE_ASSERT(m_autoTracks.empty(), "[RoadTile::Init()] Call this function after creating the Tile.")
    SG_OGL_CORE_ASSERT(m_stopPatterns.empty(), "[RoadTile::Init()] Call this function after creating the Tile.")

    // Tile

    // we use the TileType number as value for the texture
    SetTexture(static_cast<float>(type));

    // we use the TileType color as Tile color
    SetColor(TILE_TYPE_COLOR.at(type));

    // update Vbo
    m_map->UpdateMapVboByTileIndex(GetMapIndex());


    // RoadTile

    // set the RoadType (orientation of the road)
    DetermineRoadType();

    // create and link nodes
    CreateNavigationNodes();
    LinkTileNavigationNodes();

    // create Auto Tracks
    CreateAutoTracks();

    // create and apply Stop Pattern
    CreateStopPatterns();
    ApplyStopPattern(0);

    // create meshes for debug
    CreateAutoTracksMesh();
    CreateNavigationNodesMesh();
}

//-------------------------------------------------
// Regulate traffic
//-------------------------------------------------

void sg::city::map::tile::RoadTile::CreateNavigationNodes()
{
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
            m_navigationNodes.push_back(std::make_unique<automata::AutoNode>(GetMapIndex(), glm::vec3(
                GetWorldX() + xOffset,
                0.0f,
                GetWorldZ() + zOffset)
                )
            );
        }
    }
}

void sg::city::map::tile::RoadTile::LinkTileNavigationNodes()
{
    if (GetMapZ() < m_map->GetMapSize() - 1)
    {
        const auto northNeighbourIndex{ m_neighbours.at(Direction::NORTH) };

        if (m_map->GetTiles()[northNeighbourIndex]->type == TileType::TRAFFIC)
        {
            auto* northTile{ dynamic_cast<RoadTile*>(m_map->GetTiles()[northNeighbourIndex].get()) };
            SG_OGL_CORE_ASSERT(northTile, "[RoadTile::LinkTileNavigationNodes()] Null pointer.")

            const auto currentIndex{ GetMapIndex() };

            SG_OGL_LOG_INFO("Link Tile {} NORTH with Tile {} SOUTH", currentIndex, northNeighbourIndex);

            m_navigationNodes[42] = northTile->GetNavigationNodes()[0];
            m_navigationNodes[43] = northTile->GetNavigationNodes()[1];
            m_navigationNodes[44] = northTile->GetNavigationNodes()[2];
            m_navigationNodes[45] = northTile->GetNavigationNodes()[3];
            m_navigationNodes[46] = northTile->GetNavigationNodes()[4];
            m_navigationNodes[47] = northTile->GetNavigationNodes()[5];
            m_navigationNodes[48] = northTile->GetNavigationNodes()[6];

            if (m_navigationNodes[42])
            {
                m_navigationNodes[42]->tiles.clear();
                m_navigationNodes[42]->tiles.push_back(currentIndex);
                m_navigationNodes[42]->tiles.push_back(northNeighbourIndex);
            }

            if (m_navigationNodes[43])
            {
                m_navigationNodes[43]->tiles.clear();
                m_navigationNodes[43]->tiles.push_back(currentIndex);
                m_navigationNodes[43]->tiles.push_back(northNeighbourIndex);
            }

            if (m_navigationNodes[44])
            {
                m_navigationNodes[44]->tiles.clear();
                m_navigationNodes[44]->tiles.push_back(currentIndex);
                m_navigationNodes[44]->tiles.push_back(northNeighbourIndex);
            }

            if (m_navigationNodes[45])
            {
                m_navigationNodes[45]->tiles.clear();
                m_navigationNodes[45]->tiles.push_back(currentIndex);
                m_navigationNodes[45]->tiles.push_back(northNeighbourIndex);
            }

            if (m_navigationNodes[46])
            {
                m_navigationNodes[46]->tiles.clear();
                m_navigationNodes[46]->tiles.push_back(currentIndex);
                m_navigationNodes[46]->tiles.push_back(northNeighbourIndex);
            }

            if (m_navigationNodes[47])
            {
                m_navigationNodes[47]->tiles.clear();
                m_navigationNodes[47]->tiles.push_back(currentIndex);
                m_navigationNodes[47]->tiles.push_back(northNeighbourIndex);
            }

            if (m_navigationNodes[48])
            {
                m_navigationNodes[48]->tiles.clear();
                m_navigationNodes[48]->tiles.push_back(currentIndex);
                m_navigationNodes[48]->tiles.push_back(northNeighbourIndex);
            }
        }
    }

    if (GetMapX() < m_map->GetMapSize() - 1)
    {
        const auto eastIndex{ m_neighbours.at(Direction::EAST) };

        if (m_map->GetTiles()[eastIndex]->type == TileType::TRAFFIC)
        {
            auto* eastTile{ dynamic_cast<RoadTile*>(m_map->GetTiles()[eastIndex].get()) };
            SG_OGL_CORE_ASSERT(eastTile, "[RoadTile::LinkTileNavigationNodes()] Null pointer.")

            m_navigationNodes[48] = eastTile->GetNavigationNodes()[42];
            m_navigationNodes[41] = eastTile->GetNavigationNodes()[35];
            m_navigationNodes[34] = eastTile->GetNavigationNodes()[28];
            m_navigationNodes[27] = eastTile->GetNavigationNodes()[21];
            m_navigationNodes[20] = eastTile->GetNavigationNodes()[14];
            m_navigationNodes[13] = eastTile->GetNavigationNodes()[7];
            m_navigationNodes[6] = eastTile->GetNavigationNodes()[0];
        }
    }

    m_navigationNodes[37].reset();
    m_navigationNodes[39].reset();
    m_navigationNodes[29].reset();
    m_navigationNodes[33].reset();
    m_navigationNodes[15].reset();
    m_navigationNodes[19].reset();
    m_navigationNodes[9].reset();
    m_navigationNodes[11].reset();

    // the 4 corners
    m_navigationNodes[42].reset();
    m_navigationNodes[48].reset();
    m_navigationNodes[0].reset();
    m_navigationNodes[6].reset();
}

void sg::city::map::tile::RoadTile::CreateAutoTracks()
{
    switch (roadType)
    {
    case RoadType::ROAD_H:
        AddAutoTrack(34, 28);
        AddAutoTrack(14, 20, true);
        break;
    case RoadType::ROAD_V:
        AddAutoTrack(44, 2, true);
        AddAutoTrack(4, 46);
        break;

        /*
         *  --------->
         * |
         * |
         * |
         */
    case RoadType::ROAD_C1:
        AddAutoTrack(34, 30);
        AddAutoTrack(30, 2);
        AddAutoTrack(4, 18);
        AddAutoTrack(18, 20);
        break;

        /*
         *  <---------
         *            |
         *            |
         *            |
         */
    case RoadType::ROAD_C2:
        AddAutoTrack(4, 32);
        AddAutoTrack(32, 28);
        AddAutoTrack(14, 16);
        AddAutoTrack(16, 2);
        break;

        /*
         * |
         * |
         * |
         *  --------->
         */
    case RoadType::ROAD_C3:
        AddAutoTrack(44, 16);
        AddAutoTrack(16, 20);
        AddAutoTrack(34, 32);
        AddAutoTrack(32, 46);
        break;

        /*
         *           |
         *           |
         *           |
         * <---------
         */
    case RoadType::ROAD_C4:
        AddAutoTrack(44, 30);
        AddAutoTrack(30, 28);
        AddAutoTrack(14, 18);
        AddAutoTrack(18, 46);
        break;

        /*
         *  <--------->
         *       |
         *       |
         *       |
         */
    case RoadType::ROAD_T1:
        AddAutoTrack(34, 32);
        AddAutoTrack(32, 30);
        AddAutoTrack(30, 28);

        AddAutoTrack(14, 16);
        AddAutoTrack(16, 18);
        AddAutoTrack(18, 20);

        AddAutoTrack(30, 16);
        AddAutoTrack(16, 2);

        AddAutoTrack(4, 18);
        AddAutoTrack(18, 32);
        break;

        /*
         *       |
         *       |
         *       |
         *  <--------->
         */
    case RoadType::ROAD_T4:
        AddAutoTrack(34, 32);
        AddAutoTrack(32, 30);
        AddAutoTrack(30, 28);

        AddAutoTrack(14, 16);
        AddAutoTrack(16, 18);
        AddAutoTrack(18, 20);

        AddAutoTrack(44, 30);
        AddAutoTrack(30, 16);

        AddAutoTrack(18, 32);
        AddAutoTrack(32, 46);
        break;

        /*
         * |
         * |________
         * |
         * |
         */
    case RoadType::ROAD_T2:
        AddAutoTrack(44, 30);
        AddAutoTrack(30, 16);
        AddAutoTrack(16, 2);

        AddAutoTrack(4, 18);
        AddAutoTrack(18, 32);
        AddAutoTrack(32, 46);

        AddAutoTrack(34, 32);
        AddAutoTrack(32, 30);

        AddAutoTrack(16, 18);
        AddAutoTrack(18, 20);
        break;

        /*
         *         |
         * ________|
         *         |
         *         |
         */
    case RoadType::ROAD_T3:
        AddAutoTrack(44, 30);
        AddAutoTrack(30, 16);
        AddAutoTrack(16, 2);

        AddAutoTrack(4, 18);
        AddAutoTrack(18, 32);
        AddAutoTrack(32, 46);

        AddAutoTrack(32, 30);
        AddAutoTrack(30, 28);

        AddAutoTrack(14, 16);
        AddAutoTrack(16, 18);
        break;

    case RoadType::ROAD_X:
        AddAutoTrack(44, 30);
        AddAutoTrack(30, 16);
        AddAutoTrack(16, 2);

        AddAutoTrack(4, 18);
        AddAutoTrack(18, 32);
        AddAutoTrack(32, 46);

        AddAutoTrack(34, 32);
        AddAutoTrack(32, 30);
        AddAutoTrack(30, 28);

        AddAutoTrack(14, 16);
        AddAutoTrack(16, 18);
        AddAutoTrack(18, 20);
        break;

    default:;
    }
}

void sg::city::map::tile::RoadTile::CreateStopPatterns()
{
    switch (roadType)
    {
    case RoadType::ROAD_H:
    case RoadType::ROAD_V:
    case RoadType::ROAD_C1:
    case RoadType::ROAD_C2:
    case RoadType::ROAD_C3:
    case RoadType::ROAD_C4:
        break; // Allow all

/*
    m_stopPatterns.push_back(
        CreateStopPattern(
               ". . X . X . ."
               ". . . X . . ."
               "X . X . X . X"
               ". X . . . X ."
               "X . X . X . X"
               ". . . X . . ."
               ". . X . X . ."));
*/

    case RoadType::ROAD_X:
        // Allow West Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . X . O . ."
                ". . . X . . ."
                "X . X . O . X"
                ". X . . . X ."
                "O . O . O . O"
                ". . . X . . ."
                ". . O . X . ."));

        // Stop West Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . X . O . ."
                ". . . X . . ."
                "X . X . O . X"
                ". X . . . X ."
                "X . O . O . O"
                ". . . X . . ."
                ". . O . X . ."));

        // Stop North Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . O . X . ."
                ". . . X . . ."
                "O . O . X . X"
                ". X . . . X ."
                "X . O . O . O"
                ". . . X . . ."
                ". . O . X . ."));

        // Stop North Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . X . X . ."
                ". . . X . . ."
                "O . O . X . X"
                ". X . . . X ."
                "X . O . O . O"
                ". . . X . . ."
                ". . O . X . ."));

        // Allow East Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . X . O . ."
                ". . . X . . ."
                "O . O . O . O"
                ". X . . . X ."
                "X . O . X . X"
                ". . . X . . ."
                ". . O . X . ."));

        // Stop East Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . X . O . ."
                ". . . X . . ."
                "O . O . O . X"
                ". X . . . X ."
                "X . O . X . X"
                ". . . X . . ."
                ". . O . X . ."));

        // Allow South Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . X . O . ."
                ". . . X . . ."
                "O . O . O . X"
                ". X . . . X ."
                "X . X . O . O"
                ". . . X . . ."
                ". . X . O . ."));

        // Allow South Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . X . O . ."
                ". . . X . . ."
                "O . O . O . X"
                ". X . . . X ."
                "X . X . O . O"
                ". . . X . . ."
                ". . X . X . ."));

        break;

    case RoadType::ROAD_T1:
        // Allow West Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . X . X . ."
                ". . . X . . ."
                "X . X . X . X"
                ". X . . . X ."
                "O . O . O . O"
                ". . . X . . ."
                ". . O . X . ."));

        // Stop West Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . X . X . ."
                ". . . X . . ."
                "X . X . X . X"
                ". X . . . X ."
                "X . O . O . O"
                ". . . X . . ."
                ". . O . X . ."));

        // Allow East Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . X . X . ."
                ". . . X . . ."
                "O . O . O . O"
                ". X . . . X ."
                "X . O . X . X"
                ". . . X . . ."
                ". . O . X . ."));

        // Stop East Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . X . X . ."
                ". . . X . . ."
                "O . O . O . X"
                ". X . . . X ."
                "X . O . X . X"
                ". . . X . . ."
                ". . O . X . ."));

        // Allow South Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . X . X . ."
                ". . . X . . ."
                "O . O . O . X"
                ". X . . . X ."
                "X . X . O . O"
                ". . . X . . ."
                ". . X . O . ."));

        // Stop South Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . X . X . ."
                ". . . X . . ."
                "O . O . O . X"
                ". X . . . X ."
                "X . X . O . O"
                ". . . X . . ."
                ". . X . X . ."));

        break;

    case RoadType::ROAD_T2:
        // Allow North Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . O . X . ."
                ". . . X . . ."
                "X . O . X . X"
                ". . . . . X ."
                "X . O . O . O"
                ". . . X . . ."
                ". . O . X . ."));

        // Stop North Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . X . X . ."
                ". . . X . . ."
                "X . O . X . X"
                ". . . . . X ."
                "X . O . O . O"
                ". . . X . . ."
                ". . O . X . ."));

        // Allow East Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . X . O . ."
                ". . . X . . ."
                "X . O . O . O"
                ". . . . . X ."
                "X . O . X . X"
                ". . . X . . ."
                ". . O . X . ."));

        // Stop East Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . X . O . ."
                ". . . X . . ."
                "X . O . O . X"
                ". . . . . X ."
                "X . O . X . X"
                ". . . X . . ."
                ". . O . X . ."));

        // Allow South Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . X . O . ."
                ". . . X . . ."
                "X . X . O . X"
                ". . . . . X ."
                "X . X . O . O"
                ". . . X . . ."
                ". . X . O . ."));

        // Stop South Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . X . O . ."
                ". . . X . . ."
                "X . X . O . X"
                ". . . . . X ."
                "X . X . O . O"
                ". . . X . . ."
                ". . X . X . ."));

        break;

    case RoadType::ROAD_T3:
        // Allow West Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . X . O . ."
                ". . . X . . ."
                "X . X . O . X"
                ". X . . . X ."
                "O . O . O . X"
                ". . . X . . ."
                ". . O . X . ."));

        // Stop West Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . X . O . ."
                ". . . X . . ."
                "X . X . O . X"
                ". X . . . X ."
                "X . O . O . X"
                ". . . X . . ."
                ". . O . X . ."));

        // Allow North Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . O . X . ."
                ". . . X . . ."
                "O . O . X . X"
                ". X . . . X ."
                "X . O . X . X"
                ". . . X . . ."
                ". . O . X . ."));

        // Stop North Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . X . X . ."
                ". . . X . . ."
                "O . O . X . X"
                ". X . . . X ."
                "X . O . X . X"
                ". . . X . . ."
                ". . O . X . ."));

        // Allow South Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . X . O . ."
                ". . . X . . ."
                "O . O . O . X"
                ". X . . . X ."
                "X . X . O . X"
                ". . . X . . ."
                ". . X . O . ."));

        // Stop South Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . X . O . ."
                ". . . X . . ."
                "O . O . O . X"
                ". X . . . X ."
                "X . X . O . X"
                ". . . X . . ."
                ". . X . X . ."));

        break;

    case RoadType::ROAD_T4:
        // Allow West Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . X . O . ."
                ". . . X . . ."
                "X . X . O . X"
                ". X . . . X ."
                "O . O . O . O"
                ". . . X . . ."
                ". . X . X . ."));

        // Stop West Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . X . O . ."
                ". . . X . . ."
                "X . X . O . X"
                ". X . . . X ."
                "X . O . O . O"
                ". . . X . . ."
                ". . X . X . ."));

        // Allow North Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . O . X . ."
                ". . . X . . ."
                "O . O . X . X"
                ". X . . . X ."
                "X . O . O . O"
                ". . . X . . ."
                ". . X . X . ."));

        // Stop North Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . X . X . ."
                ". . . X . . ."
                "O . O . X . X"
                ". X . . . X ."
                "X . O . O . O"
                ". . . X . . ."
                ". . X . X . ."));

        // Allow East Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . X . O . ."
                ". . . X . . ."
                "O . O . O . O"
                ". X . . . X ."
                "X . X . X . X"
                ". . . X . . ."
                ". . X . X . ."));

        // Stop East Traffic
        m_stopPatterns.push_back(
            CreateStopPattern(
                ". . X . O . ."
                ". . . X . . ."
                "O . O . O . X"
                ". X . . . X ."
                "X . X . X . X"
                ". . . X . . ."
                ". . X . X . ."));

        break;

    default:;
    }
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

bool sg::city::map::tile::RoadTile::DetermineRoadType()
{
    uint8_t roadNeighbours{ 0 };

    if (m_neighbours.count(Direction::NORTH))
    {
        auto& tile{ m_map->GetTiles()[m_neighbours.at(Direction::NORTH)] };
        if (tile->type == TileType::TRAFFIC)
        {
            roadNeighbours = NORTH;
        }
    }

    if (m_neighbours.count(Direction::EAST))
    {
        auto& tile{ m_map->GetTiles()[m_neighbours.at(Direction::EAST)] };
        if (tile->type == TileType::TRAFFIC)
        {
            roadNeighbours |= EAST;
        }
    }

    if (m_neighbours.count(Direction::SOUTH))
    {
        auto& tile{ m_map->GetTiles()[m_neighbours.at(Direction::SOUTH)] };
        if (tile->type == TileType::TRAFFIC)
        {
            roadNeighbours |= SOUTH;
        }
    }

    if (m_neighbours.count(Direction::WEST))
    {
        auto& tile{ m_map->GetTiles()[m_neighbours.at(Direction::WEST)] };
        if (tile->type == TileType::TRAFFIC)
        {
            roadNeighbours |= WEST;
        }
    }

    RoadType newRoadType;
    switch (roadNeighbours)
    {
    case 0:                                   // keine Nachbarn
    case 1: newRoadType = RoadType::ROAD_V;   // Norden
        break;
    case 2: newRoadType = RoadType::ROAD_H;   // Osten
        break;
    case 3: newRoadType = RoadType::ROAD_C3;  // Norden - Osten
        break;
    case 4:                                   // Sueden
    case 5: newRoadType = RoadType::ROAD_V;   // Sueden - Norden
        break;
    case 6: newRoadType = RoadType::ROAD_C1;  // Sueden - Osten
        break;
    case 7: newRoadType = RoadType::ROAD_T2;  // Norden - Osten - Sueden
        break;
    case 8: newRoadType = RoadType::ROAD_H;   // Westen
        break;
    case 9: newRoadType = RoadType::ROAD_C4;  // Westen - Norden
        break;
    case 10: newRoadType = RoadType::ROAD_H;  // Westen - Osten
        break;
    case 11: newRoadType = RoadType::ROAD_T4; // Westen - Osten - Norden
        break;
    case 12: newRoadType = RoadType::ROAD_C2; // Westen - Sueden
        break;
    case 13: newRoadType = RoadType::ROAD_T3; // Westen - Sueden - Norden
        break;
    case 14: newRoadType = RoadType::ROAD_T1; // Westen - Sueden - Osten
        break;
    case 15: newRoadType = RoadType::ROAD_X;
        break;
    default: newRoadType = RoadType::ROAD_V;
    }

    const auto oldRoadType{ roadType };
    roadType = newRoadType;

    return oldRoadType != newRoadType;
}

void sg::city::map::tile::RoadTile::Clear()
{
    // clear Auto Tracks from Tile
    m_autoTracks.clear();

    // clear Auto Tracks from Nodes
    for (auto& node : GetNavigationNodes())
    {
        if (node)
        {
            node->autoTracks.clear();
        }
    }

    // clear Stop Patterns from Tile
    m_stopPatterns.clear();
}

void sg::city::map::tile::RoadTile::AddAutoTrack(const int t_fromNodeIndex, const int t_toNodeIndex, const bool t_safeCarAutoTrack)
{
    SG_OGL_CORE_ASSERT(t_fromNodeIndex >= 0 && t_fromNodeIndex <= 48, "[RoadTile::AddAutoTrack()] Invalid From index.")
    SG_OGL_CORE_ASSERT(t_toNodeIndex >= 0 && t_toNodeIndex <= 48, "[RoadTile::AddAutoTrack()] Invalid To index.")
    SG_OGL_CORE_ASSERT(m_navigationNodes[t_fromNodeIndex] && m_navigationNodes[t_toNodeIndex], "[RoadTile::AddAutoTrack()] Null pointer.")

    auto& from{ m_navigationNodes[t_fromNodeIndex] };
    auto& to{ m_navigationNodes[t_toNodeIndex] };

    // generate a new auto track
    auto track{ std::make_shared<automata::AutoTrack>() };
    track->startNode = from;
    track->endNode = to;
    track->tile = this;
    track->trackLength = length(track->startNode->position - track->endNode->position);

    // inserting at end of Tile track list
    m_autoTracks.push_back(track);

    if (t_safeCarAutoTrack)
    {
        safeAutoTrack = track;
    }

    /*
    SG_OGL_LOG_INFO("---------------------");
    SG_OGL_LOG_INFO("| New Track created |");
    SG_OGL_LOG_INFO("---------------------");
    SG_OGL_LOG_INFO("Tileindex: {}", t_tileIndex);
    SG_OGL_LOG_INFO("Track address: {}", (void*)track.get());
    SG_OGL_LOG_INFO("Track is safe: {}", t_safeCarAutoTrack);
    SG_OGL_LOG_INFO("Track from {} --> to {}", t_fromNodeIndex, t_toNodeIndex);
    SG_OGL_LOG_INFO("Track length: {}", track->trackLength);
    SG_OGL_LOG_INFO("New number of Tracks in the Tile: {}", tile->GetAutoTracks().size());
    */

    // store the auto track in the nodes
    from->autoTracks.push_back(m_autoTracks.back());
    to->autoTracks.push_back(m_autoTracks.back());

    /*
    SG_OGL_LOG_INFO("");

    for (auto& trackf : from->autoTracks)
    {
        SG_OGL_LOG_INFO("Track address: {} is added to node: {}", (void*)trackf.get(), t_fromNodeIndex);

        SG_OGL_LOG_INFO("node From {}  - the track start at x: {}", t_fromNodeIndex, trackf->startNode->position.x);
        SG_OGL_LOG_INFO("node From {}  - the track start at z: {}", t_fromNodeIndex, trackf->startNode->position.z);

        SG_OGL_LOG_INFO("node From {}  - the track end at x: {}", t_fromNodeIndex, trackf->endNode->position.x);
        SG_OGL_LOG_INFO("node From {}  - the track end at z: {}", t_fromNodeIndex, trackf->endNode->position.z);

        SG_OGL_LOG_INFO("");
    }

    for (auto& trackt : to->autoTracks)
    {
        SG_OGL_LOG_INFO("Track address: {} is added to node: {}", (void*)trackt.get(), t_toNodeIndex);

        SG_OGL_LOG_INFO("node To {}  - track start x: {}", t_toNodeIndex, trackt->startNode->position.x);
        SG_OGL_LOG_INFO("node To {}  - track start z: {}", t_toNodeIndex, trackt->startNode->position.z);

        SG_OGL_LOG_INFO("node To {}  - track end x: {}", t_toNodeIndex, trackt->endNode->position.x);
        SG_OGL_LOG_INFO("node To {}  - track end z: {}", t_toNodeIndex, trackt->endNode->position.z);

        SG_OGL_LOG_INFO("");
    }

    SG_OGL_LOG_INFO("node From {} count tracks: {} ", t_fromNodeIndex, from->autoTracks.size());
    SG_OGL_LOG_INFO("node To {} count tracks: {} ", t_toNodeIndex, to->autoTracks.size());

    SG_OGL_LOG_INFO("");
    SG_OGL_LOG_INFO("");
    */
}

sg::city::map::tile::RoadTile::StopPattern sg::city::map::tile::RoadTile::CreateStopPattern(std::string t_s) const
{
    // spaces have been added for readability
    t_s.erase(std::remove(t_s.begin(), t_s.end(), ' '), t_s.end());

    SG_OGL_ASSERT(t_s.size() == NODES_PER_TILE, "[RoadTile::CreateStopPattern()] Invalid string size.");

    StopPattern pattern;
    pattern.resize(NODES_PER_TILE, false);

    auto i{ 0 };
    for (auto z{ 6 }; z >= 0; --z)
    {
        for (auto x{ 0 }; x < 7; ++x)
        {
            const auto index{ z * 7 + x };
            pattern[i] = t_s[index] == STOP;
            i++;
        }
    }

    return pattern;
}

void sg::city::map::tile::RoadTile::ApplyStopPattern(const int t_index)
{
    if (!m_stopPatterns.empty())
    {
        SG_OGL_ASSERT(t_index >= 0 && t_index < static_cast<int>(m_stopPatterns.size()), "[RoadTile::ApplyStopPattern()] Invalid index.");

        auto i{ 0 };
        for (auto& node : m_navigationNodes)
        {
            if (node)
            {
                node->block = m_stopPatterns[t_index][i];
            }

            i++;
        }
    }
}
