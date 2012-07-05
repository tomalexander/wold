#version 150
#extension GL_ARB_explicit_attrib_location : enable

layout(location=0) in vec3 in_Position;
layout(location=2) in vec3 in_Normal;
layout(location=1) in vec2 in_Tex;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec2 v_tex;


uniform vec3 WCLightPos;       // Position of light in world coordinates
uniform vec4 ViewPosition;     // Position of camera in world space
uniform mat4 WCtoLC;           // World to light coordinate transform
uniform mat4 WCtoLCit;         // World to light inverse transpose
uniform mat4 MCtoWC;           // Model to world coordinate transform
uniform mat4 MCtoWCit;         // Model to world inverse transpose

out vec3 LCpos;            // Vertex position in light coordinates
out vec3 LCnorm;           // Normal in light coordinates
out vec3 LCcamera;         // Camera position in light coordinates

void main()
{
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(in_Position, 1.0);
    v_tex = in_Tex;

    // Compute world space position and normal
    vec4 wcPos = MCtoWC * vec4(in_Position, 0.0);
    vec3 wcNorm = (MCtoWCit * vec4(in_Normal, 0.0)).xyz;

    // Compute light coordinate system camera position,
    // vertex position and normal
    LCcamera = (WCtoLC * ViewPosition).xyz;

    LCpos = (WCtoLC * wcPos).xyz;
    LCnorm = (WCtoLCit * vec4(wcNorm, 0.0)).xyz;
}
