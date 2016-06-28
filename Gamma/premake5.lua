-- premake5.lua
workspace "Gamma"
   configurations { "Debug", "Release" }

project "Gamma"
   kind "WindowedApp"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"

   files { "Src/**.h", "Src/**.cpp" }

   filter "configurations:Debug"
      defines { "GAMMA_DEBUG", "DEBUG" }
      flags { "Symbols" }

   filter "configurations:Release"
      defines { "GAMMA_RELEASE", "RELEASE" }
      optimize "On"