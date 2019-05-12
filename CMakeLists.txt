cmake_minimum_required(VERSION 3.1)
project(OpenSWE1R)

option(USE_VM "Use hardware virtualization backend for Unicorn-Engine" OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)

add_subdirectory(kebun-kuda/unicorn)
add_subdirectory(kebun-kuda/keystone)
add_subdirectory(kebun-kuda/capstone)
add_subdirectory(ext/glfw)
add_subdirectory(ext/enet)

include_directories(SYSTEM
  ${unicorn_SOURCE_DIR}/include
  ${keystone_SOURCE_DIR}/include
  ${capstone_SOURCE_DIR}/include
  ${OPENGL_INCLUDE_DIR}
  ${OPENAL_INCLUDE_DIR}
  glad/include
  ${GLFW_SOURCE_DIR}/include
  ${enet_SOURCE_DIR}/include
  ext/glm
  ext/imgui
  ext/imgui/examples
  ext/stb
)

add_executable(openswe1r
  main.cpp
  emulation.cpp
  export.cpp
  shader.cpp
  exe.cpp
  common.cpp

  Application.cpp
  Application.hpp
  Game.cpp
  Game.hpp
  StarWars.cpp
  StarWars.hpp

  dll/kernel32.cpp
  dll/user32.cpp
  dll/gdi32.cpp
  dll/advapi32.cpp
  dll/ole32.cpp
  dll/wsock32.cpp
  dll/smush.cpp

  com/a3d.cpp
  com/dplay.cpp
  com/dinput.cpp
  com/ddraw.cpp
  com/d3d.cpp

  main.h
  emulation.h
  export.h
  shader.h

  descriptor.h
  shaders.h
  ms_windows.h
  common.h
  exe.h

  com/ddraw.h
  com/dinput.h
  com/a3d.h
  com/d3d.h

  glad/src/glad.c

  ext/imgui/imgui.cpp
  ext/imgui/imgui.h
  ext/imgui/imgui_internal.h
  ext/imgui/imgui_draw.cpp
  ext/imgui/imgui_widgets.cpp
  ext/imgui/imconfig.h
  ext/imgui/examples/imgui_impl_opengl3.cpp
  ext/imgui/examples/imgui_impl_opengl3.h
  ext/imgui/examples/imgui_impl_glfw.cpp
  ext/imgui/examples/imgui_impl_glfw.h
  ext/imgui/imgui_demo.cpp
  ext/imgui_club/imgui_memory_editor/imgui_memory_editor.h

  ext/stb/stb_textedit.h
  ext/stb/stb_truetype.h
  ext/stb/stb_rect_pack.h
  ext/stb/stb_image.h

  ext/glm/glm/glm.hpp
)

source_group(com com/)
source_group(dll dll/)
source_group(glad glad/)
source_group(imgui ext/imgui/)
source_group(stb ext/stb/)
source_group(glm ext/glm/)

if(USE_VM)
  target_compile_definitions(openswe1r PUBLIC -DUC_KVM)
  target_sources(openswe1r PUBLIC
    uc_kvm.c
  )
else()
  target_link_libraries(openswe1r
    unicorn-static
  )
endif()

target_compile_definitions(openswe1r PUBLIC -DDPLAY_ENET)
target_link_libraries(openswe1r
  enet
)

target_compile_definitions(openswe1r PUBLIC
  -DIMGUI_IMPL_OPENGL_LOADER_GLAD
  -D_CRT_SECURE_NO_WARNINGS)

target_link_libraries(openswe1r
  ${OPENGL_LIBRARIES}
  ${OPENAL_LIBRARY}
  keystone
  capstone-static
  glfw
)

set_target_properties(openswe1r
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/game"
)
