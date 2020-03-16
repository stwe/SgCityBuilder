#version 330

// In

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUv;
layout (location = 3) in mat4 aInstanceMatrix;

// Out

out vec2 vUv;

// Uniforms

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

// Main

void main()
{
    gl_Position = projectionMatrix * viewMatrix * aInstanceMatrix * vec4(aPosition, 1.0);
    vUv = aUv;
}
