-- File groups
HEADERS  = "include/**.h"
IMPLEMENTATION  = "src/**.cpp"
APP_SOURCES     = "test/**.cpp"

workspace "graphski"
    configurations { "Debug", "Release" }
    platforms { "Win32" ,"Win64" }

    startproject "graphski-app"

------------------------
-- LIBRARY PROJECT
------------------------
project "graphski"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    files {
        HEADERS,
        IMPLEMENTATION
    }
    includedirs { 
        "include/",
        "src/private_headers/",
        "dependencies/SFML-3.0.0/include",
        "dependencies/nlohmann"
    }
    libdirs { "dependencies/SFML-3.0.0/lib" }
    defines { "SFML_STATIC" }

    vpaths {
        ["Public Headers"]   = { HEADERS },
        ["Source Files"]     = { IMPLEMENTATION }
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        runtime "Debug"
        symbols "On"
        targetdir "build/debug"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"
        targetdir "build/release"

------------------------
-- APP / TEST PROJECT
------------------------
project "graphski-app"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    files { APP_SOURCES }
    includedirs { 
        "include/",
        "src/private_headers/",
        "dependencies/SFML-3.0.0/include",
        "dependencies/nlohmann"
    }
    libdirs { 
        "dependencies/SFML-3.0.0/lib", 
        "build/debug", -- For Debug
        "build/release" -- For Release
    }
    links { 
        "graphski",
        "freetyped.lib",
        "opengl32.lib",
        "gdi32.lib",
        "winmm.lib"
    }
    defines { "SFML_STATIC" }

    vpaths {
        ["App Sources"] = { APP_SOURCES }
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        runtime "Debug"
        symbols "On"
        targetdir "build/debug"
        links {
            "sfml-system-s-d.lib",
            "sfml-graphics-s-d.lib",
            "sfml-window-s-d.lib",
            "freetyped.lib",
            "opengl32.lib",
            "gdi32.lib",
            "winmm.lib"
        }

    filter "configurations:Release"
        runtime "Release"
        optimize "On"
        targetdir "build/release"
        links {
            "sfml-system-s.lib",
            "sfml-graphics-s.lib",
            "sfml-window-s.lib",
            "freetype.lib",
            "opengl32.lib",
            "gdi32.lib",
            "winmm.lib"
        }
