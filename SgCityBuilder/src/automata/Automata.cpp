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

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

sg::city::automata::Automata::~Automata() noexcept
{
    SG_OGL_LOG_DEBUG("[Automata::~Automata()] Destruct Automata.");
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

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

            // Ziel erreicht - es wird eine neuer Track benoetigt

            if (exitNode->autoTracks.size() == 1)
            {
                deleteAutomata = true;
                return;
            }

            // ersten Track aus der End Node holen
            auto it{ exitNode->autoTracks.begin() };

            // Track als neuen Track setzen
            auto newAutoTrack = *it;

            if (currentTrack == newAutoTrack)
            {
                ++it;
                newAutoTrack = *it;
            }

            rootNode = exitNode;
            currentTrack->automatas.pop_front();
            currentTrack = newAutoTrack;
            currentTrack->automatas.push_back(this);
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
