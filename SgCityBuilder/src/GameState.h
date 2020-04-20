// This file is part of the SgCityBuilder package.
// 
// Filename: GameState.h
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#pragma once

//-------------------------------------------------
// Build options
//-------------------------------------------------

#if defined (SG_CITY_DEBUG_BUILD)
    #define ENABLE_TRAFFIC_DEBUG
    //#define LOAD_MAP_8_8
#endif

//-------------------------------------------------

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
    using FirstPersonCameraSharedPtr = std::shared_ptr<sg::ogl::camera::FirstPersonCamera>;
    using SceneUniquePtr = std::unique_ptr<sg::ogl::scene::Scene>;

    using CityUniquePtr = std::unique_ptr<sg::city::city::City>;
    using MousePickerUniquePtr = std::unique_ptr<sg::city::input::MousePicker>;

    using ForwardRendererUniquePtr = std::unique_ptr<sg::ogl::ecs::system::ForwardRenderSystem>;

    using SkyboxRenderSystemUniquePtr = std::unique_ptr<sg::ogl::ecs::system::SkyboxRenderSystem>;
    using InstancingRenderSystemUniquePtr = std::unique_ptr<sg::ogl::ecs::system::InstancingRenderSystem>;

    using DirectionalLightSharedPtr = std::shared_ptr<sg::ogl::light::Sun>;

    //-------------------------------------------------
    // Const
    //-------------------------------------------------

    static constexpr auto CITY_NAME{ "SgCity" };
    static constexpr auto MAP_8_8_FILE_NAME{ "res/config/Map8x8.png" };
    static constexpr auto MAP_FILE_NAME{ "res/config/CityMap1.png" };

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

    int m_changedTileIndex{ -1 };

    ForwardRendererUniquePtr m_forwardRenderer;
    SkyboxRenderSystemUniquePtr m_skyboxRenderSystem;
    InstancingRenderSystemUniquePtr m_instancingRenderSystem;

    DirectionalLightSharedPtr m_sun;

    sg::city::map::tile::TileType m_currentEditTileType{ sg::city::map::tile::TileType::RESIDENTIAL };
    std::vector<bool> m_buttons{ false, true, false, false, false };

#ifdef ENABLE_TRAFFIC_DEBUG
    bool m_renderAutoTracks{ false };
    bool m_renderNavigationNodes{ false };
#endif

    //-------------------------------------------------
    // Helper
    //-------------------------------------------------

    void Init();
    void CreateDirectionalLight();
    void CreateSkybox() const;
    static std::vector<glm::mat4> CreateTreePositions(uint32_t t_instances);

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
