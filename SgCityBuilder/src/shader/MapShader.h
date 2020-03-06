#pragma once

#include "ecs/Components.h"

namespace sg::city::shader
{
    class MapShader : public ogl::resource::ShaderProgram
    {
    public:
        void UpdateUniforms(const ogl::scene::Scene& t_scene, const entt::entity t_entity, const ogl::resource::Mesh& t_currentMesh) override
        {
            auto& transformComponent{ t_scene.GetApplicationContext()->registry.get<ogl::ecs::component::TransformComponent>(t_entity) };
            auto& mapComponent{ t_scene.GetApplicationContext()->registry.get<ecs::MapComponent>(t_entity) };

            const auto projectionMatrix{ t_scene.GetApplicationContext()->GetWindow().GetProjectionMatrix() };
            const auto mvp{ projectionMatrix * t_scene.GetCurrentCamera().GetViewMatrix() * static_cast<glm::mat4>(transformComponent) };

            SetUniform("mvpMatrix", mvp);

            SetUniform("showRegionColor", mapComponent.map->showRegions);

            SetUniform("tileTexture[0]", 0);
            SetUniform("tileTexture[1]", 1);
            SetUniform("tileTexture[2]", 2);
            SetUniform("tileTexture[3]", 3);
            SetUniform("tileTexture[4]", 4);

            ogl::resource::TextureManager::BindForReading(mapComponent.map->GetTileTypeTextures()[0], GL_TEXTURE0);
            ogl::resource::TextureManager::BindForReading(mapComponent.map->GetTileTypeTextures()[1], GL_TEXTURE1);
            ogl::resource::TextureManager::BindForReading(mapComponent.map->GetTileTypeTextures()[2], GL_TEXTURE2);
            ogl::resource::TextureManager::BindForReading(mapComponent.map->GetTileTypeTextures()[3], GL_TEXTURE3);
            ogl::resource::TextureManager::BindForReading(mapComponent.map->GetTileTypeTextures()[4], GL_TEXTURE4);
        }

        [[nodiscard]] std::string GetFolderName() const override
        {
            return "map";
        }

        [[nodiscard]] bool IsBuiltIn() const override
        {
            return false;
        }

    protected:

    private:

    };
}
