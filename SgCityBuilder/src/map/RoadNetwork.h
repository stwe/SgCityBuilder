#pragma once

#include <memory>

namespace sg::ogl::resource
{
    class Mesh;
}

namespace sg::city::map
{
    class Map;
    class Tile;

    class RoadNetwork
    {
    public:
        static constexpr auto TEXTURE_ATLAS_ROWS{ 4.0f };

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
        using MeshUniquePtr = std::unique_ptr<ogl::resource::Mesh>;

        static constexpr auto NO_ROAD{ -1 };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        RoadNetwork() = delete;

        explicit RoadNetwork(Map* t_map);

        RoadNetwork(const RoadNetwork& t_other) = delete;
        RoadNetwork(RoadNetwork&& t_other) noexcept = delete;
        RoadNetwork& operator=(const RoadNetwork& t_other) = delete;
        RoadNetwork& operator=(RoadNetwork&& t_other) noexcept = delete;

        ~RoadNetwork() noexcept = default;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        [[nodiscard]] const ogl::resource::Mesh& GetMesh() const noexcept;
        [[nodiscard]] ogl::resource::Mesh& GetMesh() noexcept;

        [[nodiscard]] uint32_t GetRoadTextureAtlasId() const;

        //-------------------------------------------------
        // Add Road
        //-------------------------------------------------

        void StoreRoadOnPosition(const glm::vec3& t_mapPoint);
        void StoreRoadOnPosition(int t_mapX, int t_mapZ);

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
         * @brief A Mesh instance for the RoadNetwork.
         */
        MeshUniquePtr m_roadNetworkMesh;

        /**
         * @brief The Id of Vbo holding the vertices.
         */
        uint32_t m_vboId{ 0 };

        /**
         * @brief Vertices of all Roads.
         */
        VertexContainer m_vertices;

        /**
         * @brief The texture Id of the road texture atlas.
         */
        uint32_t m_roadTextureAtlasId{ 0 };

        /**
         * @brief Stores the position of the Road Tile in the Vbo.
         */
        TileIndexContainer m_lookupTable;

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
        static RoadType GetRoadType(const Tile& t_tile);

        void UpdateVbo();
    };
}
