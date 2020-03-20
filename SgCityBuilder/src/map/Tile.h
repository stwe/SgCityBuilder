// This file is part of the SgCityBuilder package.
// 
// Filename: Tile.h
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#pragma once

#include <glm/vec2.hpp>
#include "Map.h"

namespace sg::city::map
{
    class Tile
    {
    public:
        enum class Directions
        {
            NORTH,
            EAST,
            SOUTH,
            WEST,
            SIZE
        };

        static constexpr auto DEFAULT_HEIGHT{ 0.0f };
        static constexpr auto DEFAULT_NORMAL{ glm::vec3(0.0f, 1.0f, 0.0f) };
        static constexpr auto FLOATS_PER_VERTEX{ 12u }; // 3x position + 3x normal + 3x color + 1x texture + 2x uv
        static constexpr auto VERTICES_PER_TILE{ 6u }; // 2 triangles with 3 vertices
        static constexpr auto FLOATS_PER_TILE{ FLOATS_PER_VERTEX * VERTICES_PER_TILE }; // = 72 floats
        static constexpr auto SIZE_IN_BYTES_PER_TILE{ FLOATS_PER_TILE * sizeof(float) }; // = 288 bytes

        static constexpr auto MAX_POPULATION{ 50 };

        using VertexContainer = std::vector<float>;
        using NeighbourContainer = std::array<Tile*, static_cast<int>(Directions::SIZE)>;
        using AutoNodeSharedPtr = std::shared_ptr<automata::AutoNode>;
        using AutoNodeContainer = std::vector<AutoNodeSharedPtr>;

        //-------------------------------------------------
        // Public member
        //-------------------------------------------------

        float population{ 0.0 };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Tile() = delete;

        Tile(float t_mapX, float t_mapZ, Map::TileType t_type);

        Tile(const Tile& t_other) = delete;
        Tile(Tile&& t_other) noexcept = delete;
        Tile& operator=(const Tile& t_other) = delete;
        Tile& operator=(Tile&& t_other) noexcept = delete;

        ~Tile() noexcept;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        [[nodiscard]] const VertexContainer& GetVerticesContainer() const noexcept;
        [[nodiscard]] VertexContainer& GetVerticesContainer() noexcept;

        [[nodiscard]] float GetMapX() const;
        [[nodiscard]] float GetMapZ() const;

        [[nodiscard]] Map::TileType GetType() const;

        [[nodiscard]] const NeighbourContainer& GetNeighbours() const noexcept;
        [[nodiscard]] NeighbourContainer& GetNeighbours() noexcept;

        [[nodiscard]] int GetRegion() const;

        /**
         * @brief Returns the center of the Tile as world position.
         * @return glm::vec3
         */
        [[nodiscard]] glm::vec3 GetCenter() const;

        [[nodiscard]] const AutoNodeContainer& GetNavigationNodes() const noexcept;
        [[nodiscard]] AutoNodeContainer& GetNavigationNodes() noexcept;

        //-------------------------------------------------
        // Setter
        //-------------------------------------------------

        void SetVertexPositions(const glm::vec3& t_bL, const glm::vec3& t_bR, const glm::vec3& t_tL, const glm::vec3& t_tR);
        void SetNormal(const glm::vec3& t_normal);
        void SetColor(const glm::vec3& t_color);
        void SetTexture(float t_texture);
        void SetUv(
            const glm::vec2& t_bL = glm::vec2(0.0f, 0.0f),
            const glm::vec2& t_bR = glm::vec2(1.0f, 0.0f),
            const glm::vec2& t_tL = glm::vec2(0.0f, 1.0f),
            const glm::vec2& t_tR = glm::vec2(1.0f, 1.0f)
        );

        void SetType(Map::TileType t_type);

        void SetRegion(int t_region);

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        void Update() const;

        //-------------------------------------------------
        // To string
        //-------------------------------------------------

        static std::string TileTypeToString(Map::TileType t_type);

    protected:

    private:
        /**
         * @brief The bottom left x-position of the Tile.
         */
        float m_mapX{ 0.0f };

        /**
         * @brief The bottom left z-position of the Tile.
         */
        float m_mapZ{ 0.0f };

        /**
         * @brief The Type of the Tile.
         */
        Map::TileType m_type{ Map::TileType::NONE };

        /**
         * @brief Vertices of the Tile.
         */
        VertexContainer m_vertices;

        /**
         * @brief The neighbors of this Tile.
         */
        NeighbourContainer m_neighbours{ nullptr, nullptr, nullptr, nullptr };

        /**
         * @brief The region Id of the Tile. Tiles in the same region are connected.
         */
        int m_region{ 0 };

        /**
         * @brief Each Tile links to multiple auto nodes.
         */
        AutoNodeContainer m_navigationNodes;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        void Create();
    };
}
