#include "GameState.h"
#include "renderer/MapRenderer.h"
#include "renderer/RoadNetworkRenderer.h"
#include "map/RoadNetwork.h"
#include "map/Tile.h"
#include "map/Astar.h"
#include "ecs/Components.h"
#include "ecs/MoveSystem.h"
#include "input/MousePicker.h"

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
            //SG_OGL_LOG_DEBUG("Map Point x: {}, z: {}", m_mapPoint.x, m_mapPoint.z);

            if (m_currentTileType == sg::city::map::Map::TileType::TRAFFIC_NETWORK)
            {
                m_roadNetwork->StoreRoadOnPosition(m_mapPoint);
            }
            else
            {
                m_map->ChangeTileTypeOnPosition(m_mapPoint, m_currentTileType);
            }

            /*
            const auto& tile{ m_map->GetTileByPosition(m_mapPoint) };

            if (m_currentTileType == sg::city::map::Map::TileType::RESIDENTIAL)
            {
                GetApplicationContext()->GetEntityFactory().CreateModelEntity(
                    "res/model/H1/h1.obj",
                    glm::vec3(tile.GetMapX() + 0.5f, 2.4f, -tile.GetMapZ() + -0.45f),
                    glm::vec3(0.0f),
                    glm::vec3(1.0f / 152.0f),
                    false
                );
            }

            if (m_currentTileType == sg::city::map::Map::TileType::COMMERCIAL)
            {
                GetApplicationContext()->GetEntityFactory().CreateModelEntity(
                    "res/model/Tree_02/tree02.obj",
                    glm::vec3(tile.GetMapX() + 0.5f, 0.0f, -tile.GetMapZ() + -0.5f),
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(1.0f / 2.0f),
                    false
                );
            }
            */

            m_map->FindConnectedRegions();
        }
    }

    return true;
}

bool GameState::Update(const double t_dt)
{
    m_scene->GetCurrentCamera().Update(t_dt);
    m_roadNetwork->UpdateDirections(); // todo: do this only when necessary
    m_moveSystem->Update(t_dt);

    return true;
}

void GameState::Render()
{
    m_mapRenderer->Render();
    m_roadNetworkRenderer->Render();
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
        glm::vec3(4.0f, 3.0f, 4.0f),
        -90.0f, // todo
        -6.0f   // todo: -90 not allowed (clamp)
    );
    m_firstPersonCamera->SetCameraVelocity(4.0f);
    m_firstPersonCamera->SetMouseSensitivity(0.05f);

    m_scene = std::make_unique<sg::ogl::scene::Scene>(GetApplicationContext());
    m_scene->SetCurrentCamera(m_firstPersonCamera);

    m_map = std::make_shared<sg::city::map::Map>(m_scene.get());
    m_map->CreateMap(128);
    m_map->position = glm::vec3(0.0f);
    m_map->rotation = glm::vec3(0.0f);
    m_map->scale = glm::vec3(1.0f);

    m_roadNetwork = std::make_shared<sg::city::map::RoadNetwork>(m_map.get());

    m_astar = std::make_unique<sg::city::map::Astar>(m_map.get());

    CreateMapEntity();
    CreateRoadNetworkEntity();

    auto entity = GetApplicationContext()->GetEntityFactory().CreateModelEntity(
        "res/model/Plane1/plane1.obj",
        glm::vec3(0.65f, 0.1f, -0.35f),
        glm::vec3(0.0f),
        glm::vec3(1.0f / 16.0f),
        false
    );

    m_mousePicker = std::make_unique<sg::city::input::MousePicker>(m_scene.get(), m_map);

    m_mapRenderer = std::make_unique<sg::city::renderer::MapRenderer>(m_scene.get());
    m_roadNetworkRenderer = std::make_unique<sg::city::renderer::RoadNetworkRenderer>(m_scene.get());
    m_forwardRenderer = std::make_unique<sg::ogl::ecs::system::ForwardRenderSystem>(m_scene.get());

    CreateRoads();

    // Find path 0,0 --> 3,3
    auto path{ m_astar->FindPath(0, m_map->GetTileIndexByPosition(3, 3)) };
    GetApplicationContext()->registry.assign<sg::city::ecs::PathComponent>(entity, path, 0.65f, -0.35f);

    m_moveSystem = std::make_unique<sg::city::ecs::MoveSystem>(m_scene.get());
}

void GameState::CreateMapEntity()
{
    const auto entity{ GetApplicationContext()->registry.create() };

    GetApplicationContext()->registry.assign<sg::city::ecs::MapComponent>(
        entity,
        m_map
    );

    GetApplicationContext()->registry.assign<sg::ogl::ecs::component::TransformComponent>(
        entity,
        m_map->position,
        m_map->rotation,
        m_map->scale
    );
}

void GameState::CreateRoadNetworkEntity()
{
    const auto entity{ GetApplicationContext()->registry.create() };

    GetApplicationContext()->registry.assign<sg::city::ecs::RoadNetworkComponent>(
        entity,
        m_roadNetwork
    );

    GetApplicationContext()->registry.assign<sg::ogl::ecs::component::TransformComponent>(
        entity,
        m_map->position,
        m_map->rotation,
        m_map->scale
    );
}

void GameState::CreateRoads() const
{
    m_roadNetwork->StoreRoadOnPosition(0.0f, 0.0f);
    m_roadNetwork->StoreRoadOnPosition(0.0f, 1.0f);
    m_roadNetwork->StoreRoadOnPosition(0.0f, 2.0f);
    m_roadNetwork->StoreRoadOnPosition(0.0f, 3.0f);
    m_roadNetwork->StoreRoadOnPosition(1.0f, 3.0f);
    m_roadNetwork->StoreRoadOnPosition(2.0f, 3.0f);
    m_roadNetwork->StoreRoadOnPosition(3.0f, 3.0f);
    m_roadNetwork->StoreRoadOnPosition(3.0f, 2.0f);
    m_roadNetwork->StoreRoadOnPosition(3.0f, 1.0f);
    m_roadNetwork->StoreRoadOnPosition(3.0f, 0.0f);
    m_roadNetwork->StoreRoadOnPosition(1.0f, 0.0f);
    m_roadNetwork->StoreRoadOnPosition(2.0f, 0.0f);
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
        const auto& tile{ m_map->GetTileByPosition(m_mapPoint) };
        ImGui::Text("Current Tile x: %g", tile.GetMapX());
        ImGui::Text("Current Tile z: %g", tile.GetMapZ());
    }

    ImGui::Text("Current number of regions: %i", m_map->GetNumRegions());

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Show contiguous regions"))
    {
        m_map->showRegions = !m_map->showRegions;
    }

    if (ImGui::Button("Wireframe mode"))
    {
        m_map->wireframeMode = !m_map->wireframeMode;
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
