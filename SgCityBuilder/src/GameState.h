#pragma once

#include "SgOgl.h"

namespace sg::city::map
{
    class Map;
}

namespace sg::city::renderer
{
    class MapRenderer;
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
    using MousePickerUniquePtr = std::unique_ptr<sg::city::input::MousePicker>;

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
    SceneUniquePtr m_scene;

    FirstPersonCameraSharedPtr m_firstPersonCamera;
    MapSharedPtr m_map;
    MapRendererUniquePtr m_mapRenderer;
    MousePickerUniquePtr m_mousePicker;

    //-------------------------------------------------
    // Helper
    //-------------------------------------------------

    void Init();
    void CreateMapEntity();
};
