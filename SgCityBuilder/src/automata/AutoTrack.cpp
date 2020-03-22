#include "AutoTrack.h"
#include "AutoNode.h"

glm::vec3 sg::city::automata::AutoTrack::GetPosition(const float t_dt, const AutoNodeSharedPtr& t_node) const
{
    if (startNode == t_node)
    {
        return startNode->position + (endNode->position - startNode->position) * (t_dt / trackLength);
    }

    return endNode->position + (startNode->position - endNode->position) * (t_dt / trackLength);
}
