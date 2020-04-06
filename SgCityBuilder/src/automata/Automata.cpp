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

    /*
    SG_OGL_LOG_INFO("Root Node x: {}, z: {}", rootNode->position.x, rootNode->position.z);

    SG_OGL_LOG_INFO("Current Track - Start Node x: {}, z: {}", currentTrack->startNode->position.x, currentTrack->startNode->position.z);
    SG_OGL_LOG_INFO("Current Track - End Node x: {}, z: {}", currentTrack->endNode->position.x, currentTrack->endNode->position.z);

    SG_OGL_LOG_INFO("Exit Node x: {}, z: {}", exitNode->position.x, exitNode->position.z);
    SG_OGL_LOG_INFO("");
    */

    autoPosition += t_dt * 0.5f;

    if (autoPosition >= currentTrack->trackLength)
    {
        if (!exitNode->block)
        {
            autoPosition -= currentTrack->trackLength;

            // Ziel erreicht - es wird eine neuer Track benoetigt

            //if (exitNode->autoTracks.size() == 1)
            //    return;

            // ersten Track aus der End Node holen
            auto it{ exitNode->autoTracks.begin() };

            // Track als neuen Track setzen
            auto newAutoTrack = *it;

            // Zeigervergleich
            if (currentTrack == newAutoTrack)
            {
                ++it;
                newAutoTrack = *it;
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
