// This file is part of the SgCityBuilder package.
// 
// Filename: Line.h
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
    class Line
    {
    public:
        using VertexContainer = std::vector<float>;
        using MeshUniquePtr = std::unique_ptr<ogl::resource::Mesh>;
        using MeshContainer = std::vector<MeshUniquePtr>;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Line() = delete;

        Line(ogl::scene::Scene* t_scene, const map::Map& t_map);

        Line(const Line& t_other) = delete;
        Line(Line&& t_other) noexcept = delete;
        Line& operator=(const Line& t_other) = delete;
        Line& operator=(Line&& t_other) noexcept = delete;

        ~Line() noexcept;

        //-------------------------------------------------
        // Add
        //-------------------------------------------------

        void AddLine(const glm::vec3& t_position0, const glm::vec3& t_position1, const glm::vec3& t_color);

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        void Render();

    protected:

    private:
        ogl::scene::Scene* m_scene{ nullptr };

        MeshContainer m_meshContainer;

        ogl::math::Transform m_transform;
    };
}
