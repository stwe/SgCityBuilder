#pragma once

#include <vector>
#include <memory>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

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
        using MeshUniquePtr = std::unique_ptr<ogl::resource::Mesh>;

        using TileUniquePtr = std::unique_ptr<Tile>;
        using TileContainer = std::vector<TileUniquePtr>;

        //-------------------------------------------------
        // Puiblic member
        //-------------------------------------------------

        glm::vec3 position{ glm::vec3(0.0f) };
        glm::vec3 rotation{ glm::vec3(0.0f) };
        glm::vec3 scale{ glm::vec3(1.0f) };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Map();

        Map(const Map& t_other) = delete;
        Map(Map&& t_other) noexcept = delete;
        Map& operator=(const Map& t_other) = delete;
        Map& operator=(Map&& t_other) noexcept = delete;

        ~Map() noexcept;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        [[nodiscard]] const ogl::resource::Mesh& GetMapMesh() const noexcept;
        [[nodiscard]] ogl::resource::Mesh& GetMapMesh() noexcept;

        [[nodiscard]] const TileContainer& GetTiles() const noexcept;
        [[nodiscard]] TileContainer& GetTiles() noexcept;

        [[nodiscard]] int GetMapSize() const;

        //-------------------------------------------------
        // Create
        //-------------------------------------------------

        void CreateMap(int t_mapSize);

    protected:

    private:
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

        void CreateVbo();
        void StoreTilesInVbo();
    };
}
