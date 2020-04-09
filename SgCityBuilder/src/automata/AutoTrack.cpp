// This file is part of the SgCityBuilder package.
// 
// Filename: AutoTrack.cpp
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#include "AutoTrack.h"
#include "AutoNode.h"
#include <Log.h>

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

sg::city::automata::AutoTrack::~AutoTrack() noexcept
{
    SG_OGL_LOG_DEBUG("[AutoTrack::~AutoTrack()] Destruct AutoTrack.");
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

glm::vec3 sg::city::automata::AutoTrack::GetPosition(const float t_dt, const AutoNodeSharedPtr& t_node) const
{
    if (startNode == t_node)
    {
        return startNode->position + (endNode->position - startNode->position) * (t_dt / trackLength);
    }

    return endNode->position + (startNode->position - endNode->position) * (t_dt / trackLength);
}
