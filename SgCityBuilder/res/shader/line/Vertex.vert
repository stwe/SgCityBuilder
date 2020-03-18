#version 330

// In

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;

// Out

out vec3 vColor;

// Uniforms

uniform mat4 mvpMatrix;

// Main

void main()
{
    gl_Position = mvpMatrix * vec4(aPosition, 1.0);
    vColor = aColor;
}
