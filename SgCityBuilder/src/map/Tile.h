#pragma once

#include <glm/vec3.hpp>
#include "Map.h"

namespace sg::city::map
{
    class Tile
    {
    public:
        static constexpr auto DEFAULT_HEIGHT{ 0.0f };
        static constexpr auto DEFAULT_NORMAL{ glm::vec3(0.0f, 1.0f, 0.0f) };
        static constexpr auto FLOATS_PER_VERTEX{ 10u }; // 3x position + 3x normal + 3x color + 1x texture
        static constexpr auto VERTICES_PER_TILE{ 6u }; // 2 triangles with 3 vertices
        static constexpr auto FLOATS_PER_TILE{ FLOATS_PER_VERTEX * VERTICES_PER_TILE }; // = 60 floats
        static constexpr auto SIZE_IN_BYTES_PER_TILE{ FLOATS_PER_TILE * sizeof(float) }; // = 240 bytes

        using VertexContainer = std::vector<float>;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Tile() = delete;

        Tile(
            Map* t_map,
            float t_mapX,
            float t_mapZ,
            Map::TileType t_type
        );

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

        //-------------------------------------------------
        // Setter
        //-------------------------------------------------

        void ChangeTypeTo(Map::TileType t_type);

        //-------------------------------------------------
        // To string
        //-------------------------------------------------

        static std::string TileTypeToString(Map::TileType t_type);

    protected:

    private:
        /**
         * @brief A pointer to the parent Map.
         */
        Map* m_map{ nullptr };

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

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        void Init();
    };
}
