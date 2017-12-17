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
in vec4 CameraSpace_position;

in mat3 tangent_to_world;
in vec3 bad_n;

uniform sampler2D tex_sampler;
uniform sampler2D normal_sampler;
uniform sampler2D bump_sampler;

in vec2 uv;
const vec2 size = vec2(0.1,0.0);
const ivec3 off = ivec3(-1,0,1);

out vec3 fragColor;

void main(){

    vec3 phong_color;
    float d = sqrt(pow(WorldSpace_lightPos1.x - WorldSpace_position.x, 2)
                   + pow(WorldSpace_lightPos1.y - WorldSpace_position.y, 2)
                   + pow(WorldSpace_lightPos1.z - WorldSpace_position.z, 2));
    float atten = lightIntensity * min(1 / (attLinear * d + attQuadratic * pow(d, 2)), 1);//attConstant +
    phong_color = ambientIntensity * color;
    vec3 n = normalize(texture(normal_sampler, uv).xyz);

    float s01 = textureOffset(bump_sampler, uv, off.xy).x;
    float s21 = textureOffset(bump_sampler, uv, off.zy).x;
    float s10 = textureOffset(bump_sampler, uv, off.yx).x;
    float s12 = textureOffset(bump_sampler, uv, off.yz).x;
    vec3 va = normalize(vec3(size.xy,s21-s01));
    vec3 vb = normalize(vec3(size.yx,s12-s10));
    vec3 bump_tangent_space = cross(va,vb);
    vec3 flipped_bump = normalize(tangent_to_world * bump_tangent_space);
    vec3 bump = normalize(reflect(-flipped_bump, n));
    //temp
    float lerp = attConstant / 10.0;
    n = bump * lerp + n * (1.0 - lerp);
    n = bump * lerp + flipped_bump * (1.0 - lerp);
    fragColor = n;


    //endtemp
    vec3 L = normalize(WorldSpace_lightPos1 - WorldSpace_position.xyz);
    float lambert = max(0, dot(n, L));
    vec3 tex_color = texture(tex_sampler, uv).xyz;
    phong_color += atten * tex_color * lightColor * diffuseIntensity * lambert;

    vec3 E = normalize(CameraSpace_position.xyz);
    vec3 R = reflect(L, n);
    float highlight = pow(max(0, dot(E, R)), shininess);
    phong_color += atten * color * lightColor * specularIntensity * highlight;

    //fragColor = phong_color;

    //fragColor = normalize(vec3(a, b, 0));
    //fragColor = vec3(texture(bump_sampler, uv).x, 0, 0);

}
