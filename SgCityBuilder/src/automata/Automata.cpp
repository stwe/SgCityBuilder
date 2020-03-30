// This file is part of the SgCityBuilder package.
// 
// Filename: Automata.cpp
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

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

    autoPosition += t_dt * 0.5f;

    if (autoPosition >= currentTrack->trackLength)
    {
        if (!exitNode->block)
        {
            autoPosition -= currentTrack->trackLength;

            std::shared_ptr<AutoTrack> newAutoTrack;

            if (exitNode->autoTracks.size() == 1)
            {
                autoPosition = currentTrack->trackLength;
                SG_OGL_LOG_WARN("No more tracks.");
                deleteAutomata = true;
                return;
            }

            if (exitNode->autoTracks.size() == 2)
            {
                auto it{ exitNode->autoTracks.begin() };
                newAutoTrack = *it;
                if (currentTrack == newAutoTrack)
                {
                    ++it;
                    newAutoTrack = *it;
                }
            }
            else
            {
                while (!newAutoTrack)
                {
                    // get a random value in range 0...size
                    const auto i{ rand() % static_cast<int>(exitNode->autoTracks.size()) };
                    auto j{ 0 };

                    for (auto it{ exitNode->autoTracks.begin() }; it != exitNode->autoTracks.end(); ++it)
                    {
                        // get the track
                        auto track{ *it };

                        // create a target
                        auto newExitNode{ track->startNode };
                        if (newExitNode == exitNode)
                        {
                            newExitNode = track->endNode;
                        }

                        if (j == i && track != currentTrack && !newExitNode->block)
                        {
                            newAutoTrack = track;
                            break;
                        }

                        j++;
                    }
                }
            }

            rootNode = exitNode;
            currentTrack->automatas.pop_front();
            currentTrack = newAutoTrack;
            currentTrack->automatas.push_back(GetShared());
        }
        else
        {
            autoPosition = currentTrack->trackLength;
        }
    }
    else
    {
        position = currentTrack->GetPosition(autoPosition, rootNode);
    }
}
