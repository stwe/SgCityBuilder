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
        enum RoadNeighbours : uint8_t
        {
            NORTH = 1,
            EAST  = 2,
            SOUTH = 4,
            WEST  = 8
        };

        enum class Texture
        {
            WO,
            NS,
            NSO,
            NSW,
            ALL
        };

        using VertexContainer = std::vector<float>;
        using TextureContainer = std::map<Texture, uint32_t>;
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

        [[nodiscard]] uint32_t GetTextureId(Texture t_texture) const;

        //-------------------------------------------------
        // Add Road
        //-------------------------------------------------

        void StoreRoadOnPosition(const glm::vec3& t_mapPoint);
        void StoreRoadOnPosition(float t_posX, float t_posZ);

        //-------------------------------------------------
        // Update
        //-------------------------------------------------

        void UpdateDirections();

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
         * @brief Container with the textures for each direction.
         */
        TextureContainer m_textures;

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
         * @brief Determines the correct texture for the Tile depending on the neighbors.
         * @param t_tile The tile for which the texture is to be determined.
         * @return Texture::WO | Texture::NS | Texture::NSO | Texture::NSW | Texture::ALL
         */
        static Texture GetTexture(const Tile& t_tile);

        void UpdateVbo();
    };
}
