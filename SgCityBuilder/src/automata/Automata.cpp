#include "Automata.h"
#include "AutoTrack.h"
#include "AutoNode.h"

void sg::city::automata::Automata::Update(const float t_dt)
{
    // get the target AutoNode
    auto exitNode{ currentTrack->startNode };
    if (exitNode == originNode)
    {
        exitNode = currentTrack->endNode;
    }

    auto automataCanMove{ true };

    if (automataCanMove)
    {
        autoPos += t_dt * 0.25f;
    }

    if (autoPos >= currentTrack->trackLength)
    {
        if (!exitNode->block)
        {
            autoPos -= currentTrack->trackLength;

            std::shared_ptr<AutoTrack> newAutoTrack;

            if (exitNode->autoTracks.size() == 2)
            {
                
            }
            else
            {
                while (!newAutoTrack)
                {
                    int i{ (int)(rand() % exitNode->autoTracks.size()) };
                    int j{ 0 };
                    for (auto it{exitNode->autoTracks.begin()}; it != exitNode->autoTracks.end(); ++it)
                    {
                        auto autoTrack{ *it };

                        auto newExitNode{ autoTrack->startNode };
                        if (newExitNode == exitNode)
                        {
                            newExitNode = autoTrack->endNode;
                        }

                        if (j == i && autoTrack != currentTrack && !newExitNode->block)
                        {
                            newAutoTrack = autoTrack;
                            break;
                        }

                        j++;
                    }
                }
            }

            originNode = exitNode;
            currentTrack->automatas.pop_front();
            currentTrack = newAutoTrack;
            currentTrack->automatas.push_back(GetShared());
        }
        else
        {
            autoPos = currentTrack->trackLength;
        }

    }
    else
    {
        pos = currentTrack->GetPosition(autoPos, originNode);
    }
}
