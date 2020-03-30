// This file is part of the SgCityBuilder package.
// 
// Filename: Automata.h
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#pragma once

#include <memory>
#include <glm/vec3.hpp>

namespace sg::city::automata
{
    class AutoNode;
    class AutoTrack;

    class Automata : public std::enable_shared_from_this<Automata>
    {
    public:
        using AutoNodeSharedPtr = std::shared_ptr<AutoNode>;
        using AutoTrackSharedPtr = std::shared_ptr<AutoTrack>;

        std::shared_ptr<Automata> GetShared()
        {
            return shared_from_this();
        }

        glm::vec3 position{ glm::vec3(0.0f) };

        float autoPosition{ 0.0f };
        float autoLength{ 0.0f };

        AutoNodeSharedPtr rootNode;
        AutoTrackSharedPtr currentTrack;

        bool deleteAutomata{ false };

        void Update(float t_dt);

    protected:

    private:

    };
}
