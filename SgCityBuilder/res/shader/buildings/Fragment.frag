#version 330

// In

in vec3 vPosition;
in vec2 vUv;
in vec3 vColor;
in float vUseTexture;

// Out

out vec4 fragColor;

// Types

struct DirectionalLight
{
    vec3 direction;
    vec3 diffuseIntensity;
    vec3 specularIntensity;
};

// Uniforms

uniform DirectionalLight directionalLight;
uniform vec3 cameraPosition;
uniform sampler2D quadTextureAtlas0;
uniform sampler2D quadTextureAtlas1;

// Global

vec4 diffuse;

// Function

vec3 CalcDirectionalLight(vec3 normal, vec3 viewDir)
{
    // negate the global light direction vector to switch its direction
    // it's now a direction vector pointing towards the light source
    vec3 lightDir = normalize(-directionalLight.direction);

    // diffuse
    float diffuseFactor = max(dot(normal, lightDir), 0.0);
    vec3 diff = directionalLight.diffuseIntensity * diffuseFactor * diffuse.rgb;

    // specular
    //vec3 reflectDir = reflect(-lightDir, normal);
    //float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    //vec3 spec = directionalLight.specularIntensity * specularFactor * specular.rgb;

    // result
    return diff; // + spec;
}

// Main

void main()
{
    vec3 normal = vec3(0.0, 1.0, 0.0);
    vec3 viewDir = normalize(cameraPosition - vPosition);

    vec4 col = vec4(vColor, 1.0);
    if (vUseTexture < 0.5)
    {
        diffuse = col;
    }
    else if (vUseTexture > 1.5)
    {
        diffuse = texture(quadTextureAtlas1, vec2(vUv.x, vUv.y)) * col;
    }
    else
    {
        diffuse = texture(quadTextureAtlas0, vec2(vUv.x, vUv.y)) * col;
    }

    // calc ambient
    vec3 ambientIntensity = vec3(0.2, 0.2, 0.2);
    vec3 ambient = ambientIntensity * diffuse.rgb;

    // calc directional light
    vec3 lightResult = CalcDirectionalLight(normal, viewDir);

    // result
    fragColor = vec4(ambient + lightResult, 1.0);
}
