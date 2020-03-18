// This file is part of the SgCityBuilder package.
// 
// Filename: Line.cpp
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
#include "Line.h"
#include "shader/LineShader.h"
#include "map/Map.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

sg::city::util::Line::Line(ogl::scene::Scene* t_scene, const map::Map& t_map)
    : m_scene{ t_scene }
{
    m_scene->GetApplicationContext()->GetShaderManager().AddShaderProgram<shader::LineShader>();

    m_transform.position = t_map.position;
    m_transform.rotation = t_map.rotation;
    m_transform.scale = t_map.scale;
}

sg::city::util::Line::~Line() noexcept
{
}

//-------------------------------------------------
// Add
//-------------------------------------------------

void sg::city::util::Line::AddLine(const glm::vec3& t_position0, const glm::vec3& t_position1, const glm::vec3& t_color)
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

    m_meshContainer.push_back(std::move(lineMesh));
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void sg::city::util::Line::Render()
{
    auto& shader{ m_scene->GetApplicationContext()->GetShaderManager().GetShaderProgram<shader::LineShader>() };
    shader.Bind();

    const auto projectionMatrix{ m_scene->GetApplicationContext()->GetWindow().GetProjectionMatrix() };
    const auto mvp{ projectionMatrix * m_scene->GetCurrentCamera().GetViewMatrix() * static_cast<glm::mat4>(m_transform) };

    shader.SetUniform("mvpMatrix", mvp);

    for (auto& mesh : m_meshContainer)
    {
        mesh->InitDraw();
        glDrawArrays(GL_LINES, 0, 2);
        mesh->EndDraw();
    }

    ogl::resource::ShaderProgram::Unbind();
}
