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
    if (vTexture == 0)
        fragColor = texture(tileTexture[0], vec2(vUv.x, 1.0 - vUv.y));
    else if (vTexture == 1)
        fragColor = texture(tileTexture[1], vec2(vUv.x, 1.0 - vUv.y));
    else if (vTexture == 2)
        fragColor = texture(tileTexture[2], vec2(vUv.x, 1.0 - vUv.y));
    else if (vTexture == 3)
        fragColor = texture(tileTexture[3], vec2(vUv.x, 1.0 - vUv.y));
    else
        fragColor = texture(tileTexture[4], vec2(vUv.x, 1.0 - vUv.y));

    //fragColor = vec4(vColor, 1.0);
}
