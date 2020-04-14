// This file is part of the SgCityBuilder package.
// 
// Filename: Tile.h
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <memory>

namespace sg::ogl::resource
{
    class Mesh;
}

namespace sg::city::map
{
    class Map;
}

namespace sg::city::map::tile
{
    enum class TileType
    {
        NONE,
        RESIDENTIAL,
        COMMERCIAL,
        INDUSTRIAL,
        TRAFFIC
    };

    struct TileTypeHash
    {
        std::size_t operator()(TileType t_tileType) const
        {
            return static_cast<std::size_t>(t_tileType);
        }
    };

    enum class Direction
    {
        NORTH,
        EAST,
        SOUTH,
        WEST
    };

    struct DirectionHash
    {
        std::size_t operator()(Direction t_direction) const
        {
            return static_cast<std::size_t>(t_direction);
        }
    };

    class Tile
    {
    public:
        using VertexContainer = std::vector<float>;
        using NeighbourContainer = std::unordered_map<Direction, int, DirectionHash>;
        using MeshUniquePtr = std::unique_ptr<ogl::resource::Mesh>;

        //-------------------------------------------------
        // Const
        //-------------------------------------------------

        static constexpr auto DEFAULT_HEIGHT{ 0.0f };
        static constexpr auto DEFAULT_NORMAL{ glm::vec3(0.0f, 1.0f, 0.0f) };
        static constexpr auto FLOATS_PER_VERTEX{ 12u }; // 3x position + 3x normal + 3x color + 1x texture + 2x uv
        static constexpr auto VERTICES_PER_TILE{ 6u };  // 2 triangles with 3 vertices
        static constexpr auto FLOATS_PER_TILE{ FLOATS_PER_VERTEX * VERTICES_PER_TILE };         // = 72 floats
        static constexpr auto SIZE_IN_BYTES_PER_TILE{ FLOATS_PER_TILE * sizeof(float) }; // = 288 bytes

        /**
         * @brief The default max population value.
         */
        static constexpr auto MAX_POPULATION{ 50 };

        /**
         * @brief The value in case the Tile does not belong to any region.
         */
        static constexpr auto NO_REGION{ 0 };

        // Bottom left T1

        static constexpr auto BOTTOM_LEFT_POS_X_T1{ 0 };
        static constexpr auto BOTTOM_LEFT_POS_Y_T1{ 1 };
        static constexpr auto BOTTOM_LEFT_POS_Z_T1{ 2 };

        static constexpr auto BOTTOM_LEFT_NORMAL_X_T1{ 3 };
        static constexpr auto BOTTOM_LEFT_NORMAL_Y_T1{ 4 };
        static constexpr auto BOTTOM_LEFT_NORMAL_Z_T1{ 5 };

        static constexpr auto BOTTOM_LEFT_COLOR_X_T1{ 6 };
        static constexpr auto BOTTOM_LEFT_COLOR_Y_T1{ 7 };
        static constexpr auto BOTTOM_LEFT_COLOR_Z_T1{ 8 };

        static constexpr auto BOTTOM_LEFT_TEXTURE_NR_T1{ 9 };

        static constexpr auto BOTTOM_LEFT_TEXTURE_X_T1{ 10 };
        static constexpr auto BOTTOM_LEFT_TEXTURE_Y_T1{ 11 };

        // Bottom right T1

        static constexpr auto BOTTOM_RIGHT_POS_X_T1{ 12 };
        static constexpr auto BOTTOM_RIGHT_POS_Y_T1{ 13 };
        static constexpr auto BOTTOM_RIGHT_POS_Z_T1{ 14 };

        static constexpr auto BOTTOM_RIGHT_NORMAL_X_T1{ 15 };
        static constexpr auto BOTTOM_RIGHT_NORMAL_Y_T1{ 16 };
        static constexpr auto BOTTOM_RIGHT_NORMAL_Z_T1{ 17 };

        static constexpr auto BOTTOM_RIGHT_COLOR_X_T1{ 18 };
        static constexpr auto BOTTOM_RIGHT_COLOR_Y_T1{ 19 };
        static constexpr auto BOTTOM_RIGHT_COLOR_Z_T1{ 20 };

        static constexpr auto BOTTOM_RIGHT_TEXTURE_NR_T1{ 21 };

        static constexpr auto BOTTOM_RIGHT_TEXTURE_X_T1{ 22 };
        static constexpr auto BOTTOM_RIGHT_TEXTURE_Y_T1{ 23 };

        // Top left T1

        static constexpr auto TOP_LEFT_POS_X_T1{ 24 };
        static constexpr auto TOP_LEFT_POS_Y_T1{ 25 };
        static constexpr auto TOP_LEFT_POS_Z_T1{ 26 };

        static constexpr auto TOP_LEFT_NORMAL_X_T1{ 27 };
        static constexpr auto TOP_LEFT_NORMAL_Y_T1{ 28 };
        static constexpr auto TOP_LEFT_NORMAL_Z_T1{ 29 };

        static constexpr auto TOP_LEFT_COLOR_X_T1{ 30 };
        static constexpr auto TOP_LEFT_COLOR_Y_T1{ 31 };
        static constexpr auto TOP_LEFT_COLOR_Z_T1{ 32 };

        static constexpr auto TOP_LEFT_TEXTURE_NR_T1{ 33 };

        static constexpr auto TOP_LEFT_TEXTURE_X_T1{ 34 };
        static constexpr auto TOP_LEFT_TEXTURE_Y_T1{ 35 };

        // Top left T2

        static constexpr auto TOP_LEFT_POS_X_T2{ 36 };
        static constexpr auto TOP_LEFT_POS_Y_T2{ 37 };
        static constexpr auto TOP_LEFT_POS_Z_T2{ 38 };

        static constexpr auto TOP_LEFT_NORMAL_X_T2{ 39 };
        static constexpr auto TOP_LEFT_NORMAL_Y_T2{ 40 };
        static constexpr auto TOP_LEFT_NORMAL_Z_T2{ 41 };

        static constexpr auto TOP_LEFT_COLOR_X_T2{ 42 };
        static constexpr auto TOP_LEFT_COLOR_Y_T2{ 43 };
        static constexpr auto TOP_LEFT_COLOR_Z_T2{ 44 };

        static constexpr auto TOP_LEFT_TEXTURE_NR_T2{ 45 };

        static constexpr auto TOP_LEFT_TEXTURE_X_T2{ 46 };
        static constexpr auto TOP_LEFT_TEXTURE_Y_T2{ 47 };

        // Bottom right T2

        static constexpr auto BOTTOM_RIGHT_POS_X_T2{ 48 };
        static constexpr auto BOTTOM_RIGHT_POS_Y_T2{ 49 };
        static constexpr auto BOTTOM_RIGHT_POS_Z_T2{ 50 };

        static constexpr auto BOTTOM_RIGHT_NORMAL_X_T2{ 51 };
        static constexpr auto BOTTOM_RIGHT_NORMAL_Y_T2{ 52 };
        static constexpr auto BOTTOM_RIGHT_NORMAL_Z_T2{ 53 };

        static constexpr auto BOTTOM_RIGHT_COLOR_X_T2{ 54 };
        static constexpr auto BOTTOM_RIGHT_COLOR_Y_T2{ 55 };
        static constexpr auto BOTTOM_RIGHT_COLOR_Z_T2{ 56 };

        static constexpr auto BOTTOM_RIGHT_TEXTURE_NR_T2{ 57 };

        static constexpr auto BOTTOM_RIGHT_TEXTURE_X_T2{ 58 };
        static constexpr auto BOTTOM_RIGHT_TEXTURE_Y_T2{ 59 };

        // Top right T2

        static constexpr auto TOP_RIGHT_POS_X_T2{ 60 };
        static constexpr auto TOP_RIGHT_POS_Y_T2{ 61 };
        static constexpr auto TOP_RIGHT_POS_Z_T2{ 62 };

        static constexpr auto TOP_RIGHT_NORMAL_X_T2{ 63 };
        static constexpr auto TOP_RIGHT_NORMAL_Y_T2{ 64 };
        static constexpr auto TOP_RIGHT_NORMAL_Z_T2{ 65 };

        static constexpr auto TOP_RIGHT_COLOR_X_T2{ 66 };
        static constexpr auto TOP_RIGHT_COLOR_Y_T2{ 67 };
        static constexpr auto TOP_RIGHT_COLOR_Z_T2{ 68 };

        static constexpr auto TOP_RIGHT_TEXTURE_NR_T2{ 69 };

        static constexpr auto TOP_RIGHT_TEXTURE_X_T2{ 70 };
        static constexpr auto TOP_RIGHT_TEXTURE_Y_T2{ 71 };

        /**
         * @brief Tile color.
         */
        inline static const std::unordered_map<TileType, glm::vec3, TileTypeHash> TILE_TYPE_COLOR
        {
            { TileType::NONE, glm::vec3(0.0f, 0.5f, 0.0f) },
            { TileType::RESIDENTIAL, glm::vec3(0.0f, 0.8f, 0.0f) },
            { TileType::COMMERCIAL, glm::vec3(0.0f, 0.0f, 0.8f) },
            { TileType::INDUSTRIAL, glm::vec3(0.8f, 0.8f, 0.0f) },
            { TileType::TRAFFIC, glm::vec3(0.8f, 0.8f, 0.8f) }
        };

        /**
         * @brief Tile types that can build regions.
         */
        inline static const std::vector<TileType> REGION_TILE_TYPES
        {
            TileType::RESIDENTIAL,
            TileType::COMMERCIAL,
            TileType::INDUSTRIAL,
            TileType::TRAFFIC
        };

        //-------------------------------------------------
        // Public member
        //-------------------------------------------------

        /**
         * @brief The type of the Tile.
         */
        TileType type{ TileType::NONE };

        /**
         * @brief Current residents / employees.
         */
        float population{ 0.0 };

        /**
         * @brief The region Id of the Tile. Tiles in the same region are connected.
         */
        int region{ NO_REGION };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Tile() = delete;

        Tile(float t_mapX, float t_mapZ, TileType t_type, Map* t_map);

        Tile(const Tile& t_other) = delete;
        Tile(Tile&& t_other) noexcept = delete;
        Tile& operator=(const Tile& t_other) = delete;
        Tile& operator=(Tile&& t_other) noexcept = delete;

        virtual ~Tile() noexcept;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        /**
         * @brief Return the bottom left Map-x position of the Tile in Object Space.
         * @return int
         */
        [[nodiscard]] int GetMapX() const;

        /**
         * @brief Return the bottom left Map-z position of the Tile in Object Space.
         * @return int
         */
        [[nodiscard]] int GetMapZ() const;

        /**
         * @brief Return the bottom left x-position of the Tile in World Space.
         * @return float
         */
        [[nodiscard]] float GetWorldX() const;

        /**
         * @brief Return the bottom left z-position of the Tile in World Space.
         * @return float
         */
        [[nodiscard]] float GetWorldZ() const;

        /**
         * @brief Return the bottom left position of the Tile in World Space.
         * @return glm::vec3
         */
        [[nodiscard]] glm::vec3 GetWorldPosition() const;

        /**
         * @brief Return the center position of the Tile in World Space.
         * @return glm::vec3
         */
        [[nodiscard]] glm::vec3 GetWorldCenter() const;

        [[nodiscard]] const VertexContainer& GetVertices() const noexcept;
        [[nodiscard]] VertexContainer& GetVertices() noexcept;

        [[nodiscard]] const NeighbourContainer& GetNeighbours() const noexcept;
        [[nodiscard]] NeighbourContainer& GetNeighbours() noexcept;

        /**
         * @brief For convenience.
         *        Returns the own position (index) in the Tiles std::vector of the Map.
         * @return int
         */
        [[nodiscard]] int GetMapIndex() const;

        //-------------------------------------------------
        // Setter
        //-------------------------------------------------

        void SetVertexPositions();
        void SetNormal(const glm::vec3& t_normal);
        void SetColor(const glm::vec3& t_color);
        void SetTexture(float t_texture);
        void SetUv(
            const glm::vec2& t_bL = glm::vec2(0.0f, 0.0f),
            const glm::vec2& t_bR = glm::vec2(1.0f, 0.0f),
            const glm::vec2& t_tL = glm::vec2(0.0f, 1.0f),
            const glm::vec2& t_tR = glm::vec2(1.0f, 1.0f)
            );

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        virtual void Update();

        //-------------------------------------------------
        // To string
        //-------------------------------------------------

        static std::string TileTypeToString(TileType t_type);

    protected:
        /**
         * @brief Pointer to the parent Map.
         */
        Map* m_map{ nullptr };

        /**
         * @brief The neighbors of this Tile.
         */
        NeighbourContainer m_neighbours;

    private:
        /**
         * @brief The bottom left (Object Space) Map-x position of the Tile.
         */
        float m_mapX{ 0.0f };

        /**
         * @brief The bottom left (Object Space) Map-z position of the Tile.
         */
        float m_mapZ{ 0.0f };

        /**
         * @brief Bottom left position of the Tile in World Space.
         */
        glm::vec3 m_bottomLeft{ glm::vec3(0.0f) };

        /**
         * @brief Bottom right position of the Tile in World Space.
         */
        glm::vec3 m_bottomRight{ glm::vec3(0.0f) };

        /**
         * @brief Top left position of the Tile in World Space.
         */
        glm::vec3 m_topLeft{ glm::vec3(0.0f) };

        /**
         * @brief Top right position of the Tile in World Space.
         */
        glm::vec3 m_topRight{ glm::vec3(0.0f) };

        /**
         * @brief Vertices of the Tile.
         */
        VertexContainer m_vertices;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        void Create();
    };
}
