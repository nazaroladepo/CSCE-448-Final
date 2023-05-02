cmake_minimum_required(VERSION 3.1) # setting this is required
project(single_view_modeling)       # this sets the project name

###############################################################################
## file globbing ##############################################################
###############################################################################

# These instructions search the directory tree when cmake is
# invoked and put all files that match the pattern in the variables 
# `sources` and `data`.
file(GLOB_RECURSE sources      src/*.cpp)
file(GLOB_RECURSE sources_test test/*.cpp)
file(GLOB_RECURSE data resources/*)
# You can use set(sources src/main.cpp) etc if you don't want to
# use globbing to find files automatically.

###############################################################################
## target definitions #########################################################
###############################################################################

set(CMAKE_OSX_ARCHITECTURES arm64)

# The data is just added to the executable, because in some IDEs (QtCreator) 
# files are invisible when they are not explicitly part of the project.
add_executable(single_view_modeling ${sources} ${data})

# Just for example add some compiler flags.
target_compile_options(single_view_modeling PUBLIC -std=c++14 -Wall -Wextra -g)

# This allows to include files relative to the root of the src directory with a <> pair
target_include_directories(single_view_modeling PUBLIC src/main)

# This copies all resource files in the build directory.
# We need this, because we want to work with paths relative to the executable.
file(COPY ${data} DESTINATION resources)

###############################################################################
## dependencies ###############################################################
###############################################################################

add_subdirectory(external/glfw glfw_files)

include_directories(
  external/glad/include/
  external/glm/
  external/
  external/glfw/include/
)

add_library(glad STATIC external/glad/src/glad.c)

# find_package(glfw3 3.3 REQUIRED)

set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)

target_link_libraries(single_view_modeling PUBLIC
  glfw
  OpenGL::GL
  glad
  # here you can add any library dependencies
)


###############################################################################
## testing ####################################################################
###############################################################################

# This is for our testing framework,
# we don't add REQUIRED because it's just for testing.
# People who might want to build the project to use it should not be required
# to install testing dependencies.
find_package(GTest)

if(GTEST_FOUND)
  add_executable(unit_tests ${sources_test} ${sources})

  # This define is added to prevent collision with the main.
  # It might be better solved by not adding the source with the main to the
  # testing target.
  target_compile_definitions(unit_tests PUBLIC UNIT_TESTS)

  # This allows us to use the executable as a link library, and inherit all 
  # linker options and library dependencies from it, by simply adding it as dependency.
  set_target_properties(single_view_modeling PROPERTIES ENABLE_EXPORTS on)

  target_link_libraries(unit_tests PUBLIC
    ${GTEST_BOTH_LIBRARIES}
    single_view_modeling
  )

  target_include_directories(unit_tests PUBLIC
    ${GTEST_INCLUDE_DIRS} # doesn't do anything on linux
  )
  
endif()
