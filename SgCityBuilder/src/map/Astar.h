#pragma once

#include <vector>

namespace sg::city::map
{
    class Map;

    class Astar
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        explicit Astar(Map* t_map);

        Astar(const Astar& t_other) = delete;
        Astar(Astar&& t_other) noexcept = delete;
        Astar& operator=(const Astar& t_other) = delete;
        Astar& operator=(Astar&& t_other) noexcept = delete;

        ~Astar() = default;

        //-------------------------------------------------
        // Find
        //-------------------------------------------------

        bool FindPath(int t_startTileIndex, int t_endTileIndex);

    protected:

    private:
        struct Node
        {
            bool obstacle{ false };
            bool visited{ false };

            float g{ INFINITY };
            float h{ INFINITY };

            float mapX{ 0.0 };
            float mapZ{ 0.0 };

            std::vector<Node*> neighbours;
            Node* parent{ nullptr };
        };

        Map* m_map{ nullptr };

        std::vector<Node*> m_nodes;

        Node* m_nodeStart{ nullptr };
        Node* m_nodeEnd{ nullptr };

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        void Init();

        void CreateNodes();
        void CreateNeighbours() const;
        void CreateMarker(int t_startTileIndex, int t_endTileIndex);
        void ResetNavigationGraph();

        //-------------------------------------------------
        // Heuristics
        //-------------------------------------------------

        /**
         * @brief The standard heuristic for a square grid.
         * @param t_aNode The start Node.
         * @param t_bNode The end Node.
         * @param t_d The minimum cost for moving from one space to an adjacent space.
         * @return The distance.
         */
        float ManhattenDistance(Node* t_aNode, Node* t_bNode, float t_d = 1.0f) const;

        /**
         * @brief Units can move at any angle.
         * @param t_aNode The start Node.
         * @param t_bNode The end Node.
         * @param t_d The minimum cost for moving from one space to an adjacent space.
         * @return The distance.
         */
        float EuclideanDistance(Node* t_aNode, Node* t_bNode, float t_d = 1.0f) const;
    };
}
