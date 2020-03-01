#pragma once

#include "ecs/Components.h"

namespace sg::city::shader
{
    class RoadNetworkShader : public ogl::resource::ShaderProgram
    {
    public:
        void UpdateUniforms(const ogl::scene::Scene& t_scene, const entt::entity t_entity, const ogl::resource::Mesh& t_currentMesh) override
        {
            auto& transformComponent{ t_scene.GetApplicationContext()->registry.get<ogl::ecs::component::TransformComponent>(t_entity) };
            auto& roadNetworkComponent{ t_scene.GetApplicationContext()->registry.get<ecs::RoadNetworkComponent>(t_entity) };

            const auto projectionMatrix{ t_scene.GetApplicationContext()->GetWindow().GetProjectionMatrix() };
            const auto mvp{ projectionMatrix * t_scene.GetCurrentCamera().GetViewMatrix() * static_cast<glm::mat4>(transformComponent) };

            SetUniform("mvpMatrix", mvp);

            SetUniform("roadTexture[0]", 0);
            SetUniform("roadTexture[1]", 1);
            SetUniform("roadTexture[2]", 2);

            ogl::resource::TextureManager::BindForReading(roadNetworkComponent.roadNetwork->GetWoTextureId(), GL_TEXTURE0);
            ogl::resource::TextureManager::BindForReading(roadNetworkComponent.roadNetwork->GetNsTextureId(), GL_TEXTURE1);
            ogl::resource::TextureManager::BindForReading(roadNetworkComponent.roadNetwork->GetAllTextureId(), GL_TEXTURE2);
        }

        [[nodiscard]] std::string GetFolderName() const override
        {
            return "road_network";
        }

        [[nodiscard]] bool IsBuiltIn() const override
        {
            return false;
        }

    protected:

    private:

    };
}