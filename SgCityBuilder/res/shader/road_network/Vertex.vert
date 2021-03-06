#version 330

// In

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in float aTexture;
layout (location = 4) in vec2 aUv;

// Out

out vec3 vWorldPosition;
out vec3 vPosition;
out vec2 vUv;

// Uniforms

uniform mat4 mvpMatrix;
uniform mat4 worldMatrix;

// Main

void main()
{
    gl_Position = mvpMatrix * vec4(aPosition, 1.0);

    vWorldPosition = vec3(worldMatrix * vec4(aPosition, 1.0));

    vPosition = aPosition;
    vUv = aUv;
}
