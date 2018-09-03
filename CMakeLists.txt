cmake_minimum_required(VERSION 3.1)
project(OpenSWE1R)

option(USE_VM "Use hardware virtualization backend for Unicorn-Engine" OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(ENet)
find_package(Unicorn REQUIRED)
find_package(Keystone REQUIRED)
find_package(Capstone REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)

add_subdirectory (ext/glfw)

if(ENET_FOUND)
  include_directories(SYSTEM
    ${ENET_INCLUDE_DIR}
  )
endif()

include_directories(SYSTEM
  ${LIBUNICORN_INCLUDE_DIR}
  ${LIBKEYSTONE_INCLUDE_DIR}
  ${LIBCAPSTONE_INCLUDE_DIR}
  ${OPENGL_INCLUDE_DIR}
  ${OPENAL_INCLUDE_DIR}
  ${GLFW_SOURCE_DIR}/include
  ext/glad/include
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
  windows.h
  common.h
  exe.h

  com/ddraw.h
  com/dinput.h
  com/a3d.h
  com/d3d.h

  ext/glad/src/glad.c
)

if(USE_VM)
  target_compile_definitions(openswe1r PUBLIC -DUC_KVM)
  target_sources(openswe1r PUBLIC
    uc_kvm.c
  )
else()
  target_link_libraries(openswe1r
    ${LIBUNICORN_LIBRARY}
  )
endif()

if(ENET_FOUND)
  target_compile_definitions(openswe1r PUBLIC -DDPLAY_ENET)
  target_link_libraries(openswe1r
    ${ENET_LIBRARIES}
  )
endif()

target_link_libraries(openswe1r
  ${OPENGL_LIBRARIES}
  ${OPENAL_LIBRARY}
  ${LIBKEYSTONE_LIBRARY}
  ${LIBCAPSTONE_LIBRARY}
  glfw
)
