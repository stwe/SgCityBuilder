// This file is part of the SgCityBuilder package.
// 
// Filename: MapShader.h
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#pragma once

#include <resource/TextureManager.h>
#include <ecs/component/Components.h>
#include <camera/Camera.h>
#include <Window.h>
#include "ecs/Components.h"
#include "map/Map.h"

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
            const auto worldMatrix{ static_cast<glm::mat4>(transformComponent) };
            const auto mvp{ projectionMatrix * t_scene.GetCurrentCamera().GetViewMatrix() * worldMatrix };

            SetUniform("mvpMatrix", mvp);
            SetUniform("worldMatrix", worldMatrix);
            SetUniform("cameraPosition", t_scene.GetCurrentCamera().GetPosition());
            SetUniform("directionalLight", t_scene.GetCurrentDirectionalLight());

            SetUniform("showRegionColor", mapComponent.map->showRegions);

            SetUniform("tileTexture[0]", 0);
            SetUniform("tileTexture[1]", 1);
            SetUniform("tileTexture[2]", 2);
            SetUniform("tileTexture[3]", 3);
            SetUniform("tileTexture[4]", 4);

            ogl::resource::TextureManager::BindForReading(mapComponent.map->GetTileTypeTextures().at(map::tile::TileType::NONE), GL_TEXTURE0);
            ogl::resource::TextureManager::BindForReading(mapComponent.map->GetTileTypeTextures().at(map::tile::TileType::RESIDENTIAL), GL_TEXTURE1);
            ogl::resource::TextureManager::BindForReading(mapComponent.map->GetTileTypeTextures().at(map::tile::TileType::COMMERCIAL), GL_TEXTURE2);
            ogl::resource::TextureManager::BindForReading(mapComponent.map->GetTileTypeTextures().at(map::tile::TileType::INDUSTRIAL), GL_TEXTURE3);
            ogl::resource::TextureManager::BindForReading(mapComponent.map->GetTileTypeTextures().at(map::tile::TileType::TRAFFIC), GL_TEXTURE4);
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
