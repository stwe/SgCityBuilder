// This file is part of the SgCityBuilder package.
// 
// Filename: GameState.cpp
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#include "GameState.h"
#include "input/MousePicker.h"
#include "city/City.h"
#include "ecs/Components.h"
#include "map/Map.h"
#include "map/tile/RoadTile.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

GameState::GameState(sg::ogl::state::StateStack* t_stateStack)
    : State{ t_stateStack, "GameState" }
{
    Init();
}

GameState::~GameState() noexcept
{
    SG_OGL_LOG_DEBUG("[GameState::~GameState()] Destruct GameState.");
    CleanUpImGui();
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

bool GameState::Input()
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return true;
    }

    m_scene->GetCurrentCamera().Input();

    if (sg::ogl::input::MouseInput::IsLeftButtonPressed())
    {
        m_mousePicker->Update(
            static_cast<float>(GetApplicationContext()->GetMouseInput().GetCurrentPos().x),
            static_cast<float>(GetApplicationContext()->GetMouseInput().GetCurrentPos().y)
        );

        m_mapPoint = m_mousePicker->GetCurrentMapPoint();

        if (m_mapPoint.x >= 0)
        {
            SG_OGL_LOG_INFO("[GameState::Input()] Replace Tile on x: {}, z: {}.", m_mapPoint.x, m_mapPoint.z);
            const auto changedTileIndex{ m_city->ReplaceTile(m_mapPoint.x, m_mapPoint.z, m_currentEditTileType) };
            m_changedTiles.push_back(changedTileIndex);

            // delete mouse state
            sg::ogl::input::MouseInput::ClearMouseStates();
        }
    }

    return true;
}

bool GameState::Update(const double t_dt)
{
    m_scene->GetCurrentCamera().Update(t_dt);
    m_city->Update(t_dt, m_changedTiles);

    return true;
}

void GameState::Render()
{
    m_city->Render();

    // only render after update of changed Tiles is complete
    if (m_changedTiles.empty())
    {
        for (auto& tile : m_city->GetMap().GetTiles())
        {
            if (tile->type == sg::city::map::tile::TileType::TRAFFIC)
            {
                auto* roadTile{ dynamic_cast<sg::city::map::tile::RoadTile*>(tile.get()) };

                SG_OGL_ASSERT(roadTile, "[GameState::Render()] Null pointer.");

                roadTile->RenderNavigationNodes();
                roadTile->RenderAutoTracks();
            }
        }
    }

    RenderImGui();
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

void GameState::Init()
{
    InitImGui();

    sg::ogl::OpenGl::SetClearColor(sg::ogl::Color::CornflowerBlue());

    m_firstPersonCamera = std::make_shared<sg::ogl::camera::FirstPersonCamera>(
        GetApplicationContext(),
        glm::vec3(3.0f, 3.0f, 1.0f),
        -87.0f,
        -32.0f
    );
    m_firstPersonCamera->SetCameraVelocity(4.0f);
    m_firstPersonCamera->SetMouseSensitivity(0.05f);

    m_scene = std::make_unique<sg::ogl::scene::Scene>(GetApplicationContext());
    m_scene->SetCurrentCamera(m_firstPersonCamera);

    m_city = std::make_unique<sg::city::city::City>("SgCity", m_scene.get(), MAP_SIZE);

    m_mousePicker = std::make_unique<sg::city::input::MousePicker>(m_scene.get(), m_city->GetMapSharedPtr());
}

//-------------------------------------------------
// ImGui
//-------------------------------------------------

void GameState::InitImGui() const
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto& io{ ImGui::GetIO() };
    io.IniFilename = "res/config/Imgui.ini";

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(GetApplicationContext()->GetWindow().GetWindowHandle(), true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void GameState::RenderImGui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Map Edit");

    if (m_currentEditTileType == sg::city::map::tile::TileType::NONE)
    {
        ImGui::Text(sg::city::map::tile::Tile::TileTypeToString(sg::city::map::tile::TileType::NONE).c_str());
    }

    if (m_currentEditTileType == sg::city::map::tile::TileType::RESIDENTIAL)
    {
        ImGui::Text(sg::city::map::tile::Tile::TileTypeToString(sg::city::map::tile::TileType::RESIDENTIAL).c_str());
    }

    if (m_currentEditTileType == sg::city::map::tile::TileType::COMMERCIAL)
    {
        ImGui::Text(sg::city::map::tile::Tile::TileTypeToString(sg::city::map::tile::TileType::COMMERCIAL).c_str());
    }

    if (m_currentEditTileType == sg::city::map::tile::TileType::INDUSTRIAL)
    {
        ImGui::Text(sg::city::map::tile::Tile::TileTypeToString(sg::city::map::tile::TileType::INDUSTRIAL).c_str());
    }

    if (m_currentEditTileType == sg::city::map::tile::TileType::TRAFFIC)
    {
        ImGui::Text(sg::city::map::tile::Tile::TileTypeToString(sg::city::map::tile::TileType::TRAFFIC).c_str());
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button(sg::city::map::tile::Tile::TileTypeToString(sg::city::map::tile::TileType::NONE).c_str()))
    {
        m_currentEditTileType = sg::city::map::tile::TileType::NONE;
    }

    if (ImGui::Button(sg::city::map::tile::Tile::TileTypeToString(sg::city::map::tile::TileType::RESIDENTIAL).c_str()))
    {
        m_currentEditTileType = sg::city::map::tile::TileType::RESIDENTIAL;
    }

    if (ImGui::Button(sg::city::map::tile::Tile::TileTypeToString(sg::city::map::tile::TileType::COMMERCIAL).c_str()))
    {
        m_currentEditTileType = sg::city::map::tile::TileType::COMMERCIAL;
    }

    if (ImGui::Button(sg::city::map::tile::Tile::TileTypeToString(sg::city::map::tile::TileType::INDUSTRIAL).c_str()))
    {
        m_currentEditTileType = sg::city::map::tile::TileType::INDUSTRIAL;
    }

    if (ImGui::Button(sg::city::map::tile::Tile::TileTypeToString(sg::city::map::tile::TileType::TRAFFIC).c_str()))
    {
        m_currentEditTileType = sg::city::map::tile::TileType::TRAFFIC;
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (m_mapPoint.x >= 0)
    {
        const auto& tile{ m_city->GetMap().GetTileByMapPosition(m_mapPoint.x, m_mapPoint.z) };
        ImGui::Text("Current Tile x: %i", tile.GetMapX());
        ImGui::Text("Current Tile z: %i", tile.GetMapZ());
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Wireframe mode"))
    {
        m_city->GetMap().wireframeMode = !m_city->GetMap().wireframeMode;
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GameState::CleanUpImGui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
