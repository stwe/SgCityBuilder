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
        std::shared_ptr<Automata> GetShared()
        {
            return shared_from_this();
        }

        glm::vec3 pos{ glm::vec3(0.0f) };

        float autoPos{ 0.0f };
        float autoLength{ 0.0f };

        std::shared_ptr<AutoNode> originNode;
        std::shared_ptr<AutoTrack> currentTrack;

        void Update(float t_dt);

    protected:

    private:

    };
}
