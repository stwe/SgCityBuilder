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

bool sg::city::map::tile::RoadTile::HasSafeTrack() const
{
    return roadType == RoadType::ROAD_V || roadType == RoadType::ROAD_H;
}

int sg::city::map::tile::RoadTile::GetCurrentStopPatternIndex() const
{
    return m_currentStopPatternIndex;
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void sg::city::map::tile::RoadTile::Update()
{
    SG_OGL_LOG_INFO("[RoadTile::Update()] Start RoadTile update process.");

    DetermineRoadType();
    CreateAutoTracks();
    CreateStopPatterns();

    // is exist a StopPattern, use the index 0 by default
    ApplyStopPattern(0);

    // for debug
    CreateAutoTracksMesh();
}

void sg::city::map::tile::RoadTile::ApplyStopPattern(const int t_index)
{
    if (!m_stopPatterns.empty())
    {
        SG_OGL_ASSERT(t_index >= 0 && t_index < static_cast<int>(m_stopPatterns.size()), "[RoadTile::ApplyStopPattern()] Invalid index.");

        auto i{ 0 };
        for (auto& node : m_map->GetNavigationNodes(GetMapIndex()))
        {
            if (node)
            {
                node->block = m_stopPatterns[t_index][i];
            }

            i++;
        }

        // store given index as current
        m_currentStopPatternIndex = t_index;

        // for debug
        m_map->CreateNavigationNodesMesh();
    }
}

//-------------------------------------------------
// Debug
//-------------------------------------------------

void sg::city::map::tile::RoadTile::CreateAutoTracksMesh()
{
    SG_OGL_CORE_ASSERT(!m_autoTracks.empty(), "[RoadTile::CreateAutoTracksMesh()] No Auto Tracks available.")

    VertexContainer vertexContainer;

    for (auto& autoTrack : m_autoTracks)
    {
        // start
        vertexContainer.push_back(autoTrack->startNode->position.x);
        vertexContainer.push_back(Map::VERTEX_HEIGHT);
        vertexContainer.push_back(autoTrack->startNode->position.z);

        // color
        vertexContainer.push_back(0.0f);
        vertexContainer.push_back(0.0f);
        vertexContainer.push_back(1.0f);

        // end
        vertexContainer.push_back(autoTrack->endNode->position.x);
        vertexContainer.push_back(Map::VERTEX_HEIGHT);
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

    // create AutoTracks
    CreateAutoTracks();

    // create StopPattern
    CreateStopPatterns();

    // create AutoTrack mesh for debug
    CreateAutoTracksMesh();
}

//-------------------------------------------------
// Clear
//-------------------------------------------------

void sg::city::map::tile::RoadTile::ClearTracksAndStops()
{
    // clear Auto Tracks from Tile
    m_autoTracks.clear();

    // clear Auto Tracks from Nodes
    for (auto& node : m_map->GetNavigationNodes(GetMapIndex()))
    {
        if (node)
        {
            node->autoTracks.clear();
        }
    }

    // clear Stop Patterns from Tile
    m_stopPatterns.clear();
}

//-------------------------------------------------
// Regulate traffic
//-------------------------------------------------

void sg::city::map::tile::RoadTile::CreateAutoTracks()
{
    switch (roadType)
    {
    case RoadType::ROAD_H:
        AddAutoTrack(34, 28, 0.0f);
        AddAutoTrack(14, 20, 180.0f, true);
        break;
    case RoadType::ROAD_V:
        AddAutoTrack(44, 2, 90.0f, true);
        AddAutoTrack(4, 46, 270.0f);
        break;

        /*
         *  --------->
         * |
         * |
         * |
         */
    case RoadType::ROAD_C1:
        AddAutoTrack(34, 30, 0.0f);
        AddAutoTrack(30, 2, 90.0f);
        AddAutoTrack(4, 18, 270.0f);
        AddAutoTrack(18, 20, 180.0f);
        break;

        /*
         *  <---------
         *            |
         *            |
         *            |
         */
    case RoadType::ROAD_C2:
        AddAutoTrack(4, 32, 270.0f);
        AddAutoTrack(32, 28, 0.0f);
        AddAutoTrack(14, 16, 180.0f);
        AddAutoTrack(16, 2, 90.0f);
        break;

        /*
         * |
         * |
         * |
         *  --------->
         */
    case RoadType::ROAD_C3:
        AddAutoTrack(44, 16, 90.0f);
        AddAutoTrack(16, 20, 180.0f);
        AddAutoTrack(34, 32, 0.0f);
        AddAutoTrack(32, 46, 270.0f);
        break;

        /*
         *           |
         *           |
         *           |
         * <---------
         */
    case RoadType::ROAD_C4:
        AddAutoTrack(44, 30, 90.0f);
        AddAutoTrack(30, 28, 0.0f);
        AddAutoTrack(14, 18, 180.0f);
        AddAutoTrack(18, 46, 270.0f);
        break;

        /*
         *  <--------->
         *       |
         *       |
         *       |
         */
    case RoadType::ROAD_T1:
        AddAutoTrack(34, 32, 0.0f);
        AddAutoTrack(32, 30, 0.0f);
        AddAutoTrack(30, 28, 0.0f);

        AddAutoTrack(14, 16, 180.0f);
        AddAutoTrack(16, 18, 180.0f);
        AddAutoTrack(18, 20, 180.0f);

        AddAutoTrack(30, 16, 90.0f);
        AddAutoTrack(16, 2, 90.0f);

        AddAutoTrack(4, 18, 270.0f);
        AddAutoTrack(18, 32, 270.0f);
        break;

        /*
         *       |
         *       |
         *       |
         *  <--------->
         */
    case RoadType::ROAD_T4:
        AddAutoTrack(34, 32, 0.0f);
        AddAutoTrack(32, 30, 0.0f);
        AddAutoTrack(30, 28, 0.0f);

        AddAutoTrack(14, 16, 180.0f);
        AddAutoTrack(16, 18, 180.0f);
        AddAutoTrack(18, 20, 180.0f);

        AddAutoTrack(44, 30, 90.0f);
        AddAutoTrack(30, 16, 90.0f);

        AddAutoTrack(18, 32, 270.0f);
        AddAutoTrack(32, 46, 270.0f);
        break;

        /*
         * |
         * |________
         * |
         * |
         */
    case RoadType::ROAD_T2:
        AddAutoTrack(44, 30, 90.0f);
        AddAutoTrack(30, 16, 90.0f);
        AddAutoTrack(16, 2, 90.0f);

        AddAutoTrack(4, 18, 270.0f);
        AddAutoTrack(18, 32, 270.0f);
        AddAutoTrack(32, 46, 270.0f);

        AddAutoTrack(34, 32, 0.0f);
        AddAutoTrack(32, 30, 0.0f);

        AddAutoTrack(16, 18, 180.0f);
        AddAutoTrack(18, 20, 180.0f);
        break;

        /*
         *         |
         * ________|
         *         |
         *         |
         */
    case RoadType::ROAD_T3:
        AddAutoTrack(44, 30, 90.0f);
        AddAutoTrack(30, 16, 90.0f);
        AddAutoTrack(16, 2, 90.0f);

        AddAutoTrack(4, 18, 270.0f);
        AddAutoTrack(18, 32, 270.0f);
        AddAutoTrack(32, 46, 270.0f);

        AddAutoTrack(32, 30, 0.0f);
        AddAutoTrack(30, 28, 0.0f);

        AddAutoTrack(14, 16, 180.0f);
        AddAutoTrack(16, 18, 180.0f);
        break;

    case RoadType::ROAD_X:
        AddAutoTrack(44, 30, 90.0f);
        AddAutoTrack(30, 16, 90.0f);
        AddAutoTrack(16, 2, 90.0f);

        AddAutoTrack(4, 18, 270.0f);
        AddAutoTrack(18, 32, 270.0f);
        AddAutoTrack(32, 46, 270.0f);

        AddAutoTrack(34, 32, 0.0f);
        AddAutoTrack(32, 30, 0.0f);
        AddAutoTrack(30, 28, 0.0f);

        AddAutoTrack(14, 16, 180.0f);
        AddAutoTrack(16, 18, 180.0f);
        AddAutoTrack(18, 20, 180.0f);
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

void sg::city::map::tile::RoadTile::AddAutoTrack(const int t_fromNodeIndex, const int t_toNodeIndex, const float t_rotation, const bool t_safeCarAutoTrack)
{
    SG_OGL_CORE_ASSERT(t_fromNodeIndex >= 0 && t_fromNodeIndex <= 48, "[RoadTile::AddAutoTrack()] Invalid From index.")
    SG_OGL_CORE_ASSERT(t_toNodeIndex >= 0 && t_toNodeIndex <= 48, "[RoadTile::AddAutoTrack()] Invalid To index.")

    auto& navigationNodes{ m_map->GetNavigationNodes(GetMapIndex()) };

    SG_OGL_CORE_ASSERT(navigationNodes[t_fromNodeIndex] && navigationNodes[t_toNodeIndex], "[RoadTile::AddAutoTrack()] Null pointer.")

    auto& from{ navigationNodes[t_fromNodeIndex] };
    auto& to{ navigationNodes[t_toNodeIndex] };

    // generate a new auto track
    auto track{ std::make_shared<automata::AutoTrack>() };
    track->startNode = from;
    track->endNode = to;
    track->tile = this;
    track->trackLength = length(track->startNode->position - track->endNode->position);
    track->isSafe = t_safeCarAutoTrack;
    track->rotation = t_rotation;

    // inserting at end of Tile track list
    m_autoTracks.push_back(track);

    // store the auto track in the nodes
    from->autoTracks.push_back(m_autoTracks.back());
    to->autoTracks.push_back(m_autoTracks.back());
}

sg::city::map::tile::RoadTile::StopPattern sg::city::map::tile::RoadTile::CreateStopPattern(std::string t_s) const
{
    // spaces have been added for readability
    t_s.erase(std::remove(t_s.begin(), t_s.end(), ' '), t_s.end());

    SG_OGL_ASSERT(t_s.size() == Map::NODES_PER_TILE, "[RoadTile::CreateStopPattern()] Invalid string size.");

    StopPattern pattern;
    pattern.resize(Map::NODES_PER_TILE, false);

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
