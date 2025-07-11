project "NitronicEditor"
    kind "ConsoleApp"
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
		"%{wks.location}/NitronicCore/vendor/spdlog/include",
        "%{wks.location}/NitronicCore/src",
		"%{wks.location}/NitronicCore/vendor"
    }

    links
    {
        "NitronicCore"
    }

    filter "system:windows"
        systemversion "latest"

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
