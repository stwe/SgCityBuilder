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
        static constexpr auto TEXTURE_ATLAS_ROWS{ 4.0f };
        static constexpr auto NO_ROAD{ -1 };
        static constexpr auto ROAD_VERTICES_HEIGHT{ 0.001f };
        static constexpr auto STOP{ 'X' };
        static constexpr auto NODES_PER_TILE{ 49 };

        /**
         * @brief Possible Road Neighbours.
         *        Each flag can set by using the OR operator.
         */
        enum RoadNeighbours : uint8_t
        {
            NORTH = 1,
            EAST  = 2,
            SOUTH = 4,
            WEST  = 8
        };

        /**
         * @brief The value corresponds to the index in the texture atlas.
         */
        enum class RoadType
        {
            ROAD_V = 0,
            ROAD_H = 1,
            ROAD_C1 = 4,
            ROAD_T1 = 5,
            ROAD_C2 = 6,
            ROAD_T2 = 8,
            ROAD_X = 9,
            ROAD_T3 = 10,
            ROAD_C3 = 12,
            ROAD_T4 = 13,
            ROAD_C4 = 14,
        };

        using VertexContainer = std::vector<float>;
        using TileIndexContainer = std::vector<int>;
        using TileRoadTypeContainer = std::vector<int>;
        using MeshUniquePtr = std::unique_ptr<ogl::resource::Mesh>;

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
        // Add Road
        //-------------------------------------------------

        /**
         * @brief Sets the Tile type to Traffic at a given position.
         *        In addition, vertices for a road are created for this Tile
         *        and these are inserted into a large mesh for all roads.
         *        So all roads can be drawn with a single draw call.
         * @param t_mapX The Map-x position of the Tile in Object Space.
         * @param t_mapZ The Map-z position of the Tile in Object Space.
         */
        void StoreRoadOnMapPosition(int t_mapX, int t_mapZ);

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
         * @brief The texture Id of the road texture atlas.
         */
        uint32_t m_roadTextureAtlasId{ 0 };

        /**
         * @brief The Id of Vbo holding the vertices.
         */
        uint32_t m_vboId{ 0 };

        /**
         * @brief Vertices of all Roads.
         */
        VertexContainer m_vertices;

        /**
         * @brief Stores the position of the Road Tile in the Vbo.
         */
        TileIndexContainer m_lookupTable;

        /**
         * @brief Stores the RoadType of the Tile.
         * @todo There are certainly better solutions for this, e.g. a special RoadTile.
         */
        TileRoadTypeContainer m_tileRoadTypes;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        void CreateVbo();
        void Init();

        //-------------------------------------------------
        // Update
        //-------------------------------------------------

        /**
         * @brief Determines the correct RoadType for the Tile depending on the neighbors.
         * @param t_tile The tile for which the type is to be determined.
         * @return A RoadType.
         */
        static RoadType DetermineRoadType(const tile::RoadTile& t_tile);

        /**
         * @brief Write the new vertex data in the Vbo.
         */
        void UpdateVbo();

        void UpdateAutoTracks(int t_tileIndex, RoadType t_roadType) const;

        /**
         * @brief Update the uv values for texturing.
         */
        void UpdateRoadsTextures();

        /**
         * @brief Update navigation nodes and auto tracks.
         */
        void UpdateNavigation();

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        void AddAutoTrack(int t_tileIndex, int t_fromNodeIndex, int t_toNodeIndex, bool t_safeCarAutoTrack = false) const;

        [[nodiscard]] tile::RoadTile::StopPattern CreateStopPattern(std::string t_s) const;
    };
}
