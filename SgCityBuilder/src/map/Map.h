#pragma once

#include <vector>
#include <array>
#include <map>
#include <memory>
#include <glm/vec3.hpp>

namespace sg::ogl
{
    struct Color;
}

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
        using TileTypeContainer = std::vector<TileType>;
        using RandomColorContainer = std::map<int, ogl::Color>;

        /**
         * @brief 200 predefined colors are generated to show contiguous regions.
         */
        static constexpr auto MAX_REGION_COLORS{ 200 };

        static constexpr auto NO_REGION{ 0 };

        //-------------------------------------------------
        // Public member
        //-------------------------------------------------

        glm::vec3 position{ glm::vec3(0.0f) };
        glm::vec3 rotation{ glm::vec3(0.0f) };
        glm::vec3 scale{ glm::vec3(1.0f) };

        bool showRegions{ false };
        bool wireframeMode{ false };

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

        [[nodiscard]] ogl::scene::Scene* GetScene() const;

        [[nodiscard]] const TileTypeTextureContainer& GetTileTypeTextures() const noexcept;
        [[nodiscard]] TileTypeTextureContainer& GetTileTypeTextures() noexcept;

        [[nodiscard]] const ogl::resource::Mesh& GetMapMesh() const noexcept;
        [[nodiscard]] ogl::resource::Mesh& GetMapMesh() noexcept;

        [[nodiscard]] const TileContainer& GetTiles() const noexcept;
        [[nodiscard]] TileContainer& GetTiles() noexcept;

        [[nodiscard]] const Tile& GetTileByIndex(int t_tileIndex) const noexcept;
        [[nodiscard]] Tile& GetTileByIndex(int t_tileIndex) noexcept;

        [[nodiscard]] const Tile& GetTileByPosition(const glm::vec3& t_mapPoint) const noexcept;
        [[nodiscard]] Tile& GetTileByPosition(const glm::vec3& t_mapPoint) noexcept;

        [[nodiscard]] int GetMapSize() const;

        [[nodiscard]] int GetNumRegions() const;

        //-------------------------------------------------
        // Create
        //-------------------------------------------------

        void CreateMap(int t_mapSize);

        //-------------------------------------------------
        // Update
        //-------------------------------------------------

        void ChangeTileTypeOnPosition(const glm::vec3& t_mapPoint, TileType t_tileType);

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        [[nodiscard]] uint32_t GetFloatCountOfMap() const;
        [[nodiscard]] int GetTileIndexByPosition(const glm::vec3& t_mapPoint) const;
        [[nodiscard]] int GetTileIndexByPosition(int t_x, int t_z) const;

        //-------------------------------------------------
        // Regions
        //-------------------------------------------------

        void FindConnectedRegions();

    protected:

    private:
        /**
         * @brief Pointer to the parent Scene to get the TextureManager.
         */
        ogl::scene::Scene* m_scene{ nullptr };

        /**
         * @brief An array that contains a texture Id for each TileType.
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

        /**
         * @brief The current number of regions.
         */
        int m_numRegions{ 0 };

        /**
         * @brief Tiles of these types can build regions.
         */
        TileTypeContainer m_tileTypes{ TileType::RESIDENTIAL, TileType::COMMERCIAL, TileType::INDUSTRIAL, TileType::TRAFFIC_NETWORK };

        /**
         * @brief A container with randomly generated colors that e.g. can be used to display tile regions.
         */
        RandomColorContainer m_randomColors;

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        [[nodiscard]] int32_t GetVerticesCountOfMap() const;

        void LoadAndStoreTileTypeTextures();
        void StoreNeighbours();
        void CreateRandomColors();

        //-------------------------------------------------
        // Vbo
        //-------------------------------------------------

        void CreateVbo();
        void StoreTilesInVbo();
        void UpdateMapVboByTileIndex(int t_tileIndex) const;
        void UpdateMapVboByPosition(const glm::vec3& t_mapPoint) const;

        //-------------------------------------------------
        // Regions
        //-------------------------------------------------

        void DepthSearch(Tile& t_startTile, int t_region);
    };
}
