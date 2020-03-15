#version 330

// In

in vec2 vUv;

// Out

out vec4 fragColor;

// Uniforms

uniform sampler2D quadTextureAtlas;

// Main

void main()
{
    fragColor = texture(quadTextureAtlas, vec2(vUv.x, vUv.y));
    //fragColor = vec4(1.0, 0.1, 0.1, 1.0);
}
