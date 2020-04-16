// This file is part of the SgCityBuilder package.
// 
// Filename: BuildingGenerator.h
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#pragma once

#include "tile/BuildingTile.h"

namespace sg::city::city
{
    class City;
}

namespace sg::city::map
{
    class BuildingGenerator
    {
    public:
        using MeshUniquePtr = std::unique_ptr<ogl::resource::Mesh>;
        using VertexContainer = std::vector<float>;
        using MatricesContainer = std::vector<glm::mat4>;

        //-------------------------------------------------
        // Const
        //-------------------------------------------------

        static constexpr auto DRAW_COUNT{ 36 };
        static constexpr uint32_t NUMBER_OF_FLOATS_PER_INSTANCE{ 16 };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        BuildingGenerator() = delete;

        explicit BuildingGenerator(city::City* t_city);

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

        [[nodiscard]] uint32_t GetInstances() const;
        [[nodiscard]] uint32_t GetBuildingTextureAtlasId() const;

        //-------------------------------------------------
        // Add
        //-------------------------------------------------

        void AddBuilding(tile::BuildingTile& t_buildingTile);

    protected:

    private:
        /**
         * @brief A pointer to the parent City.
         */
        city::City* m_city{ nullptr };

        /**
         * @brief A quad mesh using as default geometry for a single building.
         */
        MeshUniquePtr m_quadMesh;

        /**
         * @brief The Id of the Vbo holding the instanced data of all buildings.
         */
        uint32_t m_vboId{ 0 };

        /**
         * @brief The transformation matrices for all the instances.
         */
        MatricesContainer m_matrices;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        void InitQuadMesh();
        void InitVboForInstancedData();
        void Init();
    };
}
