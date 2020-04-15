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
#include <glm/vec3.hpp>
#include <list>

namespace sg::city::map::tile
{
    class Tile;
}

namespace sg::city::automata
{
    class AutoNode;
    class Automata;

    class AutoTrack
    {
    public:
        using AutoNodeSharedPtr = std::shared_ptr<AutoNode>;

        using AutomataContainer = std::list<Automata*>;

        //-------------------------------------------------
        // Public member
        //-------------------------------------------------

        AutoNodeSharedPtr startNode;
        AutoNodeSharedPtr endNode;

        map::tile::Tile* tile{ nullptr };

        float trackLength{ 1.0f };

        AutomataContainer automatas;

        bool isSafe{ false };

        /**
         * @brief The rotation of the car model.
         */
        float rotation{ 0.0f };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        AutoTrack() = default;

        AutoTrack(const AutoTrack& t_other) = delete;
        AutoTrack(AutoTrack&& t_other) noexcept = delete;
        AutoTrack& operator=(const AutoTrack& t_other) = delete;
        AutoTrack& operator=(AutoTrack&& t_other) noexcept = delete;

        ~AutoTrack() noexcept;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        [[nodiscard]] glm::vec3 GetPosition(float t_dt, const AutoNodeSharedPtr& t_node) const;

    protected:

    private:

    };
}
