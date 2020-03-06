#pragma once

#include "map/Map.h"
#include "shader/MapShader.h"
#include "ecs/Components.h"

namespace sg::city::renderer
{
    class MapRenderer : public ogl::ecs::system::RenderSystem<shader::MapShader>
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        explicit MapRenderer(ogl::scene::Scene* t_scene)
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

            auto& shader{ m_scene->GetApplicationContext()->GetShaderManager().GetShaderProgram<shader::MapShader>() };
            shader.Bind();

            auto view{ m_scene->GetApplicationContext()->registry.view<
                ecs::MapComponent,
                ogl::ecs::component::TransformComponent>()
            };

            for (auto entity : view)
            {
                auto& mapComponent{ view.get<ecs::MapComponent>(entity) };

                if (mapComponent.map->wireframeMode)
                {
                    ogl::OpenGl::EnableWireframeMode();
                }

                shader.UpdateUniforms(*m_scene, entity, mapComponent.map->GetMapMesh());

                mapComponent.map->GetMapMesh().InitDraw();
                mapComponent.map->GetMapMesh().DrawPrimitives();
                mapComponent.map->GetMapMesh().EndDraw();

                if (mapComponent.map->wireframeMode)
                {
                    ogl::OpenGl::DisableWireframeMode();
                }
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
