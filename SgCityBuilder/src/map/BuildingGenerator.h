#pragma once

#include <memory>

namespace sg::ogl::scene
{
    class Scene;
}

namespace sg::ogl::resource
{
    class Mesh;
}

namespace sg::city::map
{
    class BuildingGenerator
    {
    public:
        using MeshSharedPtr = std::shared_ptr<ogl::resource::Mesh>;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        BuildingGenerator() = delete;

        explicit BuildingGenerator(ogl::scene::Scene* t_scene);

        BuildingGenerator(const BuildingGenerator& t_other) = delete;
        BuildingGenerator(BuildingGenerator&& t_other) noexcept = delete;
        BuildingGenerator& operator=(const BuildingGenerator& t_other) = delete;
        BuildingGenerator& operator=(BuildingGenerator&& t_other) noexcept = delete;

        ~BuildingGenerator() noexcept;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        [[nodiscard]] const ogl::resource::Mesh& GetBuildingsMesh() const noexcept;
        [[nodiscard]] ogl::resource::Mesh& GetBuildingsMesh() noexcept;

        [[nodiscard]] uint32_t GetQuadTextureAtlasId() const;

    protected:

    private:
        /**
         * @brief The parent Scene object.
         */
        ogl::scene::Scene* m_scene{ nullptr };

        MeshSharedPtr m_quadMesh;

        uint32_t m_quadTextureAtlasId{ 0 };

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        void Init();
    };
}
