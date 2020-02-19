#include "GameState.h"
#include "renderer/MapRenderer.h"
#include "map/Map.h"
#include "map/Tile.h"
#include "ecs/Components.h"
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
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

bool GameState::Input()
{
    m_scene->GetCurrentCamera().Input();

    if (GetApplicationContext()->GetMouseInput().IsLeftButtonPressed())
    {
        m_mousePicker->Update(
            static_cast<float>(GetApplicationContext()->GetMouseInput().GetCurrentPos().x),
            static_cast<float>(GetApplicationContext()->GetMouseInput().GetCurrentPos().y)
        );

        const auto mapPoint{ m_mousePicker->GetCurrentMapPoint() };

        if (mapPoint.x >= 0.0) // todo - if (hasResult) {}
        {
            const auto tileIndex{ static_cast<int>(mapPoint.z) * m_map->GetMapSize() + static_cast<int>(mapPoint.x) };

            auto& tile{ m_map->GetTiles()[tileIndex] };
            SG_OGL_LOG_DEBUG("Map: x {}, z {}", tile->GetMapX(), tile->GetMapZ());
        }
    }

    return true;
}

bool GameState::Update(const double t_dt)
{
    m_scene->GetCurrentCamera().Update(t_dt);
    m_mapRenderer->Update(t_dt);

    return true;
}

void GameState::Render()
{
    m_mapRenderer->Render();
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

void GameState::Init()
{
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

    m_map = std::make_shared<sg::city::map::Map>();
    m_map->CreateMap(8);
    m_map->position = glm::vec3(0.0f);
    m_map->rotation = glm::vec3(0.0f);
    m_map->scale = glm::vec3(1.0f);

    CreateMapEntity();

    m_mousePicker = std::make_unique<sg::city::input::MousePicker>(m_scene.get(), m_map);
    m_mapRenderer = std::make_unique<sg::city::renderer::MapRenderer>(m_scene.get());
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
