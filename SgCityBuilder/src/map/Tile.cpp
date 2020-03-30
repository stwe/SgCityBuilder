// This file is part of the SgCityBuilder package.
// 
// Filename: Tile.cpp
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
#include "Tile.h"
#include "automata/AutoNode.h"
#include "automata/AutoTrack.h"
#include "shader/NodeShader.h"
#include "shader/LineShader.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

sg::city::map::Tile::Tile(const float t_mapX, const float t_mapZ, const Map::TileType t_type)
    : m_mapX{ t_mapX }
    , m_mapZ{ t_mapZ }
    , m_type{ t_type }
{
    Create();
}

sg::city::map::Tile::~Tile() noexcept
{
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

const sg::city::map::Tile::VertexContainer& sg::city::map::Tile::GetVerticesContainer() const noexcept
{
    return m_vertices;
}

sg::city::map::Tile::VertexContainer& sg::city::map::Tile::GetVerticesContainer() noexcept
{
    return m_vertices;
}

int sg::city::map::Tile::GetMapX() const
{
    return static_cast<int>(m_mapX);
}

int sg::city::map::Tile::GetMapZ() const
{
    return static_cast<int>(m_mapZ);
}

float sg::city::map::Tile::GetWorldX() const
{
    return m_bottomLeft.x;
}

float sg::city::map::Tile::GetWorldZ() const
{
    return m_bottomLeft.z;
}

glm::vec3 sg::city::map::Tile::GetWorldPosition() const
{
    return m_bottomLeft;
}

glm::vec3 sg::city::map::Tile::GetWorldCenter() const
{
    return glm::vec3(
    (m_bottomLeft.x + m_bottomRight.x) * 0.5f,
    0.0f,
    (m_bottomLeft.z + m_topLeft.z) * 0.5f
    );
}

sg::city::map::Map::TileType sg::city::map::Tile::GetType() const
{
    return m_type;
}

const sg::city::map::Tile::NeighbourContainer& sg::city::map::Tile::GetNeighbours() const noexcept
{
    return m_neighbours;
}

sg::city::map::Tile::NeighbourContainer& sg::city::map::Tile::GetNeighbours() noexcept
{
    return m_neighbours;
}

int sg::city::map::Tile::GetRegion() const
{
    return m_region;
}

const sg::city::map::Tile::NavigationNodeContainer& sg::city::map::Tile::GetNavigationNodes() const noexcept
{
    return m_navigationNodes;
}

sg::city::map::Tile::NavigationNodeContainer& sg::city::map::Tile::GetNavigationNodes() noexcept
{
    return m_navigationNodes;
}

const sg::city::map::Tile::AutoTrackContainer& sg::city::map::Tile::GetAutoTracks() const noexcept
{
    return m_autoTracks;
}

sg::city::map::Tile::AutoTrackContainer& sg::city::map::Tile::GetAutoTracks() noexcept
{
    return m_autoTracks;
}

sg::city::map::Tile::StopPatternContainer& sg::city::map::Tile::GetStopPatterns() noexcept
{
    return m_stopPatterns;
}

//-------------------------------------------------
// Setter
//-------------------------------------------------

void sg::city::map::Tile::SetVertexPositions()
{
    m_vertices[BOTTOM_LEFT_POS_X_T1] = m_bottomLeft.x;
    m_vertices[BOTTOM_LEFT_POS_Y_T1] = m_bottomLeft.y;
    m_vertices[BOTTOM_LEFT_POS_Z_T1] = m_bottomLeft.z;

    m_vertices[BOTTOM_RIGHT_POS_X_T1] = m_bottomRight.x;
    m_vertices[BOTTOM_RIGHT_POS_Y_T1] = m_bottomRight.y;
    m_vertices[BOTTOM_RIGHT_POS_Z_T1] = m_bottomRight.z;

    m_vertices[TOP_LEFT_POS_X_T1] = m_topLeft.x;
    m_vertices[TOP_LEFT_POS_Y_T1] = m_topLeft.y;
    m_vertices[TOP_LEFT_POS_Z_T1] = m_topLeft.z;

    m_vertices[TOP_LEFT_POS_X_T2] = m_topLeft.x;
    m_vertices[TOP_LEFT_POS_Y_T2] = m_topLeft.y;
    m_vertices[TOP_LEFT_POS_Z_T2] = m_topLeft.z;

    m_vertices[BOTTOM_RIGHT_POS_X_T2] = m_bottomRight.x;
    m_vertices[BOTTOM_RIGHT_POS_Y_T2] = m_bottomRight.y;
    m_vertices[BOTTOM_RIGHT_POS_Z_T2] = m_bottomRight.z;

    m_vertices[TOP_RIGHT_POS_X_T2] = m_topRight.x;
    m_vertices[TOP_RIGHT_POS_Y_T2] = m_topRight.y;
    m_vertices[TOP_RIGHT_POS_Z_T2] = m_topRight.z;
}

void sg::city::map::Tile::SetNormal(const glm::vec3& t_normal)
{
    m_vertices[BOTTOM_LEFT_NORMAL_X_T1] = t_normal.x;
    m_vertices[BOTTOM_LEFT_NORMAL_Y_T1] = t_normal.y;
    m_vertices[BOTTOM_LEFT_NORMAL_Z_T1] = t_normal.z;

    m_vertices[BOTTOM_RIGHT_NORMAL_X_T1] = t_normal.x;
    m_vertices[BOTTOM_RIGHT_NORMAL_Y_T1] = t_normal.y;
    m_vertices[BOTTOM_RIGHT_NORMAL_Z_T1] = t_normal.z;

    m_vertices[TOP_LEFT_NORMAL_X_T1] = t_normal.x;
    m_vertices[TOP_LEFT_NORMAL_Y_T1] = t_normal.y;
    m_vertices[TOP_LEFT_NORMAL_Z_T1] = t_normal.z;

    m_vertices[TOP_LEFT_NORMAL_X_T2] = t_normal.x;
    m_vertices[TOP_LEFT_NORMAL_Y_T2] = t_normal.y;
    m_vertices[TOP_LEFT_NORMAL_Z_T2] = t_normal.z;

    m_vertices[BOTTOM_RIGHT_NORMAL_X_T2] = t_normal.x;
    m_vertices[BOTTOM_RIGHT_NORMAL_Y_T2] = t_normal.y;
    m_vertices[BOTTOM_RIGHT_NORMAL_Z_T2] = t_normal.z;

    m_vertices[TOP_RIGHT_NORMAL_X_T2] = t_normal.x;
    m_vertices[TOP_RIGHT_NORMAL_Y_T2] = t_normal.y;
    m_vertices[TOP_RIGHT_NORMAL_Z_T2] = t_normal.z;
}

void sg::city::map::Tile::SetColor(const glm::vec3& t_color)
{
    m_vertices[BOTTOM_LEFT_COLOR_X_T1] = t_color.x;
    m_vertices[BOTTOM_LEFT_COLOR_Y_T1] = t_color.y;
    m_vertices[BOTTOM_LEFT_COLOR_Z_T1] = t_color.z;

    m_vertices[BOTTOM_RIGHT_COLOR_X_T1] = t_color.x;
    m_vertices[BOTTOM_RIGHT_COLOR_Y_T1] = t_color.y;
    m_vertices[BOTTOM_RIGHT_COLOR_Z_T1] = t_color.z;

    m_vertices[TOP_LEFT_COLOR_X_T1] = t_color.x;
    m_vertices[TOP_LEFT_COLOR_Y_T1] = t_color.y;
    m_vertices[TOP_LEFT_COLOR_Z_T1] = t_color.z;

    m_vertices[TOP_LEFT_COLOR_X_T2] = t_color.x;
    m_vertices[TOP_LEFT_COLOR_Y_T2] = t_color.y;
    m_vertices[TOP_LEFT_COLOR_Z_T2] = t_color.z;

    m_vertices[BOTTOM_RIGHT_COLOR_X_T2] = t_color.x;
    m_vertices[BOTTOM_RIGHT_COLOR_Y_T2] = t_color.y;
    m_vertices[BOTTOM_RIGHT_COLOR_Z_T2] = t_color.z;

    m_vertices[TOP_RIGHT_COLOR_X_T2] = t_color.x;
    m_vertices[TOP_RIGHT_COLOR_Y_T2] = t_color.y;
    m_vertices[TOP_RIGHT_COLOR_Z_T2] = t_color.z;
}

void sg::city::map::Tile::SetTexture(const float t_texture)
{
    m_vertices[BOTTOM_LEFT_TEXTURE_NR_T1] = t_texture;
    m_vertices[BOTTOM_RIGHT_TEXTURE_NR_T1] = t_texture;
    m_vertices[TOP_LEFT_TEXTURE_NR_T1] = t_texture;
    m_vertices[TOP_LEFT_TEXTURE_NR_T2] = t_texture;
    m_vertices[BOTTOM_RIGHT_TEXTURE_NR_T2] = t_texture;
    m_vertices[TOP_RIGHT_TEXTURE_NR_T2] = t_texture;
}

void sg::city::map::Tile::SetUv(const glm::vec2& t_bL, const glm::vec2& t_bR, const glm::vec2& t_tL, const glm::vec2& t_tR)
{
    m_vertices[BOTTOM_LEFT_TEXTURE_X_T1] = t_bL.x;
    m_vertices[BOTTOM_LEFT_TEXTURE_Y_T1] = t_bL.y;

    m_vertices[BOTTOM_RIGHT_TEXTURE_X_T1] = t_bR.x;
    m_vertices[BOTTOM_RIGHT_TEXTURE_Y_T1] = t_bR.y;

    m_vertices[TOP_LEFT_TEXTURE_X_T1] = t_tL.x;
    m_vertices[TOP_LEFT_TEXTURE_Y_T1] = t_tL.y;

    m_vertices[TOP_LEFT_TEXTURE_X_T2] = t_tL.x;
    m_vertices[TOP_LEFT_TEXTURE_Y_T2] = t_tL.y;

    m_vertices[BOTTOM_RIGHT_TEXTURE_X_T2] = t_bR.x;
    m_vertices[BOTTOM_RIGHT_TEXTURE_Y_T2] = t_bR.y;

    m_vertices[TOP_RIGHT_TEXTURE_X_T2] = t_tR.x;
    m_vertices[TOP_RIGHT_TEXTURE_Y_T2] = t_tR.y;
}

void sg::city::map::Tile::SetType(const Map::TileType t_type)
{
    m_type = t_type;

    // we use the TileType number as value for the texture
    SetTexture(static_cast<float>(m_type));

    // we use the TileType color as Tile color
    SetColor(Map::TILE_TYPE_COLOR.at(m_type));
}

void sg::city::map::Tile::SetRegion(const int t_region)
{
    m_region = t_region;
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void sg::city::map::Tile::Update() const
{
    /*
    if (m_type == Map::TileType::RESIDENTIAL)
    {
        SG_OGL_LOG_INFO("Population: {}", population);
    }

    if (m_type == Map::TileType::RESIDENTIAL && population >= MAX_POPULATION)
    {
        SG_OGL_LOG_INFO("Max population reached.");
    }
    */
}

//-------------------------------------------------
// To string
//-------------------------------------------------

std::string sg::city::map::Tile::TileTypeToString(const Map::TileType t_type)
{
    switch (t_type)
    {
        default:
        case Map::TileType::NONE: return "None";
        case Map::TileType::RESIDENTIAL: return "Residential Zone";
        case Map::TileType::COMMERCIAL: return "Commercial Zone";
        case Map::TileType::INDUSTRIAL: return "Industrial Zone";
        case Map::TileType::TRAFFIC_NETWORK: return "Roads or rails";
    }
}

//-------------------------------------------------
// Debug
//-------------------------------------------------

void sg::city::map::Tile::CreateNavigationNodesMesh()
{
    SG_OGL_CORE_ASSERT(!m_navigationNodes.empty(), "[Tile::CreateNavigationNodesMesh()] No navigation nodes available.")

    VertexContainer vertexContainer;

    for (auto& node : m_navigationNodes)
    {
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

    m_navigationNodesMesh = std::make_unique<ogl::resource::Mesh>();

    const ogl::buffer::BufferLayout bufferLayout{
        { ogl::buffer::VertexAttributeType::POSITION, "aPosition" },
        { ogl::buffer::VertexAttributeType::COLOR, "aColor" },
    };

    m_navigationNodesMesh->GetVao().AddVertexDataVbo(vertexContainer.data(), static_cast<int32_t>(vertexContainer.size()) / 6, bufferLayout);
}

void sg::city::map::Tile::CreateAutoTracksMesh()
{
    SG_OGL_CORE_ASSERT(!m_autoTracks.empty(), "[Tile::CreateAutoTracksMesh()] No auto tracks available.")

    if (m_autoTracksMesh)
    {
        m_autoTracksMesh.reset();
    }

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

    m_autoTracksMesh = std::make_unique<ogl::resource::Mesh>();

    const ogl::buffer::BufferLayout bufferLayout{
        { ogl::buffer::VertexAttributeType::POSITION, "aPosition" },
        { ogl::buffer::VertexAttributeType::COLOR, "aColor" },
    };

    m_autoTracksMesh->GetVao().AddVertexDataVbo(vertexContainer.data(), static_cast<int32_t>(m_autoTracks.size()) * 2, bufferLayout);
}

void sg::city::map::Tile::RenderNavigationNodes(const ogl::scene::Scene* const t_scene, const Map* const t_map) const
{
    SG_OGL_CORE_ASSERT(m_navigationNodesMesh, "[Tile::RenderNavigationNodes()] Null pointer.")

    ogl::math::Transform t;
    t.position = t_map->position;
    t.rotation = t_map->rotation;
    t.scale = t_map->scale;

    auto& shader{ t_scene->GetApplicationContext()->GetShaderManager().GetShaderProgram<shader::NodeShader>() };
    shader.Bind();

    const auto projectionMatrix{ t_scene->GetApplicationContext()->GetWindow().GetProjectionMatrix() };
    const auto mvp{ projectionMatrix * t_scene->GetCurrentCamera().GetViewMatrix() * static_cast<glm::mat4>(t) };

    shader.SetUniform("mvpMatrix", mvp);

    glPointSize(POINT_SIZE);

    m_navigationNodesMesh->InitDraw();
    m_navigationNodesMesh->DrawPrimitives(GL_POINTS);
    m_navigationNodesMesh->EndDraw();

    ogl::resource::ShaderProgram::Unbind();
}

void sg::city::map::Tile::RenderAutoTracks(const ogl::scene::Scene* t_scene, const Map* t_map) const
{
    if (!m_autoTracksMesh)
    {
        return;
    }

    ogl::math::Transform t;
    t.position = t_map->position;
    t.rotation = t_map->rotation;
    t.scale = t_map->scale;

    auto& shader{ t_scene->GetApplicationContext()->GetShaderManager().GetShaderProgram<shader::LineShader>() };
    shader.Bind();

    const auto projectionMatrix{ t_scene->GetApplicationContext()->GetWindow().GetProjectionMatrix() };
    const auto mvp{ projectionMatrix * t_scene->GetCurrentCamera().GetViewMatrix() * static_cast<glm::mat4>(t) };

    shader.SetUniform("mvpMatrix", mvp);

    m_autoTracksMesh->InitDraw();
    m_autoTracksMesh->DrawPrimitives(GL_LINES);
    m_autoTracksMesh->EndDraw();

    ogl::resource::ShaderProgram::Unbind();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void sg::city::map::Tile::Create()
{
    /*
        tL       tR
        +--------+
        |  +   2 |
        |    +   |
        | 1    + |
        +--------+
        bL       bR
    */

    m_vertices.resize(FLOATS_PER_TILE);

    m_bottomLeft = glm::vec3(m_mapX, DEFAULT_HEIGHT, -m_mapZ);
    m_bottomRight = glm::vec3(m_mapX + 1.0f, DEFAULT_HEIGHT, -m_mapZ);
    m_topLeft = glm::vec3(m_mapX, DEFAULT_HEIGHT, -(m_mapZ + 1.0f));
    m_topRight = glm::vec3(m_mapX + 1.0f, DEFAULT_HEIGHT, -(m_mapZ + 1.0f));

    SetVertexPositions();
    SetNormal(DEFAULT_NORMAL);
    SetColor(Map::TILE_TYPE_COLOR.at(m_type));
    SetTexture(static_cast<float>(m_type));
    SetUv();
}
