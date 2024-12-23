workspace "ClientServer"
    architecture "x64"
    configurations { "Debug", "Release" }

targetdir "build/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Build configurations
filter "configurations:Debug"
    defines "DEBUG"
    symbols "On"

filter "configurations:Release"
    defines "NDEBUG"
    optimize "On"

-- Windows system
filter "system:windows"
    system "windows"
    cppdialect "C++17"
    systemversion "latest"
    files { "./include/Socked/sockedwin32.cpp" }
    -- flags { "MultiProcessorCompile" }

-- Linux system
filter "system:linux"
    system "linux"
    cppdialect "gnu++17"
    files { "./include/Socked/sockedlinux.cpp" }
    -- buildoptions { "-fopenmp" }
    -- links { "omp", "glfw", "GL", "m", "X11", "pthread", "Xi", "Xrandr", "dl" }

project "Client"
    location "%{prj.name}"
    kind "ConsoleApp"
    language "C++"

    files { 
        "./%{prj.name}/**.h",
        "./%{prj.name}/**.hpp",
        "./%{prj.name}/**.c",
        "./%{prj.name}/**.cpp"
    }

    includedirs {
        -- Project source code
        "./%{prj.name}/",
        "./include/",
    }

project "Server"
    location "%{prj.name}"
    kind "ConsoleApp"
    language "C++"

    files { 
        "./%{prj.name}/**.h",
        "./%{prj.name}/**.hpp",
        "./%{prj.name}/**.c",
        "./%{prj.name}/**.cpp"
    }

    includedirs {
        -- Project source code
        "./%{prj.name}/",
        "./include/"
    }

