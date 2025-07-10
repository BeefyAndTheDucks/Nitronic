project "NitronicCore"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/**.h",
        "src/**.cpp",
    }

    defines
    {

    }

    includedirs
    {
        "src"
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "SYS_WINDOWS"
        }

    filter "configurations:Debug"
        defines "N_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "N_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "N_DIST"
        runtime "Release"
        optimize "on"