#version 330 core

layout(location = 0) in vec3 ObjectSpace_position; // object-space vertex position
layout(location = 1) in vec3 ObjectSpace_normal;   // object-space vertex normal
layout(location = 2) in vec3 in_color;

uniform mat4 model, view, projection;

out vec4 WorldSpace_position;
out vec4 WorldSpace_normal;

out vec4 CameraSpace_position;
out vec4 CameraSpace_normal;

out vec3 vtx_color;

void main() {

      vtx_color = in_color;
//    vec4 position_worldSpace = model * vec4(ObjectSpace_position, 1.0);
//    vec4 normal_worldSpace = vec4(normalize(mat3(transpose(inverse(model))) * normal), 0);

//    gl_Position = projection * view * model * vec4(ObjectSpace_position, 1.0);
    WorldSpace_position = model * vec4(ObjectSpace_position, 1.0);
    CameraSpace_position = view * WorldSpace_position;
    gl_Position = projection * CameraSpace_position;

//    WorldSpace_normal = model * vec4(ObjectSpace_normal, 0);
    WorldSpace_normal = vec4(normalize(mat3(transpose(inverse(model))) * ObjectSpace_normal), 0);

//    CameraSpace_normal = view * WorldSpace_normal;

    CameraSpace_normal = vec4(normalize(mat3(transpose(inverse(view * model))) * ObjectSpace_normal), 0);


    // TEMP
    // gl_Position = vec4(ObjectSpace_position, 1.0);
}
