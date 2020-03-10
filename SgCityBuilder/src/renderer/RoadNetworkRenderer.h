#pragma once

#include "shader/RoadNetworkShader.h"

namespace sg::city::renderer
{
    class RoadNetworkRenderer : public ogl::ecs::system::RenderSystem<shader::RoadNetworkShader>
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        explicit RoadNetworkRenderer(ogl::scene::Scene* t_scene)
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

            auto& shader{ m_scene->GetApplicationContext()->GetShaderManager().GetShaderProgram<shader::RoadNetworkShader>() };
            shader.Bind();

            auto view{ m_scene->GetApplicationContext()->registry.view<
                ecs::RoadNetworkComponent,
                ogl::ecs::component::TransformComponent>()
            };

            for (auto entity : view)
            {
                auto& roadNetworkComponent{ view.get<ecs::RoadNetworkComponent>(entity) };

                shader.UpdateUniforms(*m_scene, entity, roadNetworkComponent.roadNetwork->GetMesh());

                roadNetworkComponent.roadNetwork->GetMesh().InitDraw();
                roadNetworkComponent.roadNetwork->GetMesh().DrawPrimitives();
                roadNetworkComponent.roadNetwork->GetMesh().EndDraw();
            }

            ogl::resource::ShaderProgram::Unbind();

            FinishRendering();
        }

    protected:
        void PrepareRendering() override
        {
            ogl::OpenGl::EnableFaceCulling();
        }

        void FinishRendering() override
        {
            ogl::OpenGl::DisableFaceCulling();
        }

    private:

    };
}
