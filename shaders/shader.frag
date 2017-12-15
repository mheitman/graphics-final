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

uniform float blend;

in vec4 WorldSpace_position;
in vec4 WorldSpace_normal;

in vec4 CameraSpace_position;
in vec4 CameraSpace_normal;

uniform sampler2D tex_sampler;
uniform sampler2D normal_sampler;
in vec2 uv;

out vec3 fragColor;

void main(){

//    float d = sqrt(pow(WorldSpace_lightPos1.x - WorldSpace_position.x, 2)
//                   + pow(WorldSpace_lightPos1.y - WorldSpace_position.y, 2)
//                   + pow(WorldSpace_lightPos1.z - WorldSpace_position.z, 2));
//    float atten = lightIntensity * min(1 / (attConstant + attLinear * d + attQuadratic * pow(d, 2)), 1);
//    fragColor = ambientIntensity * color;
//    vec3 n = normalize(WorldSpace_normal.xyz);
//    vec3 L = normalize(WorldSpace_lightPos1 - WorldSpace_position.xyz);
//    float prod = max(0, dot(n, L));
//    fragColor += atten * color * lightColor * diffuseIntensity * prod;

//    vec3 E = normalize(CameraSpace_position.xyz);
//    vec3 R = reflect(L, n);
//    float highlight = pow(max(0, dot(E, R)), shininess);
//    fragColor += atten * color * lightColor * specularIntensity * highlight;
    vec3 phong_color;

    float d = sqrt(pow(WorldSpace_lightPos1.x - WorldSpace_position.x, 2)
                   + pow(WorldSpace_lightPos1.y - WorldSpace_position.y, 2)
                   + pow(WorldSpace_lightPos1.z - WorldSpace_position.z, 2));
    float atten = lightIntensity * min(1 / (attConstant + attLinear * d + attQuadratic * pow(d, 2)), 1);
    phong_color = ambientIntensity * color;
    vec3 n = normalize(texture(normal_sampler, uv).xyz);
    vec3 L = normalize(WorldSpace_lightPos1 - WorldSpace_position.xyz);
    float prod = max(0, dot(n, L));
    phong_color += atten * color * lightColor * diffuseIntensity * prod;

    vec3 E = normalize(CameraSpace_position.xyz);
    vec3 R = reflect(L, n);
    float highlight = pow(max(0, dot(E, R)), shininess);
    phong_color += atten * color * lightColor * specularIntensity * highlight;

    vec3 tex_color = texture(tex_sampler, uv).xyz;
    fragColor = mix(phong_color, tex_color, blend);

//    uv.y = 1.0 - uv.y;
//    fragColor = texture(tex_sampler, uv).xyz;
//    if (WorldSpace_position.z < -0.01) {
//        fragColor = texture(normal_sampler, uv).xyz;
//    }


}
