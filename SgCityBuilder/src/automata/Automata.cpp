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

    auto canMove{ true };
    auto distanceToAutomataInFront{ 1.0f };

    /*

       N2  Start Node
       |
       |
       A0
       |   Current Track 0
       |
       |
       N1  Exit Node ------------- link to Track 0 und Track 1
       |
       |
       |   Track 1
       A1
       |
       |
       N0

    */

    /*

         A0 - StartNode N2 ---- Track 0
            - EndeNode  N1 ----
                              | Track 0 und 1
         A1 - StartNode N1 ----
            - EndeNode  N0 ---- Track 0

    */


    // zu dem momentanen Track koennen noch andere Autos gehoeren
    // Frage: ist dieses Auto ganz vorne in der Liste?

    // get an iterator for this automata
    const auto itThisAutomata = std::find(currentTrack->automatas.begin(), currentTrack->automatas.end(), this);

    // dieses Auto ist ganz vorne
    if (*itThisAutomata == currentTrack->automatas.front()) // fuer A0 und A1 true
    {
        for (auto& track : exitNode->autoTracks) // A0 = currentTrack + Track 1   // A1 = nur der currentTrack
        {
            if (track != currentTrack && !track->automatas.empty()) // wird nur von A0 durchlaufen
            {
                // A0 und Track 1

                // das Ende (letztes eingefuegtes Fahrzeug) des vorderen Tracks holen und den Abstand messen
                // autoPosition      : 0......1
                // autoLength immer  : 0.2
                auto distanceFromAutomataExitNode{ track->automatas.back()->autoPosition - track->automatas.back()->autoLength };

                // Am Anfang ist der Abstand negativ

                if (distanceFromAutomataExitNode < 0.0f)
                {
                    distanceFromAutomataExitNode = 0.0f;
                }

                // jetzt wissen wir, dass sich vor uns ein Track mit einem anderen Fahrzeug befindet und wissen den Abstand
                // dieses anderen Fahrzeugs zu unserer Exit Node (bzw. der StartNode des anderen Fahrzeugs)

                //SG_OGL_LOG_INFO("Abstand zur exit node: {}", distanceFromAutomataExitNode);

                //   0.4 < (1.0 + 0.9 - 0.2)

                //   meine eigene auto position     < (track length              + distanceFromAutomataExitNode - autoLength)
                if ((*itThisAutomata)->autoPosition < (currentTrack->trackLength + distanceFromAutomataExitNode - autoLength))
                {
                    distanceToAutomataInFront = (currentTrack->trackLength + distanceFromAutomataExitNode - 0.1f) - (*itThisAutomata)->autoPosition;
                }
                else
                {
                    canMove = false;
                }
            }
        }
    }
    else
    {
        // der Automat ist auf dem Track nicht ganz vorn
        auto itAutomataInFront = itThisAutomata;

        // hole den Automaten vor diesem
        itAutomataInFront--;

        // itAutomataInFront Listenindex jetzt zB: 0
        // itThisAutomata    Listenindex jetzt zB: 1

        //                             Abstand zum vorderen Fahrzeug zB  = 0.4             > 0.3 ? Abstand zum vorderen Fahreug ok => Automat bewegen
        //                             [0] = 0.5       - [1] = 0.1       = 0.4             > 0.2 + 0.1

        if (fabs((*itAutomataInFront)->autoPosition - (*itThisAutomata)->autoPosition) > (*itAutomataInFront)->autoLength + 0.1f)
        {
            // abstand zum vorderen Fahrzeug groesser als 0.3

            //                                                         0.5 - 0.2 - 0.1   - 0.1   =>   distance 0.1  (nicht 0.4, da Abzug von autoLength und Konstante 0.1)

            // move Automata along track
            distanceToAutomataInFront = (*itAutomataInFront)->autoPosition - (*itAutomataInFront)->autoLength - 0.1f - (*itThisAutomata)->autoPosition;
            // => 0.1 in disem Beispiel
        }
        else
        {
            canMove = false;
        }
    }

    if (canMove)
    {
        // distance nicht groesser als 1
        if (distanceToAutomataInFront > currentTrack->trackLength)
        {
            distanceToAutomataInFront = currentTrack->trackLength;
        }

        // meistens: autoPosition += t_dt * 1.0 * 0.25
        autoPosition += t_dt * std::max(distanceToAutomataInFront, 1.0f) * (autoLength < 0.1f ? 0.15f : 0.25f);

        //autoPosition += t_dt * 0.125f;
    }

    if (autoPosition >= currentTrack->trackLength)
    {
        if (!exitNode->block)
        {
            autoPosition -= currentTrack->trackLength;

            std::shared_ptr<AutoTrack> newTrack;

            // es existiert nur noch ein Track und das ist der momentane Track
            if (exitNode->autoTracks.size() == 1 && *exitNode->autoTracks.begin() == currentTrack)
            {
                // Automata zum loeschen markieren
                deleteAutomata = true;
                return;
            }

            // es existieren zwei Tracks
            if (exitNode->autoTracks.size() == 2)
            {
                // ersten Track aus der EndNode holen
                auto it{ exitNode->autoTracks.begin() };

                // Track als neuen Track setzen
                newTrack = *it;

                // falls es sich um den momentanen Track handelt, den anderen nehmen
                if (currentTrack == newTrack)
                {
                    ++it;
                    newTrack = *it;
                }
            }
            else // mehr als zwei Tracks
            {
                while (!newTrack)
                {
                    // Zufallszahl zwischen 0 und der Anzahl Tracks
                    const auto randomTrackIndex{ rand() % exitNode->autoTracks.size() };
                    auto j{ 0u };

                    // fuer alle Tracks in der EndNode
                    for (auto it = exitNode->autoTracks.begin(); it != exitNode->autoTracks.end(); ++it)
                    {
                        // deref Iterator
                        auto track = (*it);

                        // neue EndNode aus dem Track ermitteln
                        auto newExitNode = track->startNode;
                        if (newExitNode == exitNode)
                        {
                            newExitNode = track->endNode;
                        }

                        // => neuer Track mit neuer EndNode benutzen, wenn j stimmt
                        if (j == randomTrackIndex && track != currentTrack && !newExitNode->block)
                        {
                            newTrack = track;
                            break;
                        }

                        j++; // noch nicht der richtige Track
                    }
                }
            }

            // EndNode als neue StartNode benutzen
            rootNode = exitNode;

            // Automaten vorne aus der Liste loeschen
            currentTrack->automatas.pop_front();

            // neuen Track als aktuellen Track benutzen
            currentTrack = newTrack;

            // Automaten hinten wieder anfuegen
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
