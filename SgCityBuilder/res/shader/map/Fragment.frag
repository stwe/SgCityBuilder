#version 330

// In

in vec3 vWorldPosition;
in vec3 vPosition;
in vec3 vColor;
in float vTexture;
in vec2 vUv;

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
uniform sampler2D tileTexture[5];
uniform float showRegionColor;

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
    vec3 viewDir = normalize(cameraPosition - vWorldPosition);

    if(showRegionColor > 0.5)
    {
        diffuse = vec4(vColor, 1.0);
    }
    else
    {
        if (vTexture == 0)
            diffuse = texture(tileTexture[0], vec2(vUv.x, 1.0 - vUv.y));
        else if (vTexture == 1)
            diffuse = texture(tileTexture[1], vec2(vUv.x, 1.0 - vUv.y));
        else if (vTexture == 2)
            diffuse = texture(tileTexture[2], vec2(vUv.x, 1.0 - vUv.y));
        else if (vTexture == 3)
            diffuse = texture(tileTexture[3], vec2(vUv.x, 1.0 - vUv.y));
        else
            diffuse = texture(tileTexture[4], vec2(vUv.x, 1.0 - vUv.y));
    }

    // calc ambient
    vec3 ambientIntensity = vec3(0.2, 0.2, 0.2);
    vec3 ambient = ambientIntensity * diffuse.rgb;

    // calc directional light
    vec3 lightResult = CalcDirectionalLight(normal, viewDir);

    // result
    fragColor = vec4(ambient + lightResult, 1.0);
}
