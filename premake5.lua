common_deps_dir = "E:/workspace/common_deps/"
reps_dir = "E:/workspace/reps/"

workspace "Workspace"
	configurations {"Debug", "Release"}
	location "./build"
    
    includedirs {    
        common_deps_dir .. "boost_1_64_0",
        reps_dir .. "**/include",
    }
    
    libdirs {
        common_deps_dir .. "boost_1_64_0/stage/lib",
        reps_dir .. "**/lib",
	}
    
    filter "configurations:Debug"
		targetsuffix "_d"
		defines {"DEBUG", "_DEBUG", "_CRT_SECURE_NO_WARNINGS", "_SCL_SECURE_NO_WARNINGS", "MSVC_IMPLICIT_LINK"}
		symbols "On"
		
	filter "configurations:Release"        
		defines {"NDEBUG", "_CRT_SECURE_NO_WARNINGS", "_SCL_SECURE_NO_WARNINGS", "MSVC_IMPLICIT_LINK"}
		optimize "On"

project "ThreadPool"
	kind "StaticLib"
	targetdir "./lib"
	
	includedirs {
		"./include",
	}
	
	files {
		"./include/**.h",
		"./src/**.cpp",
	}
	
project "ThreadPoolTests"
	kind "ConsoleApp"
	targetdir "./bin"
	
    includedirs {    
        common_deps_dir .. "googletest/googletest/include",
		common_deps_dir .. "googletest/googlemock/include",
        reps_dir .. "gtest_wrapper/include",
        "./include",
	}
	
	libdirs {
		common_deps_dir .. "googletest/lib",
	}
	
	files {
		"./tests/main.cpp",
	}
	
	links {
		"ThreadPool",
	}