// This file is part of the SgCityBuilder package.
// 
// Filename: AutoNode.cpp
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#include "AutoNode.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

sg::city::automata::AutoNode::AutoNode()
{
}

sg::city::automata::AutoNode::AutoNode(const int t_tileIndex, const glm::vec3& t_position)
    : position{ t_position }
{
    tiles.push_back(t_tileIndex);
}

sg::city::automata::AutoNode::~AutoNode() noexcept
{
}
