#pragma once

#include <vector>
#include <array>
#include <glm/vec3.hpp>

namespace sg::city::map
{
    class Map;

    class Tile
    {
    public:
        static constexpr auto DEFAULT_HEIGHT{ 0.0f };
        static constexpr auto DEFAULT_NORMAL{ glm::vec3(0.0f, 1.0f, 0.0f) };
        static constexpr auto FLOATS_PER_VERTEX{ 9u }; // 3x position + 3x normal + 3x color
        static constexpr auto VERTICES_PER_TILE{ 6u }; // 2 triangles with 3 vertices
        static constexpr auto FLOATS_PER_TILE{ FLOATS_PER_VERTEX * VERTICES_PER_TILE }; // = 54 floats
        static constexpr auto SIZE_IN_BYTES_PER_TILE{ FLOATS_PER_TILE * sizeof(float) }; // = 216 bytes

        using VertexContainer = std::vector<float>;

        enum Type
        {
            NONE,
            RESIDENTIAL,
            COMMERCIAL,
            INDUSTRIAL,
            TRAFFIC_NETWORK,
            SIZE
        };

        static constexpr std::array<glm::vec3, SIZE> TYPE_COLOR{
            glm::vec3(0.0f, 0.0f, 0.0f), // black:  None
            glm::vec3(0.0f, 0.9f, 0.0f), // green:  Residential
            glm::vec3(0.0f, 0.0f, 0.8f), // blue:   Commercial
            glm::vec3(0.8f, 0.8f, 0.0f), // yellow: Industrial
            glm::vec3(0.8f, 0.8f, 0.8f), // white:  Traffic Network
        };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Tile() = delete;

        Tile(
            float t_mapX,
            float t_mapZ,
            Type t_type
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

        void SetParentMap(Map* t_map);
        void SetType(Type t_type);

        //-------------------------------------------------
        // To string
        //-------------------------------------------------

        static std::string TileTypeToString(Type t_type);

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
        Type m_type{ NONE };

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
