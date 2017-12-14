#version 330 core

layout(location = 0) in vec3 ObjectSpace_position; // object-space vertex position
layout(location = 1) in vec3 ObjectSpace_normal;   // object-space vertex normal

uniform mat4 model, view, projection;

out vec4 WorldSpace_position;
out vec4 WorldSpace_normal;

out vec4 CameraSpace_position;
out vec4 CameraSpace_normal;

void main() {
    vec4 position_cameraSpace = view * model * vec4(ObjectSpace_position, 1.0);
    vec4 normal_cameraSpace = vec4(normalize(mat3(transpose(inverse(view * model))) * ObjectSpace_normal), 0);

//    vec4 position_worldSpace = model * vec4(ObjectSpace_position, 1.0);
//    vec4 normal_worldSpace = vec4(normalize(mat3(transpose(inverse(model))) * normal), 0);

//    if (useArrowOffsets) {
        // Figure out the axis to use in order for the triangle to be billboarded correctly
//        vec3 offsetAxis = normalize(cross(vec3(position_cameraSpace), vec3(normal_cameraSpace)));
//        position_cameraSpace += arrowOffset * vec4(offsetAxis, 0);
//    }

    gl_Position = projection * position_cameraSpace;
//    gl_Position = projection * view * model * vec4(ObjectSpace_position, 1.0);
    WorldSpace_position = model * vec4(ObjectSpace_position, 1.0);
//    WorldSpace_normal = model * vec4(ObjectSpace_normal, 0);
    WorldSpace_normal = vec4(normalize(mat3(transpose(inverse(model))) * ObjectSpace_normal), 0);

    CameraSpace_position = view * WorldSpace_position;
//    CameraSpace_normal = view * WorldSpace_normal;
    CameraSpace_normal = vec4(normalize(mat3(transpose(inverse(view * model))) * ObjectSpace_normal), 0);
}
