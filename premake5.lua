workspace "SgCityBuilder"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release"
    }

    floatingpoint "Fast"
    flags "MultiProcessorCompile"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "SgCityBuilder"
    location "SgCityBuilder"
    architecture "x64"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/sgogl/src",
        "%{prj.name}/vendor/sgogl/src/SgOglLib",
        "%{prj.name}/vendor/sgogl/vendor/spdlog/include",
        "%{prj.name}/vendor/sgogl/vendor/glew/include",
        "%{prj.name}/vendor/sgogl/vendor/glfw/include",
        "%{prj.name}/vendor/sgogl/vendor/tinyxml2/include",
        "%{prj.name}/vendor/sgogl/vendor/glm",
        "%{prj.name}/vendor/sgogl/vendor/assimp/include",
        "%{prj.name}/vendor/sgogl/vendor/entt",
        "%{prj.name}/vendor/sgogl/vendor/imgui",
        "%{prj.name}/vendor/sgogl/vendor/gli"
    }

    links
    {
        "SgOglLib"
    }

    linkoptions
    {
        "/IGNORE:4099"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines "SG_OGL_DEBUG_BUILD"
        runtime "Debug"
        symbols "On"
        libdirs
        {
            "%{prj.name}/vendor/sgogl/lib/debug",
        }

    filter "configurations:Release"
        runtime "Release"
        optimize "On"
        libdirs
        {
            "%{prj.name}/vendor/sgogl/lib/release",
        }
