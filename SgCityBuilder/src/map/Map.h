// This file is part of the SgCityBuilder package.
// 
// Filename: Map.h
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#pragma once

#include "tile/Tile.h"

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
    class Map
    {
    public:
        using VertexContainer = std::vector<float>;

        using TileTypeTextureContainer = std::unordered_map<tile::TileType, uint32_t, tile::TileTypeHash>;

        using TileUniquePtr = std::unique_ptr<tile::Tile>;
        using TileContainer = std::vector<TileUniquePtr>;

        using NavigationNodeSharedPtr = std::shared_ptr<automata::AutoNode>;
        using NavigationNodeContainer = std::vector<NavigationNodeSharedPtr>;
        using TileNavigationNodeContainer = std::vector<NavigationNodeContainer>;

        using RandomColorContainer = std::unordered_map<int, ogl::Color>;

        using MeshUniquePtr = std::unique_ptr<ogl::resource::Mesh>;

        //-------------------------------------------------
        // Const
        //-------------------------------------------------

        /**
         * @brief 200 predefined colors are generated to show contiguous regions.
         */
        static constexpr auto MAX_REGION_COLORS{ 200 };

        /**
         * @brief The default height for debug stuff.
         */
        static constexpr auto VERTEX_HEIGHT{ 0.015 };

        /**
         * @brief GL_POINTS size for rendering nodes.
         */
        static constexpr auto POINT_SIZE{ 4.0f };

        /**
         * @brief The number of Navigation Nodes per Tile.
         */
        static constexpr auto NODES_PER_TILE{ 49 };

        //-------------------------------------------------
        // Public member
        //-------------------------------------------------

        glm::vec3 position{ glm::vec3(0.0f) };
        glm::vec3 rotation{ glm::vec3(0.0f) };
        glm::vec3 scale{ glm::vec3(1.0f) };

        bool wireframeMode{ false };
        bool showRegions{ false };

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
        [[nodiscard]] int GetNrOfAllTiles() const;

        [[nodiscard]] const TileTypeTextureContainer& GetTileTypeTextures() const noexcept;
        [[nodiscard]] TileTypeTextureContainer& GetTileTypeTextures() noexcept;

        [[nodiscard]] uint32_t GetRoadTextureAtlasId() const;
        [[nodiscard]] uint32_t GetBuildingTextureAtlasId() const;

        [[nodiscard]] const TileContainer& GetTiles() const noexcept;
        [[nodiscard]] TileContainer& GetTiles() noexcept;

        [[nodiscard]] const ogl::resource::Mesh& GetMapMesh() const noexcept;
        [[nodiscard]] ogl::resource::Mesh& GetMapMesh() noexcept;

        [[nodiscard]] uint32_t GetFloatCountOfMap() const;

        [[nodiscard]] const TileNavigationNodeContainer& GetNavigationNodes() const noexcept;
        [[nodiscard]] TileNavigationNodeContainer& GetNavigationNodes() noexcept;

        [[nodiscard]] const NavigationNodeContainer& GetNavigationNodes(int t_index) const noexcept;
        [[nodiscard]] NavigationNodeContainer& GetNavigationNodes(int t_index) noexcept;

        [[nodiscard]] int GetNumRegions() const;

        //-------------------------------------------------
        // Get Tile
        //-------------------------------------------------

        [[nodiscard]] const tile::Tile& GetTileByMapPosition(int t_mapX, int t_mapZ) const noexcept;
        [[nodiscard]] tile::Tile& GetTileByMapPosition(int t_mapX, int t_mapZ) noexcept;

        /**
         * @brief The tiles are stored in a 1D array (m_tiles). The function calculates the
         *        index from the Tile position in Object Space.
         * @param t_mapX The Map-x position of the Tile in Object Space.
         * @param t_mapZ The Map-z position of the Tile in Object Space.
         * @return The Tile index of the Tile in the Map.
         */
        [[nodiscard]] int GetTileMapIndexByMapPosition(int t_mapX, int t_mapZ) const;

        //-------------------------------------------------
        // Create
        //-------------------------------------------------

        void CreateMap(int t_mapSize);

        //-------------------------------------------------
        // Update
        //-------------------------------------------------

        void UpdateMapVboByTileIndex(int t_tileIndex) const;

        //-------------------------------------------------
        // Regions
        //-------------------------------------------------

        void FindConnectedRegions();

        //-------------------------------------------------
        // Debug
        //-------------------------------------------------

        /**
         * @brief Create a Mesh from the Navigation Nodes.
         */
        void CreateNavigationNodesMesh();

        /**
         * @brief Render the Navigation Nodes.
         */
        void RenderNavigationNodes() const;

    protected:

    private:
        /**
         * @brief Pointer to the parent Scene.
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
         * @brief The Id of the road texture atlas.
         */
        uint32_t m_roadTextureAtlasId{ 0 };

        /**
         * @brief The Id of the building texture atlas.
         */
        uint32_t m_buildingTextureAtlasId{ 0 };

        /**
         * @brief A container holding the Tile instances.
         */
        TileContainer m_tiles;

        /**
         * @brief A container with randomly generated colors that e.g. can be used to display tile regions.
         */
        RandomColorContainer m_randomColors;

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

        /**
         * @brief Navigation Nodes for each Tile.
         */
        TileNavigationNodeContainer m_tileNavigationNodes;

        /**
         * @brief A Mesh with one Vertex for each Navigation Node.
         *        Used for debugging purposes.
         */
        MeshUniquePtr m_navigationNodesMesh;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        void StoreTileTypeTextures();
        void StoreTiles();
        void StoreTileNeighbours();
        void StoreTileNavigationNodes();
        void LinkTileNavigationNodes();
        void StoreRandomColors();

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        [[nodiscard]] int32_t GetVerticesCountOfMap() const;

        void DepthSearch(tile::Tile& t_startTile, int t_region);

        //-------------------------------------------------
        // Vbo
        //-------------------------------------------------

        void CreateVbo();
        void StoreTilesInVbo();
    };
};
