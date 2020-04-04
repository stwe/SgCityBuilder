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

//-------------------------------------------------
// Logic
//-------------------------------------------------

void sg::city::map::tile::RoadTile::Update()
{
    Tile::Update();

    SG_OGL_LOG_INFO("[RoadTile::Update()] Start RoadTile update process.");

    // update the RoadType (orientation of the road)
    DetermineRoadType();

    // clear Auto Tracks and Stop Patterns
    Clear();

    // recreate the Auto Tracks
    CreateAutoTracks();

    // recreate the Stop Patterns
    // todo: nur einmal erstellen
    CreateStopPatterns();

    // Apply StopPatterns to Nodes
    // todo: Pattern regelmaessig aendern
    ApplyStopPattern(0);

    // recreate the Auto Tracks Mesh
    CreateAutoTracksMesh();

    // recreate the Navigation Nodes Mesh
    CreateNavigationNodesMesh();
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

//-------------------------------------------------
// Regulate traffic
//-------------------------------------------------

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

void sg::city::map::tile::RoadTile::DetermineRoadType()
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

    roadType = newRoadType;
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
