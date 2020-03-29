#include "Automata.h"
#include "AutoTrack.h"
#include "AutoNode.h"

#include <Log.h>

void sg::city::automata::Automata::Update(const float t_dt)
{
    auto exitNode{ currentTrack->startNode };
    if (exitNode == rootNode)
    {
        exitNode = currentTrack->endNode;
    }

    auto automataCanMove{ true };

    if (automataCanMove)
    {
        autoPosition += t_dt * 0.25f;
    }

    if (autoPosition >= currentTrack->trackLength)
    {
        autoPosition -= currentTrack->trackLength;

        std::shared_ptr<AutoTrack> newAutoTrack;

        if (exitNode->autoTracks.size() == 1)
        {
            return;
        }

        if (exitNode->autoTracks.size() == 2)
        {
            newAutoTrack = *exitNode->autoTracks.begin();

            //SG_OGL_LOG_INFO("");
        }
        else
        {

        }

        rootNode = exitNode;
        currentTrack->automatas.pop_front();
        currentTrack = newAutoTrack;
        currentTrack->automatas.push_back(GetShared());
    }
    else
    {
        position = currentTrack->GetPosition(autoPosition, rootNode);
    }
}
