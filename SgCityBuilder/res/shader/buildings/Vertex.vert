#version 330

// In

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUv;

// Out

out vec2 vUv;

// Uniforms

uniform mat4 mvpMatrix;

// Main

void main()
{
    gl_Position = mvpMatrix * vec4(aPosition, 1.0);
    vUv = aUv;
}