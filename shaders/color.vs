#version 150
#extension GL_ARB_explicit_attrib_location : enable

layout(location=0) in vec3 in_Position;
layout(location=2) in vec3 in_Normal;
layout(location=3) in vec4 in_Color;
layout(location=4) in ivec4 in_joint_indicies0;
layout(location=5) in vec4 in_joint_membership0;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 joints[50];

out vec4 v_Color;

void main( void )
{
    mat4 mvp_mat = ProjectionMatrix * ViewMatrix * ModelMatrix;

    vec4 first_joint = (joints[in_joint_indicies0[0]] * vec4(in_Position, 1.0)) * in_joint_membership0[0];
    vec4 second_joint = (joints[in_joint_indicies0[1]] * vec4(in_Position, 1.0)) * in_joint_membership0[1];
    vec4 third_joint = (joints[in_joint_indicies0[2]] * vec4(in_Position, 1.0)) * in_joint_membership0[2];
    vec4 v_prime = first_joint + second_joint + third_joint;
    v_prime.w = 1.0;

    gl_Position = mvp_mat * v_prime;
    //v_Color = in_Color;
    v_Color = vec4(1.0,0.0,0.0,1.0);

}
