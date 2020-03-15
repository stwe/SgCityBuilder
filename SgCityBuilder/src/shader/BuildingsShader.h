#pragma once

namespace sg::city::shader
{
    class BuildingsShader : public ogl::resource::ShaderProgram
    {
    public:
        void UpdateUniforms(const ogl::scene::Scene& t_scene, const entt::entity t_entity, const ogl::resource::Mesh& t_currentMesh) override
        {
            auto& transformComponent{ t_scene.GetApplicationContext()->registry.get<ogl::ecs::component::TransformComponent>(t_entity) };
            auto& buildingsComponent{ t_scene.GetApplicationContext()->registry.get<ecs::BuildingsComponent>(t_entity) };

            const auto projectionMatrix{ t_scene.GetApplicationContext()->GetWindow().GetProjectionMatrix() };
            const auto mvp{ projectionMatrix * t_scene.GetCurrentCamera().GetViewMatrix() * static_cast<glm::mat4>(transformComponent) };

            SetUniform("mvpMatrix", mvp);
            SetUniform("quadTextureAtlas", 0);
            ogl::resource::TextureManager::BindForReading(buildingsComponent.buildingGenerator->GetQuadTextureAtlasId(), GL_TEXTURE0);
        }

        [[nodiscard]] std::string GetFolderName() const override
        {
            return "buildings";
        }

        [[nodiscard]] bool IsBuiltIn() const override
        {
            return false;
        }

    protected:

    private:

    };
}
