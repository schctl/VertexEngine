#version 450

layout(binding = 0) uniform sampler2D u_Texture;
layout(location = 0) in vec2 v_TexCoord;
layout(location = 0) out vec4 o_Color;

void main()
{
    // o_Color = texture(u_Texture, v_TexCoord);
    o_Color = vec4(1.0, 1.0, 1.0, 1.0);
}