include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

workspace "Nitronic"
    architecture "x86_64"
    startproject "NitronicEditor"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

    solution_items
    {
        ".editorconfig"
    }

    flags
    {
        "MultiProcessorCompile"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"

group ""

group "Core"
    include "NitronicCore"
group ""

group "Tools"
    include "NitronicEditor"
group ""

group "Misc"
    include "Sandbox"
group ""