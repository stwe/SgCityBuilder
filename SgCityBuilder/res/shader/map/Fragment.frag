#version 330

// In

in vec3 vColor;

// Out

out vec4 fragColor;

// Uniforms

// Main

void main()
{
    fragColor = vec4(vColor, 1.0);
}
