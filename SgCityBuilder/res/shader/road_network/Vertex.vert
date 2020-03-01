#version 330

// In

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in float aTexture;
layout (location = 4) in vec2 aUv;

// Out

out vec3 vPosition;
out float vTexture;
out vec2 vUv;

// Uniforms

uniform mat4 mvpMatrix;

// Main

void main()
{
    gl_Position = mvpMatrix * vec4(aPosition, 1.0);
    vPosition = aPosition;
    vTexture = aTexture;
    vUv = aUv;
}
