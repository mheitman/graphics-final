#version 330 core

uniform mat4 model, view, projection;

// Light properties
const vec3 WorldSpace_lightPos1 = vec3(2, 2, 2); // world-space light position
const vec3 WorldSpace_lightPos2 = vec3(-2, 2, 2); // world-space light position
uniform vec3 lightColor;
uniform float lightIntensity;

// Attenuation Properties
uniform float attQuadratic;
uniform float attLinear;
uniform float attConstant;

// Material properties
uniform vec3 color;
uniform float ambientIntensity;
uniform float diffuseIntensity;
uniform float specularIntensity;
uniform float shininess;

in vec4 WorldSpace_position;
in vec4 WorldSpace_normal;

in vec4 CameraSpace_position;
in vec4 CameraSpace_normal;

uniform sampler2D sampler;
in vec2 uv;

out vec3 fragColor;

void main(){

    float d = sqrt(pow(WorldSpace_lightPos1.x - WorldSpace_position.x, 2)
                   + pow(WorldSpace_lightPos1.y - WorldSpace_position.y, 2)
                   + pow(WorldSpace_lightPos1.z - WorldSpace_position.z, 2));
    float atten = lightIntensity * min(1 / (attConstant + attLinear * d + attQuadratic * pow(d, 2)), 1);
    fragColor = ambientIntensity * color;
    vec3 n = normalize(WorldSpace_normal.xyz);
    vec3 L = normalize(WorldSpace_lightPos1 - WorldSpace_position.xyz);
    float prod = max(0, dot(n, L));
    fragColor += atten * color * lightColor * diffuseIntensity * prod;

    vec3 E = normalize(CameraSpace_position.xyz);
    vec3 R = reflect(L, n);
    float highlight = pow(max(0, dot(E, R)), shininess);
    fragColor += atten * color * lightColor * specularIntensity * highlight;

    // temp
    //fragColor = lightColor * 1/abs(WorldSpace_position.z);
    fragColor= texture(sampler, uv).xyz + vec3(0.1, 0.1, 0.1);
    fragColor = vec3(uv.x, uv.y, 1.0);
}
