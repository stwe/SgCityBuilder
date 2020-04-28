include("conanbuildinfo.premake.lua")

workspace "SgCityBuilder"
    conan_basic_setup()

    architecture "x64"
    startproject "SgCityBuilder"

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

    linkoptions { conan_exelinkflags }

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "SgOglLib/SgOglLib/src",
        "SgOglLib/SgOglLib/src/SgOglLib",
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
        defines
        {
            "SG_OGL_DEBUG_BUILD",
            "SG_CITY_DEBUG_BUILD"
        }
        runtime "Debug"
        symbols "On"
        libdirs
        {
            "bin/" .. outputdir .. "/SgOglLib/",
        }

    filter "configurations:Release"
        runtime "Release"
        optimize "On"
        libdirs
        {
            "bin/" .. outputdir .. "/SgOglLib/",
        }

project "SgOglLib"
    location "SgOglLib"
    architecture "x64"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    linkoptions { conan_exelinkflags }

    files
    {
        "%{prj.name}/%{prj.name}/src/**.h",
        "%{prj.name}/%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/%{prj.name}/src",
        "%{prj.name}/%{prj.name}/src/SgOglLib",
        "%{prj.name}/%{prj.name}/vendor/gli"
    }

    linkoptions
    {
        "/IGNORE:4099"
    }

    filter "system:windows"
        systemversion "latest"
        defines
        {
            "GLFW_INCLUDE_NONE"
        }

    filter "configurations:Debug"
        defines "SG_OGL_DEBUG_BUILD"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
