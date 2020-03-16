// This file is part of the SgCityBuilder package.
// 
// Filename: BuildingsRenderer.h
// Author:   stwe
// 
// License:  MIT
// 
// 2020 (c) stwe <https://github.com/stwe/SgCityBuilder>

#pragma once

#include "shader/BuildingsShader.h"

namespace sg::city::renderer
{
    class BuildingsRenderer : public ogl::ecs::system::RenderSystem<shader::BuildingsShader>
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        explicit BuildingsRenderer(ogl::scene::Scene* t_scene)
            : RenderSystem(t_scene)
        {
        }

        //-------------------------------------------------
        // Override
        //-------------------------------------------------

        void Update(double t_dt) override {}

        void Render() override
        {
            PrepareRendering();

            auto& shader{ m_scene->GetApplicationContext()->GetShaderManager().GetShaderProgram<shader::BuildingsShader>() };
            shader.Bind();

            auto view{ m_scene->GetApplicationContext()->registry.view<
                ecs::BuildingsComponent,
                ogl::ecs::component::TransformComponent>(entt::exclude<ecs::PathComponent>) // todo: remove
            };

            for (auto entity : view)
            {
                auto& buildingsComponent{ view.get<ecs::BuildingsComponent>(entity) };

                shader.UpdateUniforms(*m_scene, entity, buildingsComponent.buildingGenerator->GetMesh());

                buildingsComponent.buildingGenerator->GetMesh().InitDraw();
                buildingsComponent.buildingGenerator->GetMesh().DrawInstanced(buildingsComponent.buildingGenerator->GetInstances());
                buildingsComponent.buildingGenerator->GetMesh().EndDraw();
            }

            ogl::resource::ShaderProgram::Unbind();

            FinishRendering();
        }

    protected:
        void PrepareRendering() override
        {
            //ogl::OpenGl::EnableFaceCulling(); // todo
        }

        void FinishRendering() override
        {
            //ogl::OpenGl::DisableFaceCulling();
        }

    private:

    };
}
