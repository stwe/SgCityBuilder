#version 330

// In

in vec3 vPosition;
in vec3 vColor;
in float vTexture;

// Out

out vec4 fragColor;

// Uniforms

uniform sampler2D tileTexture[5];

// Main

void main()
{
    int textureIndex = int(vTexture);
    fragColor = texture(tileTexture[textureIndex], vec2(vPosition.x, vPosition.z));

    //fragColor = vec4(vColor, 1.0);
}
