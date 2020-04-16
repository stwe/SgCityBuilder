// This file is part of the SgCityBuilder package.
// 
// Filename: BuildingsShader.h
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#pragma once

namespace sg::city::shader
{
    class BuildingsShader : public ogl::resource::ShaderProgram
    {
    public:
        void UpdateUniforms(const ogl::scene::Scene& t_scene, const entt::entity t_entity, const ogl::resource::Mesh& t_currentMesh) override
        {
            auto& buildingsComponent{ t_scene.GetApplicationContext()->registry.get<ecs::BuildingsComponent>(t_entity) };

            SetUniform("projectionMatrix", t_scene.GetApplicationContext()->GetWindow().GetProjectionMatrix());
            SetUniform("viewMatrix", t_scene.GetCurrentCamera().GetViewMatrix());
            SetUniform("quadTextureAtlas", 0);
            ogl::resource::TextureManager::BindForReading(buildingsComponent.buildingGenerator->GetBuildingTextureAtlasId(), GL_TEXTURE0);
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
