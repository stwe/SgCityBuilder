#version 330

// In

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUv;
layout (location = 3) in mat4 aInstanceMatrix; // 3, 4, 5, 6
layout (location = 7) in vec4 aInstanceColor;

// Out

out vec2 vUv;
out vec3 vColor;
out float vUseTexture;

// Uniforms

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

// Main

void main()
{
    gl_Position = projectionMatrix * viewMatrix * aInstanceMatrix * vec4(aPosition, 1.0);

    vUv = aUv;
    vColor = aInstanceColor.xyz;
    vUseTexture = aInstanceColor.w;
}
