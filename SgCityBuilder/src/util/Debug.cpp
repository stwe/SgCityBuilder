// This file is part of the SgCityBuilder package.
// 
// Filename: Debug.cpp
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#include <Application.h>
#include <Window.h>
#include <resource/Mesh.h>
#include <resource/ShaderManager.h>
#include <scene/Scene.h>
#include <camera/Camera.h>
#include "Debug.h"
#include "automata/AutoNode.h"
#include "automata/AutoTrack.h"
#include "shader/LineShader.h"
#include "shader/NodeShader.h"
#include "map/Map.h"
#include "map/Tile.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

sg::city::util::Debug::Debug(ogl::scene::Scene* t_scene, map::Map* t_map)
    : m_scene{ t_scene }
    , m_map{ t_map }
{
    m_scene->GetApplicationContext()->GetShaderManager().AddShaderProgram<shader::LineShader>();
    m_scene->GetApplicationContext()->GetShaderManager().AddShaderProgram<shader::NodeShader>();

    m_mapTransform.position = m_map->position;
    m_mapTransform.rotation = m_map->rotation;
    m_mapTransform.scale = m_map->scale;
}

sg::city::util::Debug::~Debug() noexcept
{
}

//-------------------------------------------------
// Render
//-------------------------------------------------

void sg::city::util::Debug::RenderAutoTrackLines()
{
    auto& shader{ m_scene->GetApplicationContext()->GetShaderManager().GetShaderProgram<shader::LineShader>() };
    shader.Bind();

    const auto projectionMatrix{ m_scene->GetApplicationContext()->GetWindow().GetProjectionMatrix() };
    const auto mvp{ projectionMatrix * m_scene->GetCurrentCamera().GetViewMatrix() * static_cast<glm::mat4>(m_mapTransform) };

    shader.SetUniform("mvpMatrix", mvp);

    for (auto& mesh : m_lineMeshContainer)
    {
        mesh->InitDraw();
        mesh->DrawPrimitives(GL_LINES);
        mesh->EndDraw();
    }

    ogl::resource::ShaderProgram::Unbind();
}

void sg::city::util::Debug::RenderAutoNodes(const float t_size) const
{
    if (!m_nodesMesh || m_nodesMesh->GetVao().GetDrawCount() == 0)
    {
        return;
    }

    auto& shader{ m_scene->GetApplicationContext()->GetShaderManager().GetShaderProgram<shader::NodeShader>() };
    shader.Bind();

    const auto projectionMatrix{ m_scene->GetApplicationContext()->GetWindow().GetProjectionMatrix() };
    const auto mvp{ projectionMatrix * m_scene->GetCurrentCamera().GetViewMatrix() * static_cast<glm::mat4>(m_mapTransform) };

    shader.SetUniform("mvpMatrix", mvp);

    glPointSize(t_size);

    m_nodesMesh->InitDraw();
    m_nodesMesh->DrawPrimitives(GL_POINTS);
    m_nodesMesh->EndDraw();

    ogl::resource::ShaderProgram::Unbind();
}

//-------------------------------------------------
// Update
//-------------------------------------------------

void sg::city::util::Debug::UpdateAutoNodesMesh(const bool t_trafficOnly)
{
    if (m_nodesMesh)
    {
        m_nodesMesh.reset();
    }

    VertexContainer vertexContainer;

    for (auto& tile : m_map->GetTiles())
    {
        bool b;
        t_trafficOnly ? b = tile->GetType() == map::Map::TileType::TRAFFIC_NETWORK : b = true;

        if (b)
        {
            for (auto& autoNode : tile->GetNavigationNodes())
            {
                if (autoNode)
                {
                    // position
                    vertexContainer.push_back(autoNode->position.x);
                    vertexContainer.push_back(VERTEX_HEIGHT);
                    vertexContainer.push_back(autoNode->position.z);

                    // color
                    if (autoNode->block)
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
    }

    m_nodesMesh = std::make_unique<ogl::resource::Mesh>();

    const ogl::buffer::BufferLayout bufferLayout{
        { ogl::buffer::VertexAttributeType::POSITION, "aPosition" },
        { ogl::buffer::VertexAttributeType::COLOR, "aColor" },
    };

    m_nodesMesh->GetVao().AddVertexDataVbo(vertexContainer.data(), static_cast<int32_t>(vertexContainer.size()) / 6, bufferLayout);
}

void sg::city::util::Debug::UpdateAutoTrackMeshes()
{
    if (!m_lineMeshContainer.empty())
    {
        m_lineMeshContainer.clear();
    }

    for (auto& tile : m_map->GetTiles())
    {
        for (auto& autoTrack : tile->GetAutoTracks())
        {
            AddAutoTrackLine(
                glm::vec3(autoTrack->startNode->position.x, VERTEX_HEIGHT, autoTrack->startNode->position.z),
                glm::vec3(autoTrack->endNode->position.x, VERTEX_HEIGHT, autoTrack->endNode->position.z),
                glm::vec3(0.0f, 0.0f, 1.0f)
            );
        }
    }
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

void sg::city::util::Debug::AddAutoTrackLine(const glm::vec3& t_position0, const glm::vec3& t_position1, const glm::vec3& t_color)
{
    auto lineMesh{ std::make_unique<ogl::resource::Mesh>() };

    const ogl::buffer::BufferLayout bufferLayout{
        { ogl::buffer::VertexAttributeType::POSITION, "aPosition" },
        { ogl::buffer::VertexAttributeType::COLOR, "aColor" },
    };

    VertexContainer vertices{
        t_position0.x, t_position0.y, t_position0.z, t_color.x, t_color.y, t_color.z,
        t_position1.x, t_position1.y, t_position1.z, t_color.x, t_color.y, t_color.z,
    };

    lineMesh->GetVao().AddVertexDataVbo(vertices.data(), 2, bufferLayout);

    m_lineMeshContainer.push_back(std::move(lineMesh));
}
