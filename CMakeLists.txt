cmake_minimum_required (VERSION 2.8.3)
project(cad-app)

# Enable C++11 mode on GCC / Clang
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()


# Compile with compiler warnings
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter")
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-gnu-anonymous-struct -Wno-c99-extensions -Wno-nested-anon-types")
  endif()
endif()

# Build NanoGUI
set(NANOGUI_BUILD_EXAMPLE OFF CACHE BOOL " " FORCE)
set(NANOGUI_BUILD_SHARED  OFF CACHE BOOL " " FORCE)
set(NANOGUI_BUILD_PYTHON  OFF CACHE BOOL " " FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/nanogui ext_build/nanogui)
set_property(TARGET nanogui glfw glfw_objects bin2c PROPERTY FOLDER "dependencies")

# Header file directories of dependency libraries
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/external/nanogui/include
  ${NANOGUI_EXTRA_INCS} # Eigen, GLFW, NanoVG, GLAD
)

add_executable(cad-app
  src/main.cpp
  src/viewer.h src/viewer.cpp
  src/mesh.h src/mesh.cpp
  src/opengl.h src/opengl.cpp
  src/shader.h src/shader.cpp
  src/control.h src/control.cpp
  src/loader.h src/loader.cpp
)

add_definitions(${NANOGUI_EXTRA_DEFS})
target_link_libraries(cad-app nanogui ${NANOGUI_EXTRA_LIBS})

# set_target_properties(cad-app PROPERTIES LINK_FLAGS "-static-libgcc")


