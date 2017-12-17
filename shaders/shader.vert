#version 330 core

layout(location = 0) in vec3 ObjectSpace_position; // object-space vertex position
layout(location = 3) in vec3 WorldSpace_bitangent;
layout(location = 4) in vec3 WorldSpace_tangent;
layout(location = 5) in vec2 UV_coords;

uniform mat4 model, view, projection;

out vec4 WorldSpace_position;
out vec4 CameraSpace_position;

out mat3 tangent_to_world;
out vec3 bad_n;

out vec2 uv;

void main() {

    uv = vec2(UV_coords.x, UV_coords.y);

    WorldSpace_position = model * vec4(ObjectSpace_position, 1.0);
    CameraSpace_position = view * WorldSpace_position;
    gl_Position = projection * CameraSpace_position;

    vec3 WorldSpace_normal = normalize(cross(WorldSpace_tangent, WorldSpace_bitangent));
    bad_n = WorldSpace_normal;
    tangent_to_world = mat3(
        WorldSpace_tangent,
        WorldSpace_bitangent,
        WorldSpace_normal
    );
}
