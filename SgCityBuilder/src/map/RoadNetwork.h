#pragma once

#include <memory>

namespace sg::ogl::resource
{
    class Mesh;
}

namespace sg::city::map
{
    class Map;

    class RoadNetwork
    {
    public:
        using VertexContainer = std::vector<float>;
        using MeshUniquePtr = std::unique_ptr<ogl::resource::Mesh>;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        RoadNetwork() = delete;

        explicit RoadNetwork(Map* t_map);

        RoadNetwork(const RoadNetwork& t_other) = delete;
        RoadNetwork(RoadNetwork&& t_other) noexcept = delete;
        RoadNetwork& operator=(const RoadNetwork& t_other) = delete;
        RoadNetwork& operator=(RoadNetwork&& t_other) noexcept = delete;

        ~RoadNetwork() noexcept = default;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        [[nodiscard]] const ogl::resource::Mesh& GetMesh() const noexcept;
        [[nodiscard]] ogl::resource::Mesh& GetMesh() noexcept;

        [[nodiscard]] uint32_t GetWoTextureId() const;
        [[nodiscard]] uint32_t GetNsTextureId() const;
        [[nodiscard]] uint32_t GetAllTextureId() const;

        //-------------------------------------------------
        // Create
        //-------------------------------------------------

        void StoreRoad(int t_tileIndex);
        void RemoveRoadFromVbo(int t_tileIndex);

        void Init();

    protected:

    private:
        /**
         * @brief A pointer to the parent Map.
         */
        Map* m_map{ nullptr };

        /**
         * @brief A Mesh instance for the RoadNetwork.
         */
        MeshUniquePtr m_roadNetworkMesh;

        /**
         * @brief The Id of Vbo holding the vertices.
         */
        uint32_t m_vboId{ 0 };

        /**
         * @brief Vertices of all Roads.
         */
        VertexContainer m_vertices;

        uint32_t m_woTextureId{ 0 };
        uint32_t m_nsTextureId{ 0 };
        uint32_t m_allTextureId{ 0 };

        std::vector<int> m_lookupTable;

        void CreateVbo();
    };
}
