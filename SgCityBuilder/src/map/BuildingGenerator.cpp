// This file is part of the SgCityBuilder package.
// 
// Filename: BuildingGenerator.cpp
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#include <Core.h>
#include <resource/Mesh.h>
#include <math/Transform.h>
#include "BuildingGenerator.h"
#include "Map.h"
#include "city/City.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

sg::city::map::BuildingGenerator::BuildingGenerator(city::City* t_city)
    : m_city{ t_city }
{
    SG_OGL_CORE_ASSERT(t_city, "[BuildingGenerator::BuildingGenerator()] Null pointer.")
    SG_OGL_LOG_DEBUG("[BuildingGenerator::BuildingGenerator()] Construct BuildingGenerator.");

    Init();
}

sg::city::map::BuildingGenerator::~BuildingGenerator() noexcept
{
    SG_OGL_LOG_DEBUG("BuildingGenerator::~BuildingGenerator()] Destruct BuildingGenerator.");
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

const sg::ogl::resource::Mesh& sg::city::map::BuildingGenerator::GetMesh() const noexcept
{
    return *m_quadMesh;
}

sg::ogl::resource::Mesh& sg::city::map::BuildingGenerator::GetMesh() noexcept
{
    return *m_quadMesh;
}

uint32_t sg::city::map::BuildingGenerator::GetInstances() const
{
    return static_cast<int>(m_matrices.size());
}

uint32_t sg::city::map::BuildingGenerator::GetBuildingTextureAtlasId() const
{
    return m_city->GetMap().GetBuildingTextureAtlasId();
}

//-------------------------------------------------
// Add
//-------------------------------------------------

void sg::city::map::BuildingGenerator::AddBuilding(tile::BuildingTile& t_buildingTile)
{
    ogl::math::Transform transform;
    transform.position = glm::vec3(t_buildingTile.GetWorldX() + 0.5f, 0.5f, t_buildingTile.GetWorldZ() + -0.5f);
    transform.scale = glm::vec3(1.0f);

    m_matrices.push_back(static_cast<glm::mat4>(transform));

    const auto instances{ GetInstances() };
    if (instances > 0)
    {
        const auto sizeInBytes{ instances * NUMBER_OF_FLOATS_PER_INSTANCE * static_cast<uint32_t>(sizeof(float)) };

        ogl::buffer::Vbo::BindVbo(m_vboId);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeInBytes, m_matrices.data());
        ogl::buffer::Vbo::UnbindVbo();
    }
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void sg::city::map::BuildingGenerator::InitQuadMesh()
{
    /*

    0, 1                                            1, 1
             -----------------------------------------
             |       |  bottom   |   top   |    x    |
    0, 0.5   -----------------------------------------
             |       |           |         |    x    |
             -----------------------------------------
    0, 0           0, 0.25     0, 0.5    0, 0.75    1, 0

    */

    // create Mesh
    m_quadMesh = std::make_unique<ogl::resource::Mesh>();

    // create BufferLayout
    const ogl::buffer::BufferLayout bufferLayout{
        { ogl::buffer::VertexAttributeType::POSITION, "aPosition" },
        { ogl::buffer::VertexAttributeType::NORMAL, "aNormal" },
        { ogl::buffer::VertexAttributeType::UV, "aUv" },
    };

    // to render with GL_TRIANGLES
    VertexContainer vertices{
        // positions          // normals           // uv
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f, // bottom left image - ok
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.25f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.25f, 0.5f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.25f, 0.5f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.5f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.25f, 0.0f, // bottom middle image - ok
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.5f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.25f, 0.5f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.25f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.75f, 0.0f, // bottom right image - ok, aber rotiert
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.75f, 0.5f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.5f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.75f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.25f, 0.5f, // top left image - ok, aber rotiert
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.25f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.5f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.25f, 0.5f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.25f, 1.0f, // top middle image - ok
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.5f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.25f, 0.5f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.25f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.5f,  1.0f, // top right image - ok
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.75f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.75f, 0.5f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.75f, 0.5f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.5f,  1.0f
    };

    // add Vbo
    m_quadMesh->GetVao().AddVertexDataVbo(vertices.data(), DRAW_COUNT, bufferLayout);
}

void sg::city::map::BuildingGenerator::InitVboForInstancedData()
{
    SG_OGL_CORE_ASSERT(m_quadMesh, "[BuildingGenerator::InitVboForInstancedData()] Null pointer.")

    // create Vbo for instanced data
    m_vboId = ogl::buffer::Vbo::GenerateVbo();

    // only one building on one tile is possible at the moment ---> number of Tiles * 16 floats per instance
    ogl::buffer::Vbo::InitEmpty(m_vboId, m_city->GetMap().GetNrOfAllTiles() * NUMBER_OF_FLOATS_PER_INSTANCE, GL_DYNAMIC_DRAW);

    // get and bind the Vao of the quad Mesh
    auto& vao{ m_quadMesh->GetVao() };
    vao.BindVao();

    // set attributes of the above Vbo
    ogl::buffer::Vbo::AddInstancedAttribute(m_vboId, 3, 4, NUMBER_OF_FLOATS_PER_INSTANCE, 0);
    ogl::buffer::Vbo::AddInstancedAttribute(m_vboId, 4, 4, NUMBER_OF_FLOATS_PER_INSTANCE, 4);
    ogl::buffer::Vbo::AddInstancedAttribute(m_vboId, 5, 4, NUMBER_OF_FLOATS_PER_INSTANCE, 8);
    ogl::buffer::Vbo::AddInstancedAttribute(m_vboId, 6, 4, NUMBER_OF_FLOATS_PER_INSTANCE, 12);

    // unbind quad Mesh Vao
    ogl::buffer::Vao::UnbindVao();
}

void sg::city::map::BuildingGenerator::Init()
{
    SG_OGL_LOG_DEBUG("[BuildingGenerator::Init()] Initialize BuildingGenerator.");

    // a simple cube as building
    InitQuadMesh();

    // create a Vbo for instanced data
    InitVboForInstancedData();
}
