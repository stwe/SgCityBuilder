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

namespace sg::ogl::resource
{
    class Mesh;
}

namespace sg::city::map
{
    class Map
    {
    public:
        using TileTypeTextureContainer = std::unordered_map<tile::TileType, uint32_t, tile::TileTypeHash>;

        using TileSharedPtr = std::shared_ptr<tile::Tile>;
        using TileContainer = std::vector<TileSharedPtr>;

        using RandomColorContainer = std::unordered_map<int, ogl::Color>;

        using MeshUniquePtr = std::unique_ptr<ogl::resource::Mesh>;

        //-------------------------------------------------
        // Const
        //-------------------------------------------------

        /**
         * @brief 200 predefined colors are generated to show contiguous regions.
         */
        static constexpr auto MAX_REGION_COLORS{ 200 };

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

        [[nodiscard]] const TileTypeTextureContainer& GetTileTypeTextures() const noexcept;
        [[nodiscard]] TileTypeTextureContainer& GetTileTypeTextures() noexcept;

        [[nodiscard]] const TileContainer& GetTiles() const noexcept;
        [[nodiscard]] TileContainer& GetTiles() noexcept;

        [[nodiscard]] const ogl::resource::Mesh& GetMapMesh() const noexcept;
        [[nodiscard]] ogl::resource::Mesh& GetMapMesh() noexcept;

        [[nodiscard]] uint32_t GetFloatCountOfMap() const;

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

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        void StoreTileTypeTextures();
        void CreateTiles();
        void StoreTileNeighbours();
        void CreateRandomColors();
        void CreateNavigationNodes();
        void LinkTileNavigationNodes();
        void CreateNavigationNodesMeshes();

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        [[nodiscard]] int32_t GetVerticesCountOfMap() const;

        //-------------------------------------------------
        // Vbo
        //-------------------------------------------------

        void CreateVbo();
        void StoreTilesInVbo();
    };
};
