solution("topaz")
configurations({"Debug", "Release", "Static", "StaticDebug"})
flags {"NoRTTI"}

configuration("Debug")
flags({"Symbols"})
configuration("Release")
flags({"OptimizeSpeed"})
configuration "Static"
flags({"OptimizeSpeed"})
flags({"StaticRuntime"})
defines({"SFML_STATIC", "GLEW_STATIC"})
configuration "StaticDebug"
flags({"Symbols"})
flags({"StaticRuntime"})
defines({"SFML_STATIC", "GLEW_STATIC"})
configuration "windows"
linkoptions({"-static"})
configuration({})

configuration("linux")
libdirs({"/usr/local/lib/"})
configuration({})

function link_library_deps()
   links({"oolua", "lua", "physfs", "z", "pthread"})
   configuration "linux"
   links {"dl"}
   configuration {}
   link_sfml()
   link_opengl()
end

function link_sfml()
   configuration "windows"
   links {"glew32"}
   configuration "linux"
   links {"GLEW"}
   configuration {}
   configuration "Debug"
   links {"sfml-window", "sfml-graphics", "sfml-audio", "sfml-network", "sfml-system"}
   configuration "Release"
   links {"sfml-window", "sfml-graphics", "sfml-audio", "sfml-network", "sfml-system"}
   configuration "Static"
   links {"sfml-window-s", "sfml-graphics-s", "sfml-audio-s", "sfml-network-s", "sfml-system-s"}
   configuration "StaticDebug"
   links {"sfml-window-s", "sfml-graphics-s", "sfml-audio-s", "sfml-network-s", "sfml-system-s"}
   configuration {}
   configuration "linux"
   links {"rt"}
   configuration {}
   links {"jpeg"}
end

function link_opengl()
   configuration("windows")
   links({"opengl32", "gdi32", "winmm", "user32"})
   configuration("linux")
   linkoptions {"-dy -lGL -lX11"}
   links({"Xrandr", "Xrender", "SM", "ICE", "Xext"})
   configuration "Static"
   -- Renew -static
   linkoptions {"-static"}   
   configuration({})
end

project("wold")
kind("WindowedApp")
language("C++")
buildoptions({"-std=c++0x"})
link_library_deps();
files({"src/libmain.cpp", "src/egg_parser.cpp", "src/util.cpp", "src/model.cpp", "src/quaternion.cpp", "src/sqt.cpp", "src/matrix.cpp", "src/panda_node.cpp", "src/shaders.cpp", "src/vector.cpp", "src/camera.cpp", "src/free_view_camera.cpp", "src/light.cpp", "src/nolight.cpp", "src/lookat_camera.cpp", "src/uberlight.cpp", "src/floor_model.cpp", "src/unit.cpp", "src/joint.cpp", "src/animation.cpp", "src/vertex.cpp", "src/gl_program.cpp", "src/rigidbody.cpp", "src/gameobject.cpp", "src/collider.cpp", "src/sphere_collider.cpp", "src/sphere_primitive.cpp", "src/collision.cpp", "src/aabb_collider.cpp", "src/box_primitive.cpp", "src/main.cpp"})
