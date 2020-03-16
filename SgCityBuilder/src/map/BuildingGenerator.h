// This file is part of the SgCityBuilder package.
// 
// Filename: BuildingGenerator.h
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#pragma once

#include <memory>
#include <glm/mat4x4.hpp>

namespace sg::ogl::resource
{
    class Mesh;
}

namespace sg::city::map
{
    class Map;

    class BuildingGenerator
    {
    public:
        using VertexContainer = std::vector<float>;
        using TileIndexContainer = std::vector<int>;
        using MeshUniquePtr = std::unique_ptr<ogl::resource::Mesh>;
        using MatricesContainer = std::vector<glm::mat4>;

        static constexpr uint32_t NUMBER_OF_FLOATS_PER_INSTANCE{ 16 };
        static constexpr auto DRAW_COUNT{ 36 };
        static constexpr auto NO_BUILDING{ -1 };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        BuildingGenerator() = delete;

        explicit BuildingGenerator(Map* t_map);

        BuildingGenerator(const BuildingGenerator& t_other) = delete;
        BuildingGenerator(BuildingGenerator&& t_other) noexcept = delete;
        BuildingGenerator& operator=(const BuildingGenerator& t_other) = delete;
        BuildingGenerator& operator=(BuildingGenerator&& t_other) noexcept = delete;

        ~BuildingGenerator() noexcept;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        [[nodiscard]] const ogl::resource::Mesh& GetMesh() const noexcept;
        [[nodiscard]] ogl::resource::Mesh& GetMesh() noexcept;

        [[nodiscard]] uint32_t GetTextureAtlasId() const;
        [[nodiscard]] uint32_t GetInstances() const;

        //-------------------------------------------------
        // Add Building
        //-------------------------------------------------

        void StoreBuildingOnPosition(const glm::vec3& t_mapPoint);

        //-------------------------------------------------
        // Update
        //-------------------------------------------------

        void Update();

    protected:

    private:
        /**
         * @brief A pointer to the parent Map.
         */
        Map* m_map{ nullptr };

        /**
         * @brief The Vbo Id holding the instanced data of all buildings.
         */
        uint32_t m_vboId{ 0 };

        /**
         * @brief A quad mesh using as default geometry for a building.
         */
        MeshUniquePtr m_quadMesh;

        /**
         * @brief The texture Id of the building texture atlas.
         */
        uint32_t m_buildingTextureAtlasId{ 0 };

        /**
         * @brief Stores the position of the building in the Vbo.
         */
        TileIndexContainer m_lookupTable;

        /**
         * @brief The current number of building instances.
         */
        uint32_t m_instances{ 0 };

        /**
         * @brief The transformation matrices for all the instances.
         */
        MatricesContainer m_matrices;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        void InitQuadMesh();
        void Init();

        void AddBuildingInstance(const glm::vec3& t_mapPoint);

        //-------------------------------------------------
        // Update
        //-------------------------------------------------

        void UpdateVbo();
    };
}
