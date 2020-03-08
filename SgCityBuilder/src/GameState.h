#pragma once

#include "SgOgl.h"
#include "map/Map.h"

namespace sg::city::ecs
{
    class MoveSystem;
}

namespace sg::city::map
{
    class RoadNetwork;
    class Astar;
}

namespace sg::city::renderer
{
    class MapRenderer;
    class RoadNetworkRenderer;
}

namespace sg::city::input
{
    class MousePicker;
}

class GameState : public sg::ogl::state::State
{
public:
    using SceneUniquePtr = std::unique_ptr<sg::ogl::scene::Scene>;

    using FirstPersonCameraSharedPtr = std::shared_ptr<sg::ogl::camera::FirstPersonCamera>;
    using MapSharedPtr = std::shared_ptr<sg::city::map::Map>;

    using MapRendererUniquePtr = std::unique_ptr<sg::city::renderer::MapRenderer>;
    using RoadNetworkRendererUniquePtr = std::unique_ptr<sg::city::renderer::RoadNetworkRenderer>;
    using ForwardRendererUniquePtr = std::unique_ptr<sg::ogl::ecs::system::ForwardRenderSystem>;

    using MousePickerUniquePtr = std::unique_ptr<sg::city::input::MousePicker>;

    using RoadNetworkSharedPtr = std::shared_ptr<sg::city::map::RoadNetwork>;

    using AstarUniquePtr = std::unique_ptr<sg::city::map::Astar>;

    //-------------------------------------------------
    // Ctors. / Dtor.
    //-------------------------------------------------

    GameState() = delete;

    explicit GameState(sg::ogl::state::StateStack* t_stateStack);

    GameState(const GameState& t_other) = delete;
    GameState(GameState&& t_other) noexcept = delete;
    GameState& operator=(const GameState& t_other) = delete;
    GameState& operator=(GameState&& t_other) noexcept = delete;

    ~GameState() noexcept override;

    //-------------------------------------------------
    // Logic
    //-------------------------------------------------

    bool Input() override;
    bool Update(double t_dt) override;
    void Render() override;

protected:

private:
    glm::vec3 m_mapPoint{ glm::vec3(0.0f) };

    SceneUniquePtr m_scene;

    FirstPersonCameraSharedPtr m_firstPersonCamera;
    MapSharedPtr m_map;

    MapRendererUniquePtr m_mapRenderer;
    RoadNetworkRendererUniquePtr m_roadNetworkRenderer;
    ForwardRendererUniquePtr m_forwardRenderer;

    MousePickerUniquePtr m_mousePicker;

    RoadNetworkSharedPtr m_roadNetwork;

    sg::city::map::Map::TileType m_currentTileType{ sg::city::map::Map::TileType::TRAFFIC_NETWORK };

    AstarUniquePtr m_astar;

    std::unique_ptr<sg::city::ecs::MoveSystem> m_moveSystem;

    //-------------------------------------------------
    // Helper
    //-------------------------------------------------

    void Init();
    void CreateMapEntity();
    void CreateRoadNetworkEntity();
    void CreateRoads() const;

    //-------------------------------------------------
    // ImGui
    //-------------------------------------------------

    void InitImGui() const;
    void RenderImGui();
    static void CleanUpImGui();
};
