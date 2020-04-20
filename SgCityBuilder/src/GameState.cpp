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
#include "automata/Automata.h"
#include "automata/AutoTrack.h"
//#include "city/Timer.h"

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
            const auto[changedTileIndex, skip]{ m_city->ReplaceTile(m_mapPoint.x, m_mapPoint.z, m_currentEditTileType) };

            if (!skip)
            {
                m_changedTileIndex = changedTileIndex;
            }
            else
            {
                m_changedTileIndex = -1;
            }

            // delete mouse state
            sg::ogl::input::MouseInput::ClearMouseStates();
        }
    }

    return true;
}

bool GameState::Update(const double t_dt)
{
    m_scene->GetCurrentCamera().Update(t_dt);

    {
        //sg::city::timer::Timer timer;
        m_city->Update(t_dt, m_changedTileIndex);
    }

    UpdateCars(t_dt);

    return true;
}

void GameState::Render()
{
    // render Map
    m_city->Render();

#ifdef ENABLE_TRAFFIC_DEBUG
    // only render the AutoTracks after update of changed Tile is complete
    if (m_changedTileIndex == -1 && m_renderAutoTracks)
    {
        m_city->RenderAutoTracks();
    }

    // render Navigation Nodes
    if (m_renderNavigationNodes)
    {
        m_city->GetMap().RenderNavigationNodes();
    }
#endif

    // render cars
    m_forwardRenderer->Render();

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

#ifdef LOAD_MAP_8_8
    m_city = std::make_unique<sg::city::city::City>(CITY_NAME, MAP_8_8_FILE_NAME, m_scene.get());
#else
    m_city = std::make_unique<sg::city::city::City>(CITY_NAME, MAP_FILE_NAME, m_scene.get());
#endif

    m_mousePicker = std::make_unique<sg::city::input::MousePicker>(m_scene.get(), m_city->GetMapSharedPtr());
    m_forwardRenderer = std::make_unique<sg::ogl::ecs::system::ForwardRenderSystem>(m_scene.get());
}

//-------------------------------------------------
// Cars
//-------------------------------------------------

void GameState::UpdateCars(const double t_dt) const
{
    auto del{ false };

    // update Automatas
    for (auto& automata : m_city->automatas)
    {
        if (automata)
        {
            automata->Update(static_cast<float>(t_dt));

            // the Update function may have set deleteAutomata to true
            if (automata->deleteAutomata)
            {
                // despawn

                // 1) remove from the track
                automata->currentTrack->automatas.remove(automata.get());

                // 2) eliminating one owner of the automata shared_ptr
                //    the automata is also owned by a car entity
                automata.reset();
                del = true;
            }
        }
    }

    if (del)
    {
        // remove dead automata
        m_city->automatas.remove(nullptr);
    }

    // get view
    auto view{ m_scene->GetApplicationContext()->registry.view<
        sg::ogl::ecs::component::ModelComponent,
        sg::ogl::ecs::component::TransformComponent,
        sg::city::ecs::AutomataComponent>()
    };

    // change the Transform Component of each Entity in the View
    for (auto entity : view)
    {
        auto& automataComponent{ view.get<sg::city::ecs::AutomataComponent>(entity) };
        auto& transformComponent{ view.get<sg::ogl::ecs::component::TransformComponent>(entity) };

        if (automataComponent.automata->deleteAutomata)
        {
            // eliminating the other owner of the automata shared_ptr -> instance is destroyed
            automataComponent.automata.reset();

            // destroy car entity
            m_scene->GetApplicationContext()->registry.destroy(entity);
        }
        else
        {
            transformComponent.position = glm::vec3(automataComponent.automata->position.x, 0.015f, automataComponent.automata->position.z);
            transformComponent.rotation = glm::vec3(0.0f, automataComponent.automata->currentTrack->rotation, 0.0f);
        }
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

    ImGui::Begin("Menu");

    for (auto type : sg::city::map::tile::Tile::TILE_TYPES)
    {
        const auto i{ static_cast<int>(type) };

        if (m_buttons[i])
        {
            ImGui::PushID(i);
            ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(7.0f, 0.6f, 0.6f)));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor::HSV(7.0f, 0.7f, 0.7f)));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor::HSV(7.0f, 0.8f, 0.8f)));
            ImGui::Button(sg::city::map::tile::Tile::TileTypeToString(type).c_str());
            if (ImGui::IsItemClicked(0))
            {
                m_buttons[i] = !m_buttons[i];
            }

            ImGui::PopStyleColor(3);
            ImGui::PopID();
        }
        else
        {
            if (ImGui::Button(sg::city::map::tile::Tile::TileTypeToString(type).c_str()))
            {
                std::fill(m_buttons.begin(), m_buttons.end(), false);
                m_buttons[i] = true;
                m_currentEditTileType = type;
            }
        }
    }

    ImGui::Text("Current number of regions: %i", m_city->GetMap().GetNumRegions());

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Save map"))
    {
        m_city->GetMap().SaveMap();
    }

    if (ImGui::Button("Load map"))
    {
        m_city->GetMap().LoadMap();
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

    ImGui::Text("City Automatas: %i", m_city->automatas.size());

    if (ImGui::Button("Spawn single car on current tile"))
    {
        m_city->TrySpawnCarAtSafeTrack(m_mapPoint.x, m_mapPoint.z);
    }

    if (ImGui::Button("Spawn multiple cars on random tiles"))
    {
        m_city->spawnCars = !m_city->spawnCars;
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

#ifdef ENABLE_TRAFFIC_DEBUG
    if (ImGui::Button("Render AutoTracks"))
    {
        m_renderAutoTracks = !m_renderAutoTracks;
    }

    if (ImGui::Button("Render Navigation Nodes"))
    {
        m_renderNavigationNodes = !m_renderNavigationNodes;
    }
#endif

    if (ImGui::Button("Show contiguous regions"))
    {
        m_city->GetMap().showRegions = !m_city->GetMap().showRegions;
    }

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
