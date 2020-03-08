#pragma once

namespace sg::ogl::scene
{
    class Scene;
}

namespace sg::city::ecs
{
    class MoveSystem
    {
    public:
        static constexpr auto SMALLDIST{ 0.04f };

        explicit MoveSystem(ogl::scene::Scene* t_scene)
            : m_scene{ t_scene }
        {}

        void Update(const double t_dt) const
        {
            auto view{ m_scene->GetApplicationContext()->registry.view<
                ogl::ecs::component::ModelComponent,
                ogl::ecs::component::TransformComponent,
                PathComponent>()
            };

            for (auto entity : view)
            {
                auto& transformComponent{ view.get<ogl::ecs::component::TransformComponent>(entity) };
                auto& pathComponent{ view.get<PathComponent>(entity) };

                // no waypoints
                if (pathComponent.waypoints.empty())
                {
                    return;
                }

                // get the next waypoint
                auto& waypoint{ pathComponent.waypoints.top() };

                // calculate the distance from the current position to the waypoint
                const auto distance{ length(waypoint - glm::vec2(transformComponent.position.x, transformComponent.position.z)) };

                // we are almost there...
                if (abs(distance) < SMALLDIST)
                {
                    // use the waypoint as the current position
                    transformComponent.position.x = waypoint.x;
                    transformComponent.position.z = waypoint.y;

                    // remove waypoint
                    if (!pathComponent.waypoints.empty())
                    {
                        pathComponent.waypoints.pop();
                    }
                }
                else
                {
                    // move towards the waypoint
                    const auto targetX{ waypoint.x - transformComponent.position.x };
                    const auto targetZ{ waypoint.y - transformComponent.position.z };

                    const auto direction{ normalize(glm::vec2(targetX, targetZ)) };

                    transformComponent.position.x += direction.x * static_cast<float>(t_dt);
                    transformComponent.position.z += direction.y * static_cast<float>(t_dt);
                }
            }
        }

    protected:

    private:
        ogl::scene::Scene* m_scene{ nullptr };
    };
}
