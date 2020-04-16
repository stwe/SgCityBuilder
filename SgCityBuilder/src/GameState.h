// This file is part of the SgCityBuilder package.
// 
// Filename: GameState.h
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#pragma once

//#define ENABLE_TRAFFIC_DEBUG

#include "SgOgl.h"
#include "map/tile/Tile.h"

namespace sg::city::city
{
    class City;
}

namespace sg::city::input
{
    class MousePicker;
}

class GameState : public sg::ogl::state::State
{
public:
    static constexpr auto MAP_SIZE{ 8 };

    using FirstPersonCameraSharedPtr = std::shared_ptr<sg::ogl::camera::FirstPersonCamera>;
    using SceneUniquePtr = std::unique_ptr<sg::ogl::scene::Scene>;

    using CityUniquePtr = std::unique_ptr<sg::city::city::City>;
    using MousePickerUniquePtr = std::unique_ptr<sg::city::input::MousePicker>;

    using TileIndexContainer = std::vector<int>;

    using ForwardRendererUniquePtr = std::unique_ptr<sg::ogl::ecs::system::ForwardRenderSystem>;

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
    glm::ivec3 m_mapPoint{ glm::ivec3(0) };

    FirstPersonCameraSharedPtr m_firstPersonCamera;
    SceneUniquePtr m_scene;

    CityUniquePtr m_city;
    MousePickerUniquePtr m_mousePicker;

    sg::city::map::tile::TileType m_currentEditTileType{ sg::city::map::tile::TileType::TRAFFIC };

    TileIndexContainer m_changedTiles;

    ForwardRendererUniquePtr m_forwardRenderer;

    bool m_renderAutoTracks{ false };
    bool m_renderNavigationNodes{ false };

    //-------------------------------------------------
    // Helper
    //-------------------------------------------------

    void Init();

    //-------------------------------------------------
    // Cars
    //-------------------------------------------------

    void UpdateCars(double t_dt) const;

    //-------------------------------------------------
    // ImGui
    //-------------------------------------------------

    void InitImGui() const;
    void RenderImGui();
    static void CleanUpImGui();
};
