#version 330

// In

in vec3 vPosition;
in float vTexture;
in vec2 vUv;

// Out

out vec4 fragColor;

// Uniforms

uniform sampler2D roadTexture[5];

// Main

void main()
{
    if (vTexture == 0)
        fragColor = texture(roadTexture[0], vec2(vUv.x, 1.0 - vUv.y));
    else if (vTexture == 1)
        fragColor = texture(roadTexture[1], vec2(vUv.x, 1.0 - vUv.y));
    else if (vTexture == 2)
        fragColor = texture(roadTexture[2], vec2(vUv.x, 1.0 - vUv.y));
    else if (vTexture == 3)
        fragColor = texture(roadTexture[3], vec2(vUv.x, 1.0 - vUv.y));
    else if (vTexture == 4)
        fragColor = texture(roadTexture[4], vec2(vUv.x, 1.0 - vUv.y));

    //fragColor = vec4(vColor, 1.0);
}
