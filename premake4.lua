solution("topaz")
configurations({"Debug", "Release"})
linkoptions({"-static"})
flags({"StaticRuntime"})
defines({"SFML_STATIC", "GLEW_STATIC"})

configuration("Debug")
flags({"Symbols"})
configuration("Release")
flags({"OptimizeSpeed"})
configuration({})

configuration("linux")
libdirs({"/usr/local/lib/"})
configuration({})

function link_library_deps()
   links({"oolua", "lua", "dl", "physfs", "pthread"})
   link_sfml()
   link_opengl()
end

function link_sfml()
   links({"GLEW", "sfml-window-s", "sfml-graphics-s", "sfml-audio-s", "sfml-network-s", "sfml-system-s", "rt", "jpeg"})
end

function link_opengl()
   configuration("windows")
   links({"glu32", "opengl32", "gdi32", "winmm", "user32"})
   configuration("linux")
   links({"GL", "GLU", "Xrandr", "SM", "ICE", "X11", "Xext"})
   configuration({})
end

project("wold")
kind("WindowedApp")
language("C++")
buildoptions({"-std=c++0x"})
link_library_deps();
files({"src/libmain.cpp", "src/egg_parser.cpp", "src/util.cpp", "src/model.cpp", "src/quaternion.cpp", "src/sqt.cpp", "src/matrix.cpp", "src/panda_node.cpp", "src/shaders.cpp", "src/vector.cpp", "src/camera.cpp", "src/free_view_camera.cpp", "src/light.cpp", "src/nolight.cpp", "src/lookat_camera.cpp", "src/uberlight.cpp", "src/floor_model.cpp", "src/unit.cpp", "src/joint.cpp", "src/animation.cpp", "src/vertex.cpp", "src/gl_program.cpp", "src/rigidbody.cpp", "src/gameobject.cpp", "src/collider.cpp", "src/sphere_collider.cpp", "src/sphere_primitive.cpp", "src/collision.cpp", "src/aabb_collider.cpp", "src/box_primitive.cpp", "src/main.cpp"})
