#version 460
layout (location = 0) in vec2 v_uv;
layout (location = 1) in vec4 v_color;
layout (location = 0) out vec4 FragColor;
layout(set = 2, binding = 0) uniform sampler2D u_font_atlas;
layout(set = 3, binding = 0) uniform UBO_frag {
	float u_smoothing;
};
void main(){
   float dist = texture(u_font_atlas, v_uv).r;
   float alpha = smoothstep(0.5 - u_smoothing, 0.5 + u_smoothing, dist);
   alpha += 0.1;
   FragColor = vec4(v_color.rgb, v_color.a * alpha);
}
