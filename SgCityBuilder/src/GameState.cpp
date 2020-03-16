// This file is part of the SgCityBuilder package.
// 
// Filename: GameState.cpp
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#include "GameState.h"
#include "map/RoadNetwork.h"
#include "map/BuildingGenerator.h"
#include "map/Tile.h"
#include "ecs/Components.h"
#include "ecs/MoveSystem.h"
#include "input/MousePicker.h"
#include "city/City.h"

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

    if (GetApplicationContext()->GetMouseInput().IsLeftButtonPressed())
    {
        m_mousePicker->Update(
            static_cast<float>(GetApplicationContext()->GetMouseInput().GetCurrentPos().x),
            static_cast<float>(GetApplicationContext()->GetMouseInput().GetCurrentPos().y)
        );

        m_mapPoint = m_mousePicker->GetCurrentMapPoint();

        if (m_mapPoint.x >= 0.0)
        {
            if (m_currentTileType == sg::city::map::Map::TileType::TRAFFIC_NETWORK)
            {
                m_city->GetRoadNetwork().StoreRoadOnPosition(m_mapPoint);
            }
            else if (m_currentTileType == sg::city::map::Map::TileType::RESIDENTIAL)
            {
                m_city->GetBuildingGenerator().StoreBuildingOnPosition(m_mapPoint);
            }
            else
            {
                m_city->GetMap().ChangeTileTypeOnPosition(m_mapPoint, m_currentTileType);
            }

            m_city->GetMap().FindConnectedRegions();
        }
    }

    return true;
}

bool GameState::Update(const double t_dt)
{
    m_scene->GetCurrentCamera().Update(t_dt);
    m_moveSystem->Update(t_dt);
    m_city->Update(t_dt);

    return true;
}

void GameState::Render()
{
    m_city->RenderMap();
    m_city->RenderRoadNetwork();
    m_city->RenderBuildings();

    m_forwardRenderer->Render();

    m_textRenderer->RenderText("SgCityBuilder", 10.0f, 10.0f, 0.25f, glm::vec3(0.1f));

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

    m_city = std::make_unique<sg::city::city::City>("SgCity", m_scene.get(), 128);

    // example "car" model
    const auto entity{ GetApplicationContext()->GetEntityFactory().CreateModelEntity(
        "res/model/Plane1/plane1.obj",
        glm::vec3(0.5f, 0.1f, -0.5f),
        glm::vec3(0.0f),
        glm::vec3(1.0f / 16.0f),
        false
    ) };

    m_mousePicker = std::make_unique<sg::city::input::MousePicker>(m_scene.get(), m_city->GetMapPtr());
    m_forwardRenderer = std::make_unique<sg::ogl::ecs::system::ForwardRenderSystem>(m_scene.get());
    m_textRenderer = std::make_unique<sg::ogl::ecs::system::TextRenderSystem>(m_scene.get(), "res/font/bitter/Bitter-Italic.otf");

    CreateExampleRoads();

    // Find an example path 0,0 --> 3,3
    auto path{ m_city->Path(0, 0, 3, 3) };
    GetApplicationContext()->registry.assign<sg::city::ecs::PathComponent>(entity, path, 0.15f, 0.15f);
    GetApplicationContext()->registry.assign<sg::city::ecs::MapComponent>(entity, m_city->GetMapPtr());

    m_moveSystem = std::make_unique<sg::city::ecs::MoveSystem>(m_scene.get());
}

void GameState::CreateExampleRoads() const
{
    auto& roadNetwork{ m_city->GetRoadNetwork() };
    roadNetwork.StoreRoadOnPosition(0, 0);
    roadNetwork.StoreRoadOnPosition(0, 1);
    roadNetwork.StoreRoadOnPosition(0, 2);
    roadNetwork.StoreRoadOnPosition(0, 3);
    roadNetwork.StoreRoadOnPosition(1, 3);
    roadNetwork.StoreRoadOnPosition(2, 3);
    roadNetwork.StoreRoadOnPosition(3, 3);
    roadNetwork.StoreRoadOnPosition(3, 2);
    roadNetwork.StoreRoadOnPosition(3, 1);
    roadNetwork.StoreRoadOnPosition(3, 0);
    roadNetwork.StoreRoadOnPosition(1, 0);
    roadNetwork.StoreRoadOnPosition(2, 0);
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

    if (m_currentTileType == sg::city::map::Map::TileType::NONE)
    {
        ImGui::Text(sg::city::map::Tile::TileTypeToString(sg::city::map::Map::TileType::NONE).c_str());
    }

    if (m_currentTileType == sg::city::map::Map::TileType::RESIDENTIAL)
    {
        ImGui::Text(sg::city::map::Tile::TileTypeToString(sg::city::map::Map::TileType::RESIDENTIAL).c_str());
    }

    if (m_currentTileType == sg::city::map::Map::TileType::COMMERCIAL)
    {
        ImGui::Text(sg::city::map::Tile::TileTypeToString(sg::city::map::Map::TileType::COMMERCIAL).c_str());
    }

    if (m_currentTileType == sg::city::map::Map::TileType::INDUSTRIAL)
    {
        ImGui::Text(sg::city::map::Tile::TileTypeToString(sg::city::map::Map::TileType::INDUSTRIAL).c_str());
    }

    if (m_currentTileType == sg::city::map::Map::TileType::TRAFFIC_NETWORK)
    {
        ImGui::Text(sg::city::map::Tile::TileTypeToString(sg::city::map::Map::TileType::TRAFFIC_NETWORK).c_str());
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button(sg::city::map::Tile::TileTypeToString(sg::city::map::Map::TileType::NONE).c_str()))
    {
        m_currentTileType = sg::city::map::Map::TileType::NONE;
    }

    if (ImGui::Button(sg::city::map::Tile::TileTypeToString(sg::city::map::Map::TileType::RESIDENTIAL).c_str()))
    {
        m_currentTileType = sg::city::map::Map::TileType::RESIDENTIAL;
    }

    if (ImGui::Button(sg::city::map::Tile::TileTypeToString(sg::city::map::Map::TileType::COMMERCIAL).c_str()))
    {
        m_currentTileType = sg::city::map::Map::TileType::COMMERCIAL;
    }

    if (ImGui::Button(sg::city::map::Tile::TileTypeToString(sg::city::map::Map::TileType::INDUSTRIAL).c_str()))
    {
        m_currentTileType = sg::city::map::Map::TileType::INDUSTRIAL;
    }

    if (ImGui::Button(sg::city::map::Tile::TileTypeToString(sg::city::map::Map::TileType::TRAFFIC_NETWORK).c_str()))
    {
        m_currentTileType = sg::city::map::Map::TileType::TRAFFIC_NETWORK;
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (m_mapPoint.x >= 0.0)
    {
        const auto& tile{ m_city->GetMap().GetTileByPosition(m_mapPoint) };
        ImGui::Text("Current Tile x: %g", tile.GetMapX());
        ImGui::Text("Current Tile z: %g", tile.GetMapZ());
    }

    ImGui::Text("Current number of regions: %i", m_city->GetMap().GetNumRegions());

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Show contiguous regions"))
    {
        m_city->GetMap().showRegions = !m_city->GetMap().showRegions;
    }

    if (ImGui::Button("Wireframe mode"))
    {
        m_city->GetMap().wireframeMode = !m_city->GetMap().wireframeMode;
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Day: %i", m_city->GetDay());
    ImGui::SliderFloat("Time per day: ", &m_city->GetTimePerDay(), 0.0f, 1.0f, "ratio = %.2f");
    ImGui::Text("Homeless people: %f", m_city->GetPopulationPool());
    ImGui::Text("Population: %f", m_city->GetPopulation());

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
