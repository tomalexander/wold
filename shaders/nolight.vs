#version 150
#extension GL_ARB_explicit_attrib_location : enable

layout(location=0) in vec3 in_Position;
layout(location=1) in vec2 in_Tex;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec2 v_tex;

void main( void )
{
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(in_Position, 1.0);
	v_tex = in_Tex;
}
