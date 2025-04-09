workspace "graphski"
	configurations { "Debug", "Release" }
	platforms { "Win32" ,"Win64" } 

project "graphski"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17" -- required by SFML

	files {
		"src/**.h",
		"src/**.cpp"
	}

	-- Set up filters in Visual Studio:
    vpaths {
        ["Header Files"] = { "src/**.h" },
        ["Source Files"] = { "src/**.cpp" }
    }

	-- add SFML includes
	includedirs { "dependencies/SFML-3.0.0/include" }
	-- add SFML lib path for linker
	libdirs { "dependencies/SFML-3.0.0/lib" }
	
	--using static libraries
	defines { "SFML_STATIC" } 

	filter ("configurations:Debug")
		defines{"DEBUG"}
		runtime "Debug"
		links {
			"sfml-system-s-d.lib",
			"sfml-graphics-s-d.lib",
			"sfml-window-s-d.lib",
			"freetyped.lib",
			"opengl32.lib",
			"gdi32.lib",
    		"winmm.lib"
		}
		targetdir "bin/debug"

	filter ("configurations:Release")
		runtime "Release"
		links {
			"sfml-system-s.lib",
			"sfml-graphics-s.lib",
			"sfml-window-s.lib",
			"freetype.lib",
			"opengl32.lib",
			"gdi32.lib",
    		"winmm.lib"
		}
		optimize "On"
		targetdir "bin/release"