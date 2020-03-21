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

        static constexpr auto VERTEX_HEIGHT{ 0.015 };

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
        // Render
        //-------------------------------------------------

        void RenderAutoTrackLines();
        void RenderAutoNodes(float t_size = 4.0f) const;

        //-------------------------------------------------
        // Update
        //-------------------------------------------------

        /**
         * @brief Generate a Mesh with one Vertex for each AutoNode.
         */
        void UpdateAutoNodesMesh(bool t_trafficOnly = true);

        /**
         * @brief Generate a Mesh for each AutoTrack line.
         */
        void UpdateAutoTrackMeshes();

    protected:

    private:
        /**
         * @brief Pointer to the parent Scene object.
         */
        ogl::scene::Scene* m_scene{ nullptr };

        /**
         * @brief Pointer to the parent Map object.
         */
        map::Map* m_map{ nullptr };

        /**
         * @brief The transformation matrix of the Map.
         */
        ogl::math::Transform m_mapTransform;

        /**
         * @brief A container holding one Mesh for each AutoTrack.
         */
        MeshContainer m_lineMeshContainer;

        /**
         * @brief A Mesh with one Vertex for each AutoNode.
         */
        MeshUniquePtr m_nodesMesh;

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * @brief Generate a Mesh for each AutoTrack line.
         * @param t_position0 Start point of the line.
         * @param t_position1 End point of the line.
         * @param t_color The color of the line.
         */
        void AddAutoTrackLine(const glm::vec3& t_position0, const glm::vec3& t_position1, const glm::vec3& t_color);
    };
}
