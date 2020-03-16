// This file is part of the SgCityBuilder package.
// 
// Filename: MoveSystem.h
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

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
                PathComponent,
                MapComponent>()
            };

            for (auto entity : view)
            {
                auto& transformComponent{ view.get<ogl::ecs::component::TransformComponent>(entity) };
                auto& pathComponent{ view.get<PathComponent>(entity) };
                auto& mapComponent{ view.get<MapComponent>(entity) };

                // no waypoints
                if (pathComponent.waypoints.empty())
                {
                    return;
                }

                // get the next waypoint
                auto& waypoint{ pathComponent.waypoints.top() };

                // get target Tile of the waypoint
                auto& targetTile{ mapComponent.map->GetTileByPosition(glm::vec3(waypoint.x, 0.0f, waypoint.y)) };

                // get center of the Tile as target position
                auto targetTilePosition{ targetTile.GetCenter() };

                // add an offset if the object should not be in the middle of the Tile
                targetTilePosition.x += pathComponent.offsetX;
                targetTilePosition.z += pathComponent.offsetZ;

                // calculate the distance from the current position to the targe position
                const auto distance{ length(glm::vec2(targetTilePosition.x, targetTilePosition.z) - glm::vec2(transformComponent.position.x, transformComponent.position.z)) };

                // we are almost there...
                if (abs(distance) < SMALLDIST)
                {
                    // use the target position as the current position
                    transformComponent.position.x = targetTilePosition.x;
                    transformComponent.position.z = targetTilePosition.z;

                    // remove waypoint
                    if (!pathComponent.waypoints.empty())
                    {
                        pathComponent.waypoints.pop();
                    }
                }
                else
                {
                    // move towards the target
                    const auto targetX{ targetTilePosition.x - transformComponent.position.x };
                    const auto targetZ{ targetTilePosition.z - transformComponent.position.z };

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
