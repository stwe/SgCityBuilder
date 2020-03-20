// This file is part of the SgCityBuilder package.
// 
// Filename: Debug.h
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#pragma once

#include <memory>
#include <vector>
#include <glm/vec3.hpp>
#include <math/Transform.h>

namespace sg::ogl::scene
{
    class Scene;
}

namespace sg::ogl::resource
{
    class Mesh;
}

namespace sg::city::automata
{
    class AutoNode;
}

namespace sg::city::map
{
    class Map;
}

namespace sg::city::util
{
    class Debug
    {
    public:
        using VertexContainer = std::vector<float>;
        using MeshUniquePtr = std::unique_ptr<ogl::resource::Mesh>;
        using MeshContainer = std::vector<MeshUniquePtr>;
        using AutoNodeContainer = std::vector<std::unique_ptr<automata::AutoNode>>;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Debug() = delete;

        Debug(ogl::scene::Scene* t_scene, map::Map* t_map);

        Debug(const Debug& t_other) = delete;
        Debug(Debug&& t_other) noexcept = delete;
        Debug& operator=(const Debug& t_other) = delete;
        Debug& operator=(Debug&& t_other) noexcept = delete;

        ~Debug() noexcept;

        //-------------------------------------------------
        // Lines
        //-------------------------------------------------

        void AddLine(const glm::vec3& t_position0, const glm::vec3& t_position1, const glm::vec3& t_color);

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        void RenderLines();
        void RenderAutoNodes();

        //-------------------------------------------------
        // Nodes
        //-------------------------------------------------

        void InitAutoNodesMesh(bool t_trafficOnly = true);

    protected:

    private:
        ogl::scene::Scene* m_scene{ nullptr };
        map::Map* m_map{ nullptr };

        ogl::math::Transform m_mapTransform;

        MeshContainer m_lineMeshContainer;

        VertexContainer m_nodeVertices;
        MeshUniquePtr m_nodesMesh;
    };
}
