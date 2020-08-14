#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 0) out vec2 v_TexCoord;

layout(std140, binding = 0) uniform UniformBufferObject
{
    mat4 u_ProjectionViewMatrix;
    mat4 u_Transform;
} ubo;

void main()
{
    gl_Position = ubo.u_ProjectionViewMatrix * ubo.u_Transform * vec4(a_Position, 1.0);
    v_TexCoord = a_TexCoord;
}