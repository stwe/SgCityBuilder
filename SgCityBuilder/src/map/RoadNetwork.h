// This file is part of the SgCityBuilder package.
// 
// Filename: RoadNetwork.h
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#pragma once

#include "tile/RoadTile.h"

namespace sg::city::city
{
    class City;
}

namespace sg::city::map
{
    class RoadNetwork
    {
    public:
        using MeshUniquePtr = std::unique_ptr<ogl::resource::Mesh>;
        using VertexContainer = std::vector<float>;

        //-------------------------------------------------
        // Const
        //-------------------------------------------------

        static constexpr auto TEXTURE_ATLAS_ROWS{ 4.0f };
        static constexpr auto ROAD_VERTICES_HEIGHT{ 0.001f };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        RoadNetwork() = delete;

        explicit RoadNetwork(city::City* t_city);

        RoadNetwork(const RoadNetwork& t_other) = delete;
        RoadNetwork(RoadNetwork&& t_other) noexcept = delete;
        RoadNetwork& operator=(const RoadNetwork& t_other) = delete;
        RoadNetwork& operator=(RoadNetwork&& t_other) noexcept = delete;

        ~RoadNetwork() noexcept;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        [[nodiscard]] const ogl::resource::Mesh& GetMesh() const noexcept;
        [[nodiscard]] ogl::resource::Mesh& GetMesh() noexcept;

        [[nodiscard]] uint32_t GetRoadTextureAtlasId() const;

        //-------------------------------------------------
        // Create
        //-------------------------------------------------

        void CreateRoadNetworkMesh() const;

    protected:

    private:
        /**
         * @brief A pointer to the parent City.
         */
        city::City* m_city{ nullptr };

        /**
         * @brief A Mesh instance for the RoadNetwork.
         */
        MeshUniquePtr m_roadNetworkMesh;

        /**
         * @brief The Id of Vbo holding the vertices.
         */
        uint32_t m_vboId{ 0 };

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        void CreateVbo();
        void Init();
    };
}
