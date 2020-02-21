#include <Log.h>
#include "Astar.h"
#include "Map.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

sg::city::map::Astar::Astar(Map* t_map)
    : m_map{ t_map }
{
    Init();
}

//-------------------------------------------------
// Find
//-------------------------------------------------

bool sg::city::map::Astar::FindPath(const int t_startTileIndex, const int t_endTileIndex)
{
    CreateMarker(t_startTileIndex, t_endTileIndex);

    ResetNavigationGraph();

    // Setup starting conditions.
    auto* currentNode{ m_nodeStart };
    m_nodeStart->g = 0.0f;
    m_nodeStart->h = EuclideanDistance(m_nodeStart, m_nodeEnd);

    // Add start node to openlist - this will ensure it gets tested.
    std::list<Node*> openlist;
    openlist.push_back(m_nodeStart);

    // If the openlist contains nodes, there may be better paths
    // which have not yet been explored. However, we will also stop 
    // searching when we reach the target - there may well be better
    // paths but this one will do - it wont be the longest.
    while (!openlist.empty() && currentNode != m_nodeEnd)
    {
        // Sort openlist nodes by heuristic, so lowest is first.
        openlist.sort([](const Node* t_lhs, const Node* t_rhs) { return t_lhs->h < t_rhs->h; });

        // Front of openlist is potentially the lowest distance node. Our
        // list may also contain nodes that have been visited.
        // These are now removed.
        while (!openlist.empty() && openlist.front()->visited)
        {
            openlist.pop_front();
        }

        // It is canceled when there are no more valid nodes - the list is empty.
        if (openlist.empty())
        {
            break;
        }

        // Get the new current node from the openlist.
        currentNode = openlist.front();

        // We only explore a node once.
        currentNode->visited = true;

        // Check each of this node's neighbours.
        for (auto* nodeNeighbour : currentNode->neighbours)
        {
            // Only if the neighbour is not visited and is 
            // not an obstacle, add it to the openlist.
            if (!nodeNeighbour->visited && !nodeNeighbour->obstacle)
            {
                openlist.push_back(nodeNeighbour);
            }

            // Calculate the neighbours potential lowest parent distance.
            const auto possiblyLowerGoal{ currentNode->g + EuclideanDistance(currentNode, nodeNeighbour) };

            // If choosing to path through this node is a lower distance than what 
            // the neighbour currently has set, update the neighbour to use this node
            // as the path source, and set its distance scores as necessary.
            if (possiblyLowerGoal < nodeNeighbour->g)
            {
                nodeNeighbour->parent = currentNode;
                nodeNeighbour->g = possiblyLowerGoal;

                // The best path length to the neighbour being tested has changed, so
                // update the neighbour's score. The heuristic is used to globally bias
                // the path algorithm, so it knows if its getting better or worse. At some
                // point the algo will realise this path is worse and abandon it, and then go
                // and search along the next best path.
                nodeNeighbour->h = nodeNeighbour->g + EuclideanDistance(nodeNeighbour, m_nodeEnd);
            }
        }
    }

    if (m_nodeEnd != nullptr)
    {
        auto* p = m_nodeEnd;
        while (p->parent != nullptr)
        {
            SG_OGL_LOG_DEBUG("Node x: {}, z: {}", p->mapX, p->mapZ);
            /*
            DrawLine(p->x * nNodeSize + nNodeSize / 2, p->y * nNodeSize + nNodeSize / 2,
                p->parent->x * nNodeSize + nNodeSize / 2, p->parent->y * nNodeSize + nNodeSize / 2, PIXEL_SOLID, FG_YELLOW);
            */

            // Set next node to this node's parent
            p = p->parent;
        }
    }

    return true;
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void sg::city::map::Astar::Init()
{
    CreateNodes();
    CreateNeighbours();
}

void sg::city::map::Astar::CreateNodes()
{
    const auto mapSize{ m_map->GetMapSize() };
    for (auto z{ 0 }; z < mapSize; ++z)
    {
        for (auto x{ 0 }; x < mapSize; ++x)
        {
            auto* node{ new Node };
            node->mapX = static_cast<float>(x);
            node->mapZ = static_cast<float>(z);
            node->obstacle = false;
            node->parent = nullptr;
            node->visited = false;
            m_nodes.push_back(node);
        }
    }
}

void sg::city::map::Astar::CreateNeighbours() const
{
    const auto mapSize{ m_map->GetMapSize() };
    for (auto z{ 0 }; z < mapSize; ++z)
    {
        for (auto x{ 0 }; x < mapSize; ++x)
        {
            if (z > 0)
            {
                m_nodes[z * mapSize + x]->neighbours.push_back(m_nodes[(z - 1) * mapSize + (x + 0)]);
            }

            if (z < mapSize - 1)
            {
                m_nodes[z * mapSize + x]->neighbours.push_back(m_nodes[(z + 1) * mapSize + (x + 0)]);
            }

            if (x > 0)
            {
                m_nodes[z * mapSize + x]->neighbours.push_back(m_nodes[(z + 0) * mapSize + (x - 1)]);
            }

            if (x < mapSize - 1)
            {
                m_nodes[z * mapSize + x]->neighbours.push_back(m_nodes[(z + 0) * mapSize + (x + 1)]);
            }
        }
    }
}

void sg::city::map::Astar::CreateMarker(const int t_startTileIndex, const int t_endTileIndex)
{
    m_nodeStart = m_nodes[t_startTileIndex];
    m_nodeEnd = m_nodes[t_endTileIndex];
}

void sg::city::map::Astar::ResetNavigationGraph()
{
    const auto mapSize{ m_map->GetMapSize() };
    for (auto z{ 0 }; z < mapSize; ++z)
    {
        for (auto x{ 0 }; x < mapSize; ++x)
        {
            m_nodes[z * mapSize + x]->visited = false;
            m_nodes[z * mapSize + x]->g = INFINITY;
            m_nodes[z * mapSize + x]->h = INFINITY;
            m_nodes[z * mapSize + x]->parent = nullptr;
        }
    }
}

//-------------------------------------------------
// Heuristics
//-------------------------------------------------

float sg::city::map::Astar::ManhattenDistance(Node* t_aNode, Node* t_bNode, const float t_d) const
{
    const auto deltaX{ abs(t_aNode->mapX - t_bNode->mapX) };
    const auto deltaZ{ abs(t_aNode->mapZ - t_bNode->mapZ) };

    return t_d * (deltaX + deltaZ);
}

float sg::city::map::Astar::EuclideanDistance(Node* t_aNode, Node* t_bNode, const float t_d) const
{
    const auto deltaX{ abs(t_aNode->mapX - t_bNode->mapX) };
    const auto deltaZ{ abs(t_aNode->mapZ - t_bNode->mapZ) };

    return t_d * sqrtf(deltaX * deltaX + deltaZ * deltaZ);
}
