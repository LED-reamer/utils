#version 460
layout (location = 0) in vec2 a_pos;
layout (location = 1) in vec2 a_uv;
layout (location = 2) in vec4 a_color;

layout (location = 0) out vec2 v_uv;
layout (location = 1) out vec4 v_color;
layout(set = 1, binding = 0) uniform UBO_vert {
    mat4 u_proj;
};
void main()
{
   gl_Position = u_proj * vec4(a_pos.x, a_pos.y, 0.0, 1.0);
   v_uv = a_uv;
   v_color = a_color;
}
