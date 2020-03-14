#version 330

// In

in vec3 vPosition;
in vec2 vUv;

// Out

out vec4 fragColor;

// Uniforms

uniform sampler2D roadTextureAtlas;

// Main

void main()
{
    fragColor = texture(roadTextureAtlas, vec2(vUv.x, vUv.y));
}
