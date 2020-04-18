#version 330

// In

in vec2 vUv;
in vec3 vColor;
in float vUseTexture;

// Out

out vec4 fragColor;

// Uniforms

uniform sampler2D quadTextureAtlas0;
uniform sampler2D quadTextureAtlas1;

// Main

void main()
{
    vec4 col = vec4(vColor, 1.0);

    if (vUseTexture < 0.5)
    {
        fragColor = col;
    }
    else if (vUseTexture > 1.5)
    {
        fragColor = texture(quadTextureAtlas1, vec2(vUv.x, vUv.y)) * col;
    }
    else
    {
        fragColor = texture(quadTextureAtlas0, vec2(vUv.x, vUv.y)) * col;
    }
}
