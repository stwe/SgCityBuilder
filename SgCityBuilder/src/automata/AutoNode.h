// This file is part of the SgCityBuilder package.
// 
// Filename: AutoNode.h
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#pragma once

#include <glm/vec3.hpp>
#include <list>
#include <memory>

namespace sg::city::automata
{
    class AutoTrack;

    class AutoNode
    {
    public:
        using AutoTrackSharedPtr = std::shared_ptr<AutoTrack>;
        using AutoTrackContainer = std::list<AutoTrackSharedPtr>;

        //-------------------------------------------------
        // Public member
        //-------------------------------------------------

        glm::vec3 position{ glm::vec3(0.0f) };
        bool block{ false };
        AutoTrackContainer autoTracks;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        AutoNode();

        explicit AutoNode(const glm::vec3& t_position);

        AutoNode(const AutoNode& t_other) = delete;
        AutoNode(AutoNode&& t_other) noexcept = delete;
        AutoNode& operator=(const AutoNode& t_other) = delete;
        AutoNode& operator=(AutoNode&& t_other) noexcept = delete;

        ~AutoNode() noexcept;

    protected:

    private:

    };
}
