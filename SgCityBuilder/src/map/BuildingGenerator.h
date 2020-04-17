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
        struct BuildingInstanceData
        {
            glm::mat4 transformationMatrix;
            glm::vec4 color;
        };

        using MeshUniquePtr = std::unique_ptr<ogl::resource::Mesh>;
        using VertexContainer = std::vector<float>;
        using BuildingInstanceContainer = std::vector<BuildingInstanceData>;

        //-------------------------------------------------
        // Const
        //-------------------------------------------------

        static constexpr auto DRAW_COUNT{ 36 };

        // 4x4 tranformation Matrix                                                    = 16 floats
        // 3x  color                                                                   =  3 floats
        // 1x  texture flag (use the colors alpha value: 0 = use texture, 1 use color) =  1 float
        //                                                                             -----------
        //                                                                             = 20 floats
        static constexpr uint32_t NUMBER_OF_FLOATS_PER_INSTANCE{ 20 };

        /**
         * @brief Required to calculate the maximum number of quads instances.
         */
        static constexpr uint32_t MAX_INSTANCES_PER_TILE{ 10 };

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
         * @brief The the instances data.
         */
        BuildingInstanceContainer m_instanceDatas;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        void InitQuadMesh();
        void InitVboForInstancedData();
        void Init();

        //-------------------------------------------------
        // Floors
        //-------------------------------------------------

        void AddFloor(tile::BuildingTile& t_buildingTile, const glm::vec3& t_color);
    };
}
