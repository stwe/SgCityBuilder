#version 330

// In

in vec2 vUv;
in vec3 vColor;
in float vUseTexture;

// Out

out vec4 fragColor;

// Uniforms

uniform sampler2D quadTextureAtlas;

// Main

void main()
{
    vec4 col = vec4(vColor, 1.0f);

    if (vUseTexture > 0.5f)
    {
        fragColor = texture(quadTextureAtlas, vec2(vUv.x, vUv.y)) * col;
    }
    else
    {
        fragColor = col;
    }
}
