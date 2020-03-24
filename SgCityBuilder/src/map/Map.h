// This file is part of the SgCityBuilder package.
// 
// Filename: Map.h
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#pragma once

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

namespace sg::city::automata
{
    class AutoNode;
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
            TRAFFIC_NETWORK // roads or rails
        };

        struct TileTypeHash
        {
            std::size_t operator()(TileType t_tileType) const
            {
                return static_cast<std::size_t>(t_tileType);
            }
        };

        inline static const std::unordered_map<TileType, glm::vec3, TileTypeHash> TILE_TYPE_COLOR
        {
            { TileType::NONE, glm::vec3(0.0f, 0.5f, 0.0f) },
            { TileType::RESIDENTIAL, glm::vec3(0.0f, 0.8f, 0.0f) },
            { TileType::COMMERCIAL, glm::vec3(0.0f, 0.0f, 0.8f) },
            { TileType::INDUSTRIAL, glm::vec3(0.8f, 0.8f, 0.0f) },
            { TileType::TRAFFIC_NETWORK, glm::vec3(0.8f, 0.8f, 0.8f) }
        };

        /**
         * @brief Tile types that can build regions.
         */
        inline static const std::vector<TileType> REGION_TILE_TYPES
        {
            TileType::RESIDENTIAL,
            TileType::COMMERCIAL,
            TileType::INDUSTRIAL,
            TileType::TRAFFIC_NETWORK
        };

        /**
         * @brief 200 predefined colors are generated to show contiguous regions.
         */
        static constexpr auto MAX_REGION_COLORS{ 200 };

        static constexpr auto NO_REGION{ 0 };

        using TileTypeTextureContainer = std::unordered_map<TileType, uint32_t, TileTypeHash>;

        using MeshUniquePtr = std::unique_ptr<ogl::resource::Mesh>;

        using TileSharedPtr = std::shared_ptr<Tile>;
        using TileContainer = std::vector<TileSharedPtr>;

        using RandomColorContainer = std::unordered_map<int, ogl::Color>;

        using NavigationNodeSharedPtr = std::shared_ptr<automata::AutoNode>;
        using NavigationNodeContainer = std::vector<NavigationNodeSharedPtr>;

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

        [[nodiscard]] int GetMapSize() const;

        [[nodiscard]] const TileTypeTextureContainer& GetTileTypeTextures() const noexcept;
        [[nodiscard]] TileTypeTextureContainer& GetTileTypeTextures() noexcept;

        [[nodiscard]] const TileContainer& GetTiles() const noexcept;
        [[nodiscard]] TileContainer& GetTiles() noexcept;

        [[nodiscard]] const NavigationNodeContainer& GetNavigationNodes() const noexcept;
        [[nodiscard]] NavigationNodeContainer& GetNavigationNodes() noexcept;

        [[nodiscard]] const ogl::resource::Mesh& GetMapMesh() const noexcept;
        [[nodiscard]] ogl::resource::Mesh& GetMapMesh() noexcept;

        [[nodiscard]] int GetNumRegions() const;

        [[nodiscard]] uint32_t GetFloatCountOfMap() const;

        //-------------------------------------------------
        // Get Tile
        //-------------------------------------------------

        [[nodiscard]] const Tile& GetTileByIndex(int t_tileIndex) const noexcept;
        [[nodiscard]] Tile& GetTileByIndex(int t_tileIndex) noexcept;

        [[nodiscard]] const Tile& GetTileByMapPosition(int t_mapX, int t_mapZ) const noexcept;
        [[nodiscard]] Tile& GetTileByMapPosition(int t_mapX, int t_mapZ) noexcept;

        /**
         * @brief The tiles are stored in a 1D array (m_tiles). The function calculates the
         *        index from the Tile position in Object Space.
         * @param t_mapX The Map-x position of the Tile in Object Space.
         * @param t_mapZ The Map-z position of the Tile in Object Space.
         * @return The Tile index of the Tile in the Map.
         */
        [[nodiscard]] int GetTileMapIndexByMapPosition(int t_mapX, int t_mapZ) const;

        //-------------------------------------------------
        // Create Map
        //-------------------------------------------------

        void CreateMap(int t_mapSize);

        //-------------------------------------------------
        // Update
        //-------------------------------------------------

        void ChangeTileTypeOnMapPosition(int t_mapX, int t_mapZ, TileType t_tileType);

        //-------------------------------------------------
        // Regions
        //-------------------------------------------------

        //void FindConnectedRegions();

    protected:

    private:
        /**
         * @brief Pointer to the parent Scene to get the TextureManager.
         */
        ogl::scene::Scene* m_scene{ nullptr };

        /**
         * @brief The number of tiles in the x and z direction.
         */
        int m_mapSize{ 0 };

        /**
         * @brief A container that stores a texture Id for each TileType.
         */
        TileTypeTextureContainer m_tileTypeTextures;

        /**
         * @brief A container holding the Tile instances.
         */
        TileContainer m_tiles;

        /**
         * @brief A container with randomly generated colors that e.g. can be used to display tile regions.
         */
        RandomColorContainer m_randomColors;

        /**
         * @brief The container with the navigation nodes.
         */
        NavigationNodeContainer m_navigationNodes;

        /**
         * @brief A Mesh instance for the Map.
         */
        MeshUniquePtr m_mapMesh;

        /**
         * @brief The Id of Vbo holding the vertices.
         */
        uint32_t m_vboId{ 0 };

        /**
         * @brief The current number of regions.
         */
        int m_numRegions{ 0 };

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        void StoreTileTypeTextures();
        void CreateTiles();
        void StoreTileNeighbours();
        void CreateRandomColors();
        void CreateNavigationNodes();
        //void LinkTilesForNavigation();

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        [[nodiscard]] int32_t GetVerticesCountOfMap() const;

        //-------------------------------------------------
        // Vbo
        //-------------------------------------------------

        void CreateVbo();
        void StoreTilesInVbo();
        void UpdateMapVboByTileIndex(int t_tileIndex) const;

        //-------------------------------------------------
        // Regions
        //-------------------------------------------------

        //void DepthSearch(Tile& t_startTile, int t_region);
    };
}
