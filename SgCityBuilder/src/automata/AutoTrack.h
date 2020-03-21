// This file is part of the SgCityBuilder package.
// 
// Filename: AutoTrack.h
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#pragma once

#include <memory>

namespace sg::city::map
{
    class Tile;
}

namespace sg::city::automata
{
    class AutoNode;

    class AutoTrack
    {
    public:
        using AutoNodeSharedPtr = std::shared_ptr<AutoNode>;

        //-------------------------------------------------
        // Public member
        //-------------------------------------------------

        AutoNodeSharedPtr startNode;
        AutoNodeSharedPtr endNode;

        map::Tile* tile{ nullptr };

        float trackLength{ 1.0f };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        AutoTrack() = default;

        AutoTrack(const AutoTrack& t_other) = delete;
        AutoTrack(AutoTrack&& t_other) noexcept = delete;
        AutoTrack& operator=(const AutoTrack& t_other) = delete;
        AutoTrack& operator=(AutoTrack&& t_other) noexcept = delete;

        ~AutoTrack() noexcept = default;

    protected:

    private:

    };
}
