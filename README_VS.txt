DEEP DARK - Visual Studio project (no CMake, no vcpkg, no json)

WHAT YOU NEED (the same downloads as the course activities)
  GLEW  glew-2.3.1  (Binaries)        -> must contain include\GL\glew.h and lib\Release\x64\glew32s.lib
  GLFW  glfw-3.4.bin.WIN64            -> must contain include\GLFW\glfw3.h and lib-vc2022\glfw3.lib
  GLM   glm (from github.com/g-truc/glm/releases, glm-1.0.x.zip)
                                      -> a folder that contains another folder named glm, i.e. glm\glm\glm.hpp

STEPS
 1. Open Dependencies.props in Notepad. Change the three paths (GLEW_DIR, GLFW_DIR, GLM_DIR) to where those
    folders are on your computer. Save.
 2. Double-click DeepDark.sln. At the top set the configuration to Debug (or Release) and the platform to x64.
 3. Press F5. The exe, Shaders\ and level.txt are put in bin\Debug\ automatically.

GLEW is linked statically (GLEW_STATIC + glew32s.lib), so no DLL has to be copied anywhere.

IF IT DOES NOT BUILD
  "Cannot open include file glm/glm.hpp"  -> GLM_DIR is one level too deep or too shallow.
  "Cannot open input file glew32s.lib"    -> GLEW_DIR is wrong, or you downloaded the source zip, not the Binaries.
  "Cannot open input file glfw3.lib"      -> GLFW_DIR is wrong, or lib-vc2022 is named differently (try lib-vc2019).

CONTROLS   WASD move | mouse look | arrows turn | ENTER start / next level | R restart | Esc quit
           X shadows off | C point-light maps static / every frame | 0-4 shadow casters | M benchmark (300 frames)
FILES      See OWNERS.txt. Edit level.txt and Shaders\ and press R / restart - no rebuild needed for level.txt.
