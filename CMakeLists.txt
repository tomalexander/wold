cmake_minimum_required(VERSION 2.6)
project(topaz)

set(DOXYFILE_LATEX "ON")
include(UseDoxygen REQUIRED)

IF(WIN32)
        set(CMAKE_CXX_FLAGS "-g -static -static-libgcc -static-libstdc++ -std=c++0x")
        #set(CMAKE_CXX_FLAGS "-O2 -static -static-libgcc -static-libstdc++ -std=c++0x")

        set(SFML_STATIC_LIBRARIES "TRUE")
ELSE()
        set(CMAKE_CXX_FLAGS "-g -std=c++0x")
        #set(CMAKE_CXX_FLAGS "-march=native -mtune=native -O2 -std=c++0x")
        #set(CMAKE_CXX_FLAGS "--analyze -std=c++0x") #For clang's static analysis
        set(SFML_STATIC_LIBRARIES "TRUE")
ENDIF()

include_directories(include)
include_directories("${PROJECT_BINARY_DIR}")

# Define sources and executable
set(LIBRARY_NAME "topaz")
set(EXECUTABLE_NAME "game")
set(GAMEAI_NAME "gameai")

set (LIBSOURCES
    src/libmain.cpp
    src/egg_parser.cpp
    src/util.cpp
    src/model.cpp
    src/quaternion.cpp
    src/sqt.cpp
    src/matrix.cpp
    src/panda_node.cpp
    src/shaders.cpp
    src/vector.cpp
    src/camera.cpp
    src/free_view_camera.cpp
    src/light.cpp
    src/nolight.cpp
    src/lookat_camera.cpp
    src/uberlight.cpp
    src/floor_model.cpp
    src/unit.cpp
    src/joint.cpp
    src/animation.cpp
    src/vertex.cpp
    src/gl_program.cpp
    src/rigidbody.cpp
    src/gameobject.cpp
    src/collider.cpp
    src/sphere_collider.cpp
    src/sphere_primitive.cpp
    src/collision.cpp
    src/aabb_collider.cpp
    src/box_primitive.cpp
    src/opencl_accelerator.cpp
    src/opencl_sqt.cpp
)
set (EXECSOURCES
    src/main.cpp
)
set (GAMEAISOURCES
    src/gameaimain.cpp
)

# execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory bin)
add_library(${LIBRARY_NAME} ${LIBSOURCES})
add_executable(${EXECUTABLE_NAME} ${EXECSOURCES})
#add_executable(${GAMEAI_NAME} ${GAMEAISOURCES})
target_link_libraries(${EXECUTABLE_NAME} ${LIBRARY_NAME})
#target_link_libraries(${GAMEAI_NAME} ${LIBRARY_NAME})
target_link_libraries(${LIBRARY_NAME} /usr/lib/liboolua.a)


find_package (Threads)
target_link_libraries (${LIBRARY_NAME} ${CMAKE_THREAD_LIBS_INIT} )

IF(WIN32)
target_link_libraries (${LIBRARY_NAME} /usr/i486-mingw32/lib/libphysfs.a)
ELSE()
find_package(PhysFS REQUIRED)
include_directories(${PHYSFS_INCLUDE_DIR})
target_link_libraries (${LIBRARY_NAME} ${PHYSFS_LIBRARY})
ENDIF()

# Detect and add SFML
find_package(SFML 2 REQUIRED window audio graphics system network)
target_link_libraries(${LIBRARY_NAME} ${SFML_LIBRARIES})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_PATH})
target_link_libraries(${LIBRARY_NAME} ${GLEW_LIBRARY})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(${LIBRARY_NAME} ${OPENGL_LIBRARIES})

IF(WIN32)
find_package(JPEG REQUIRED)
include_directories(${JPEG_INCLUDE_DIR})
target_link_libraries(${LIBRARY_NAME} /usr/i486-mingw32/lib/libjpeg.a)

target_link_libraries(${LIBRARY_NAME} /usr/i486-mingw32/lib/libwinmm.a)
target_link_libraries(${LIBRARY_NAME} /usr/i486-mingw32/lib/libstdc++.a)
ENDIF()

find_package(Lua51 REQUIRED)
include_directories(${LUA_INCLUDE_DIR})
target_link_libraries(${LIBRARY_NAME} ${LUA_LIBRARIES})

find_package(OpenCL REQUIRED)
include_directories(${OPENCL_INCLUDE_DIR})
target_link_libraries(${LIBRARY_NAME} ${OPENCL_LIBRARIES})

# Install Config
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
install (TARGETS ${EXECUTABLE_NAME} RUNTIME DESTINATION bin)
install (DIRECTORY models DESTINATION share/topaz)
install (DIRECTORY textures DESTINATION share/topaz)
install (DIRECTORY shaders DESTINATION share/topaz)
install (DIRECTORY animations DESTINATION share/topaz)
install (DIRECTORY scripts DESTINATION share/topaz)
set_property(TARGET ${EXECUTABLE_NAME} PROPERTY INSTALL_RPATH_USE_LINK_PATH TRUE)

# CPack Config
INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Topaz Game Engine")
SET(CPACK_PACKAGE_VENDOR "Tom Alexander")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/readme.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/copyright.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "3")
SET(CPACK_PACKAGE_VERSION_PATCH "2")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
set(CPACK_PACKAGE_CONTACT "craftkiller@gmail.com")
IF(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\topaz.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} Topaz Engine")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.paphus.com")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.paphus.com")
  SET(CPACK_NSIS_CONTACT "craftkiller@gmail.com")
  SET(CPACK_NSIS_MODIFY_PATH ON)
  SET(CPACK_GENERATOR "ZIP")
ELSE(WIN32 AND NOT UNIX)
  SET(CPACK_STRIP_FILES "")
  SET(CPACK_SOURCE_STRIP_FILES "")
  SET(CPACK_GENERATOR "TBZ2")
ENDIF(WIN32 AND NOT UNIX)

SET(CPACK_PACKAGE_EXECUTABLES ${EXECUTABLE_NAME} "My Executable")
INCLUDE(CPack)
