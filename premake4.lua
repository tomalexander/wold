solution("topaz")
configurations({"Debug", "Release", "Static", "StaticDebug"})
flags {"NoRTTI"}

configuration("Debug")
flags({"Symbols"})

configuration("Release")
flags({"OptimizeSpeed"})

configuration "Static"
buildoptions {"-O4"} -- Link time optimization for nightly builds
flags({"StaticRuntime"})
defines({"SFML_STATIC", "GLEW_STATIC"})

configuration "StaticDebug"
flags({"Symbols"})
flags({"StaticRuntime"})
defines({"SFML_STATIC", "GLEW_STATIC"})

configuration "windows"
linkoptions({"-static"})

configuration("linux")
libdirs({"/usr/local/lib/"})

configuration({})

function link_library_deps()
   configuration "windows"
   links({"oolua", "lua", "physfs", "z", "pthread"})
   configuration {"linux", "Debug or Release"}
   links({"oolua", "lua", "physfs", "z", "pthread", "dl"})
   configuration {"linux", "Static*"}
   linkoptions {"/usr/local/lib/liboolua.a", "/usr/local/lib/liblua.a", "/usr/local/lib/libphysfs.a", "/usr/local/lib/libz.a", "-pthread", "/usr/lib/libdl.a", "-static-libgcc", "-static-libstdc++"}
   links {"c"}
   configuration {}

   link_sfml()
   link_opengl()
end

function link_sfml()
   configuration "windows"
   links {"glew32"}
   configuration {"linux", "Debug or Release"}
   links {"GLEW"}
   configuration {"linux", "Static*"}
   if os.is64bit   then
      linkoptions {"/usr/lib64/libGLEW.a"}
   else
      linkoptions {"/usr/lib/libGLEW.a"}
   end
   configuration {}

   configuration "Debug or Release"
   links {"sfml-window", "sfml-graphics", "sfml-audio", "sfml-network", "sfml-system"}
   configuration {"linux", "Static*"}
   linkoptions {"/usr/local/lib/libsfml-window-s.a", "/usr/local/lib/libsfml-graphics-s.a", "/usr/local/lib/libsfml-audio-s.a", "/usr/local/lib/libsfml-network-s.a", "/usr/local/lib/libsfml-system-s.a"}
   configuration {"windows", "Static*"}
   links {"sfml-window-s", "sfml-graphics-s", "sfml-audio-s", "sfml-network-s", "sfml-system-s"}
   configuration {}

   configuration "windows"
   links {"jpeg"}
   configuration {"linux", "Debug or Release"}
   links {"rt", "jpeg"}
   configuration {"linux", "Static*"}
   linkoptions {"/usr/lib/librt.a", "/usr/lib/libjpeg.a"}
   configuration {}
end

function link_opengl()
   configuration "windows"
   links({"opengl32", "gdi32", "winmm", "user32"})
   configuration {"linux", "Debug or Release"}
   links({"GL", "Xrandr", "Xrender", "SM", "ICE", "Xext", "X11"})
   configuration {"linux", "Static*"}
   links {"GL", "X11"}
   linkoptions {"/usr/lib/libXrandr.a", "/usr/local/lib/libXrender.a", "/usr/lib/libSM.a", "/usr/lib/libICE.a", "/usr/local/lib/libXext.a"}
   configuration({})
end

project("wold")
kind("WindowedApp")
language("C++")
buildoptions({"-std=c++0x"})
link_library_deps();
includedirs {"topaz/src"}
files({"topaz/src/libmain.cpp", "topaz/src/egg_parser.cpp", "topaz/src/util.cpp", "topaz/src/model.cpp", "topaz/src/quaternion.cpp", "topaz/src/sqt.cpp", "topaz/src/matrix.cpp", "topaz/src/panda_node.cpp", "topaz/src/shaders.cpp", "topaz/src/vector.cpp", "topaz/src/camera.cpp", "topaz/src/free_view_camera.cpp", "topaz/src/light.cpp", "topaz/src/nolight.cpp", "topaz/src/lookat_camera.cpp", "topaz/src/uberlight.cpp", "topaz/src/floor_model.cpp", "topaz/src/unit.cpp", "topaz/src/joint.cpp", "topaz/src/animation.cpp", "topaz/src/vertex.cpp", "topaz/src/gl_program.cpp", "topaz/src/rigidbody.cpp", "topaz/src/gameobject.cpp", "topaz/src/collider.cpp", "topaz/src/sphere_collider.cpp", "topaz/src/sphere_primitive.cpp", "topaz/src/collision.cpp", "topaz/src/aabb_collider.cpp", "topaz/src/box_primitive.cpp", "src/main.cpp"})
