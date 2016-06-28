-- premake5.lua
workspace "Gamma"
   configurations { "Debug", "Release" }
   filter {"action:vs*"}
      platforms { "Game Win32", "Game Win64" }

project "Gamma"
   basedir "Src"
   location "Src"
   kind "WindowedApp"

   files { "Src/**.h", "Src/**.cpp" }

   filter "configurations:Debug"
      defines { "GAMMA_DEBUG", "DEBUG" }
      flags { "Symbols" }

   filter "configurations:Release"
      defines { "GAMMA_RELEASE", "RELEASE" }
      optimize "On"

   filter { "platforms:Game Win32" }
      system "windows"
      architecture "x32"
      defines { "DEV_WINDOWS", "GAME_WINDOWS", "GAME_WINDOWS32", "UNICODE", "WIN32_LEAN_AND_MEAN" }
   filter { "platforms:Game Win64" }
      system "windows"
      architecture "x86_64"
      defines { "DEV_WINDOWS", "GAME_WINDOWS", "GAME_WINDOWS64", "UNICODE", "WIN32_LEAN_AND_MEAN" }

   filter { "configurations:Debug" , "platforms:Game Win32" }
      links { }
   filter { "configurations:Release", "platforms:Game Win32" }
      links { }
   filter { "configurations:Debug", "platforms:Game Win64" }
      links { }
   filter { "configurations:Release", "platforms:Game Win64" }
      links { }

   filter "configurations:Debug"
      defines { "DEBUG" }
      flags { "Symbols" }

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"