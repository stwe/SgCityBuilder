#include <Application.h>
#include <scene/Scene.h>
#include <resource/Mesh.h>
#include <resource/TextureManager.h>
#include "BuildingGenerator.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

sg::city::map::BuildingGenerator::BuildingGenerator(ogl::scene::Scene* t_scene)
    : m_scene{ t_scene }
{
    Init();
}

sg::city::map::BuildingGenerator::~BuildingGenerator() noexcept
{
}

const sg::ogl::resource::Mesh& sg::city::map::BuildingGenerator::GetBuildingsMesh() const noexcept
{
    return *m_quadMesh;
}

sg::ogl::resource::Mesh& sg::city::map::BuildingGenerator::GetBuildingsMesh() noexcept
{
    return *m_quadMesh;
}

uint32_t sg::city::map::BuildingGenerator::GetQuadTextureAtlasId() const
{
    return m_quadTextureAtlasId;
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void sg::city::map::BuildingGenerator::Init()
{
    /*

    512x256 = 128x128 textures

    0, 1                                            1, 1
             -----------------------------------------
             |       |  bottom   |   top   |    x    |
    0, 0.5   -----------------------------------------
             |       |           |         |    x    |
             -----------------------------------------
    0, 0           0, 0.25     0, 0.5    0, 0.75    1, 0
 
    */

    // load texture atlas
    m_quadTextureAtlasId = m_scene->GetApplicationContext()->GetTextureManager().GetTextureIdFromPath("res/texture/sc.png", true);

    // create Mesh
    m_quadMesh = std::make_shared<ogl::resource::Mesh>();

    // create BufferLayout
    const ogl::buffer::BufferLayout bufferLayout{
        { ogl::buffer::VertexAttributeType::POSITION, "aPosition" },
        { ogl::buffer::VertexAttributeType::NORMAL, "aNormal" },
        { ogl::buffer::VertexAttributeType::UV, "aUv" },
    };

    // vertices to render with GL_TRIANGLES
    std::vector<float> vertices{
        // positions          // normals           // texture coords
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

    // set draw count
    const auto drawCount{ 36 };

    // add Vbo
    m_quadMesh->GetVao().AddVertexDataVbo(vertices.data(), drawCount, bufferLayout);
}
