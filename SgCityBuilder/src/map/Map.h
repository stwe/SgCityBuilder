#pragma once

#include <vector>
#include <array>
#include <memory>
#include <glm/vec3.hpp>

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
    class Tile;

    class Map
    {
    public:
        enum class TileType
        {
            NONE,
            RESIDENTIAL,
            COMMERCIAL,
            INDUSTRIAL,
            TRAFFIC_NETWORK, // roads or rails
            SIZE
        };

        static constexpr std::array<glm::vec3, static_cast<int>(TileType::SIZE)> TILE_TYPE_COLOR{
            glm::vec3(0.0f, 0.5f, 0.0f), // green:  None
            glm::vec3(0.0f, 0.8f, 0.0f), // lime:   Residential
            glm::vec3(0.0f, 0.0f, 0.8f), // blue:   Commercial
            glm::vec3(0.8f, 0.8f, 0.0f), // yellow: Industrial
            glm::vec3(0.8f, 0.8f, 0.8f), // white:  Traffic Network
        };

        using TileTypeTextureContainer = std::array<uint32_t, static_cast<int>(TileType::SIZE)>;
        using MeshUniquePtr = std::unique_ptr<ogl::resource::Mesh>;
        using TileUniquePtr = std::unique_ptr<Tile>;
        using TileContainer = std::vector<TileUniquePtr>;

        //-------------------------------------------------
        // Public member
        //-------------------------------------------------

        glm::vec3 position{ glm::vec3(0.0f) };
        glm::vec3 rotation{ glm::vec3(0.0f) };
        glm::vec3 scale{ glm::vec3(1.0f) };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Map() = delete;

        explicit Map(ogl::scene::Scene* t_scene);

        Map(const Map& t_other) = delete;
        Map(Map&& t_other) noexcept = delete;
        Map& operator=(const Map& t_other) = delete;
        Map& operator=(Map&& t_other) noexcept = delete;

        ~Map() noexcept;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        [[nodiscard]] const TileTypeTextureContainer& GetTileTypeTextures() const noexcept;
        [[nodiscard]] TileTypeTextureContainer& GetTileTypeTextures() noexcept;

        [[nodiscard]] const ogl::resource::Mesh& GetMapMesh() const noexcept;
        [[nodiscard]] ogl::resource::Mesh& GetMapMesh() noexcept;

        [[nodiscard]] const TileContainer& GetTiles() const noexcept;
        [[nodiscard]] TileContainer& GetTiles() noexcept;

        [[nodiscard]] int GetMapSize() const;

        //-------------------------------------------------
        // Create
        //-------------------------------------------------

        void CreateMap(int t_mapSize);

        /**
         * @brief Updates the vertex data in the Vbo of a given Tile.
         * @param t_tileIndex The Tile index to determine the Vbo buffer-offset.
         */
        void UpdateMapTile(int t_tileIndex) const;

    protected:

    private:
        /**
         * @brief Pointer to the parent Scene to get the TextureManager.
         */
        ogl::scene::Scene* m_scene{ nullptr };

        /**
         * @brief An array that contains a texture id for each Tile type.
         */
        TileTypeTextureContainer m_tileTypeTextures{ 0,0,0,0,0 };

        /**
         * @brief A Mesh instance for the Map.
         */
        MeshUniquePtr m_mapMesh;

        /**
         * @brief A container holding the Tile instances.
         */
        TileContainer m_tiles;

        /**
         * @brief The number of tiles in the x and z direction.
         */
        int m_mapSize{ 0 };

        /**
         * @brief The Id of Vbo holding the vertices.
         */
        uint32_t m_vboId{ 0 };

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        [[nodiscard]] uint32_t GetFloatCountOfMap() const;
        [[nodiscard]] int32_t GetVerticesCountOfMap() const;

        void LoadAndStoreTileTypeTextures();
        void StoreNeighbours();
        void CreateVbo();
        void StoreTilesInVbo();
    };
}
