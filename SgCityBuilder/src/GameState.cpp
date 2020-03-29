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
#include "input/MousePicker.h"
#include "city/City.h"
#include "automata/Automata.h"
#include "ecs/Components.h"

// todo: remove roads
// todo: remove buildings
// todo: buildings of different heights
// todo: load map
// todo: select/unselect tiles

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
            if (m_currentTileType == sg::city::map::Map::TileType::TRAFFIC_NETWORK)
            {
                m_city->GetRoadNetwork().StoreRoadOnMapPosition(m_mapPoint.x, m_mapPoint.z);

                if (!m_spawn)
                {
                    m_city->SpawnCarAtSafeTrack(m_mapPoint.x, m_mapPoint.z);

                    for (auto& automata : m_city->automatas)
                    {
                        auto e{ GetApplicationContext()->GetEntityFactory().CreateModelEntity(
                            "res/model/Plane1/plane1.obj",
                            glm::vec3(automata->position.x, 0.015f, automata->position.z),
                            glm::vec3(0.0f),
                            glm::vec3(0.125f / 4.0f),
                            false
                        ) };

                        GetApplicationContext()->registry.assign<sg::city::ecs::AutomataComponent>(
                            e,
                            automata
                        );
                    }

                    m_spawn = true;
                }
            }
            else
            {
                m_city->GetMap().ChangeTileTypeOnMapPosition(m_mapPoint.x, m_mapPoint.z, m_currentTileType);
            }

            //m_city->GetMap().FindConnectedRegions();

            // delete mouse state
            sg::ogl::input::MouseInput::ClearMouseStates();
        }
    }

    return true;
}

bool GameState::Update(const double t_dt)
{
    m_scene->GetCurrentCamera().Update(t_dt);
    m_city->Update(t_dt);

    /////////////////////

    for (auto& automata : m_city->automatas)
    {
        automata->Update(t_dt);
    }

    auto view{ m_scene->GetApplicationContext()->registry.view<
            sg::ogl::ecs::component::ModelComponent,
            sg::ogl::ecs::component::TransformComponent,
            sg::city::ecs::AutomataComponent>()
    };

    for (auto entity : view)
    {
        auto& automataComponent{ view.get<sg::city::ecs::AutomataComponent>(entity) };
        auto& transformComponent{ view.get<sg::ogl::ecs::component::TransformComponent>(entity) };

        transformComponent.position = glm::vec3(automataComponent.automata->position.x, 0.015f, automataComponent.automata->position.z);
    }

    /////////////////////

    return true;
}

void GameState::Render()
{
    m_city->RenderMap();
    m_city->RenderRoadNetwork();

    RenderDebug();

    //m_city->RenderBuildings();
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

    m_city = std::make_unique<sg::city::city::City>("SgCity", m_scene.get(), MAP_SIZE);

    m_mousePicker = std::make_unique<sg::city::input::MousePicker>(m_scene.get(), m_city->GetMapPtr());
    m_textRenderer = std::make_unique<sg::ogl::ecs::system::TextRenderSystem>(m_scene.get(), "res/font/bitter/Bitter-Italic.otf");
    m_forwardRenderer = std::make_unique<sg::ogl::ecs::system::ForwardRenderSystem>(m_scene.get());

    //CreateExampleRoads();
}

void GameState::CreateExampleRoads() const
{
    auto& roadNetwork{ m_city->GetRoadNetwork() };
    roadNetwork.StoreRoadOnMapPosition(1, 1);
    roadNetwork.StoreRoadOnMapPosition(1, 2);
    roadNetwork.StoreRoadOnMapPosition(1, 3);
    roadNetwork.StoreRoadOnMapPosition(1, 4);
    roadNetwork.StoreRoadOnMapPosition(2, 4);
    roadNetwork.StoreRoadOnMapPosition(3, 4);
    roadNetwork.StoreRoadOnMapPosition(4, 4);
    roadNetwork.StoreRoadOnMapPosition(4, 3);
    roadNetwork.StoreRoadOnMapPosition(4, 2);
    roadNetwork.StoreRoadOnMapPosition(4, 1);
    roadNetwork.StoreRoadOnMapPosition(2, 1);
    roadNetwork.StoreRoadOnMapPosition(3, 1);
}

void GameState::RenderDebug() const
{
    //m_city->GetMap().RenderTileNavigationNodes(1, 1);

    for (auto& tile : m_city->GetMap().GetTiles())
    {
        tile->RenderNavigationNodes(m_scene.get(), &m_city->GetMap());
        tile->RenderAutoTracks(m_scene.get(), &m_city->GetMap());
    }
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

    if (m_mapPoint.x >= 0)
    {
        const auto& tile{ m_city->GetMap().GetTileByMapPosition(m_mapPoint.x, m_mapPoint.z) };
        ImGui::Text("Current Tile x: %i", tile.GetMapX());
        ImGui::Text("Current Tile z: %i", tile.GetMapZ());
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
