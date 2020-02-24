#version 330

// In

in vec3 vPosition;
in vec3 vColor;
in float vTexture;
in vec2 vUv;

// Out

out vec4 fragColor;

// Uniforms

uniform sampler2D tileTexture[5];

// Main

void main()
{
    fragColor = texture(tileTexture[int(vTexture)], vec2(vUv.x, 1.0 - vUv.y)); // todo

    //fragColor = vec4(vColor, 1.0);
}
