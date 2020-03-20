// This file is part of the SgCityBuilder package.
// 
// Filename: GameState.h
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#pragma once

#include "SgOgl.h"
#include "map/Map.h"

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
    using SceneUniquePtr = std::unique_ptr<sg::ogl::scene::Scene>;
    using FirstPersonCameraSharedPtr = std::shared_ptr<sg::ogl::camera::FirstPersonCamera>;
    using ForwardRendererUniquePtr = std::unique_ptr<sg::ogl::ecs::system::ForwardRenderSystem>;
    using TextRendererUniquePtr = std::unique_ptr<sg::ogl::ecs::system::TextRenderSystem>;
    using MousePickerUniquePtr = std::unique_ptr<sg::city::input::MousePicker>;
    using CityUniquePtr = std::unique_ptr<sg::city::city::City>;

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

    FirstPersonCameraSharedPtr m_firstPersonCamera;
    SceneUniquePtr m_scene;
    CityUniquePtr m_city;
    ForwardRendererUniquePtr m_forwardRenderer;
    TextRendererUniquePtr m_textRenderer;
    MousePickerUniquePtr m_mousePicker;
    sg::city::map::Map::TileType m_currentTileType{ sg::city::map::Map::TileType::RESIDENTIAL };

    //-------------------------------------------------
    // Helper
    //-------------------------------------------------

    void Init();
    void CreateExampleRoads() const;

    //-------------------------------------------------
    // ImGui
    //-------------------------------------------------

    void InitImGui() const;
    void RenderImGui();
    static void CleanUpImGui();
};
