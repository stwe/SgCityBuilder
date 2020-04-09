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

    class Automata
    {
    public:
        using AutoNodeSharedPtr = std::shared_ptr<AutoNode>;
        using AutoTrackSharedPtr = std::shared_ptr<AutoTrack>;

        //-------------------------------------------------
        // Public member
        //-------------------------------------------------

        glm::vec3 position{ glm::vec3(0.0f) };

        float autoPosition{ 0.0f };
        float autoLength{ 0.0f };

        AutoNodeSharedPtr rootNode;
        AutoTrackSharedPtr currentTrack;

        bool deleteAutomata{ false };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Automata() = default;

        Automata(const Automata& t_other) = delete;
        Automata(Automata&& t_other) noexcept = delete;
        Automata& operator=(const Automata& t_other) = delete;
        Automata& operator=(Automata&& t_other) noexcept = delete;

        ~Automata() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        void Update(float t_dt);

    protected:

    private:

    };
}
