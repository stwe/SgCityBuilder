// This file is part of the SgCityBuilder package.
// 
// Filename: RoadNetworkShader.h
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#pragma once

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
            const auto worldMatrix{ static_cast<glm::mat4>(transformComponent) };
            const auto mvp{ projectionMatrix * t_scene.GetCurrentCamera().GetViewMatrix() * worldMatrix };

            SetUniform("mvpMatrix", mvp);
            SetUniform("worldMatrix", worldMatrix);
            SetUniform("cameraPosition", t_scene.GetCurrentCamera().GetPosition());
            SetUniform("directionalLight", t_scene.GetCurrentDirectionalLight());

            SetUniform("roadTextureAtlas", 0);
            ogl::resource::TextureManager::BindForReading(roadNetworkComponent.roadNetwork->GetRoadTextureAtlasId(), GL_TEXTURE0);
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
