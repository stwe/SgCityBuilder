// This file is part of the SgCityBuilder package.
// 
// Filename: BuildingGenerator.cpp
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#include <Application.h>
#include <scene/Scene.h>
#include <resource/Mesh.h>
#include <resource/TextureManager.h>
#include <math/Transform.h>
#include "BuildingGenerator.h"
#include "Map.h"
#include "Tile.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

sg::city::map::BuildingGenerator::BuildingGenerator(Map* t_map)
    : m_map{ t_map }
{
    Init();
}

sg::city::map::BuildingGenerator::~BuildingGenerator() noexcept
{
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

uint32_t sg::city::map::BuildingGenerator::GetTextureAtlasId() const
{
    return m_buildingTextureAtlasId;
}

uint32_t sg::city::map::BuildingGenerator::GetInstances() const
{
    return m_instances;
}

//-------------------------------------------------
// Add Building
//-------------------------------------------------

void sg::city::map::BuildingGenerator::StoreBuildingOnPosition(const glm::vec3& t_mapPoint)
{
    // get Tile index by given map point
    const auto tileIndex{ m_map->GetTileIndexByPosition(t_mapPoint) };

    // checks whether the Tile is already stored as a building
    if (m_lookupTable[tileIndex] > 0)
    {
        return;
    }

    // update TileType
    m_map->ChangeTileTypeOnPosition(t_mapPoint, Map::TileType::RESIDENTIAL);

    // Add a building (there is currently only one type of building)
    AddBuildingInstance(t_mapPoint);

    // update BuildingGenerator
    Update();
}

//-------------------------------------------------
// Update
//-------------------------------------------------

void sg::city::map::BuildingGenerator::Update()
{
    UpdateVbo();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void sg::city::map::BuildingGenerator::InitQuadMesh()
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

void sg::city::map::BuildingGenerator::Init()
{
    // a simple cube as a building
    InitQuadMesh();

    // create Vbo for instanced data
    m_vboId = ogl::buffer::Vbo::GenerateVbo();

    // get the number of Tiles
    const auto nrOfTiles{ m_map->GetMapSize() * m_map->GetMapSize() };

    // only one building on one tile is possible at the moment ---> number of Tiles (128 * 128) * 16 floats per instance
    ogl::buffer::Vbo::InitEmpty(m_vboId, nrOfTiles * NUMBER_OF_FLOATS_PER_INSTANCE, GL_DYNAMIC_DRAW);

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

    // load texture atlas
    m_buildingTextureAtlasId = m_map->GetScene()->GetApplicationContext()->GetTextureManager().GetTextureIdFromPath("res/texture/sc.png", true);

    // Every Tile can be a (and only one) building. The lookup table stores the position of the Tile in the Vbo.
    // The default value -1 (NO_BUILDING) means that the Tile is currently not a building and is therefore not in the Vbo.
    m_lookupTable.resize(nrOfTiles, NO_BUILDING);
}

void sg::city::map::BuildingGenerator::AddBuildingInstance(const glm::vec3& t_mapPoint)
{
    auto& tile{ m_map->GetTileByPosition(t_mapPoint) };

    ogl::math::Transform transform;
    transform.position = glm::vec3(tile.GetMapX() + 0.5f, 0.5f, -tile.GetMapZ() + -0.5f);
    transform.scale = glm::vec3(1.0f);

    m_matrices.push_back(static_cast<glm::mat4>(transform));

    m_instances = static_cast<int>(m_matrices.size());
    m_lookupTable[m_map->GetTileIndexByPosition(t_mapPoint)] = m_instances;
}

//-------------------------------------------------
// Update
//-------------------------------------------------

void sg::city::map::BuildingGenerator::UpdateVbo()
{
    if (m_instances > 0)
    {
        const auto sizeInBytes{ m_instances * NUMBER_OF_FLOATS_PER_INSTANCE * static_cast<uint32_t>(sizeof(float)) };

        ogl::buffer::Vbo::BindVbo(m_vboId);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeInBytes, m_matrices.data());
        ogl::buffer::Vbo::UnbindVbo();
    }
}
