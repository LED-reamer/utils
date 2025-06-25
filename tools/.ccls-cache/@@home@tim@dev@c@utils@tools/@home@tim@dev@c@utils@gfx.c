#include "gfx.h"
#include "gl_wrapper.h"
#include "logging.h"
#include "window.h"
#include "allocator.h"

#include <GL/glew.h>
#include <string.h>

#define PRIMITIVE_CIRCLE_SEGMENTS 32
#define PRIMITIVE_MAX_VERTEX_COUNT 10000

#define TEXTURE_MAX_VERTEX_COUNT 1000 * 6 //1000 rectangles
#define TEXTURE_MAX_TEXTURE_COUNT 64
#define TEXTURE_MAX_TEXTURE_CHUNK_SIZE 32 //embedded into shader source

#define TEXT_MAX_VERTEX_COUNT 1000 * 6 //1000 rectangles
#define TEXT_MAX_TEXTURE_COUNT 64
#define TEXT_MAX_TEXTURE_CHUNK_SIZE 32 //embedded into shader source

#define XSTR(s) STR(s)
#define STR(s) #s

typedef struct
{
	float position[3];
	color_t color;
	float uv[2];
	float texture_index;
	float source_rect[4];
}gfx_texture_vertex;


//static memory PRIMITIVE RENDERING
uint8_t primitive_rendering_initialized = 0;
gfx_primitive_vertex primitive_vertices[PRIMITIVE_MAX_VERTEX_COUNT];
size_t primitive_vertex_count = 0;
shader_t primitive_shader;
mesh_t* primitive_mesh;
const char* primitive_shader_vert = "#version 330 core\n" "layout (location = 0) in vec2 a_pos;\n" "layout (location = 1) in vec4 a_color;\n" "out vec4 v_color;\n" "uniform mat4 u_proj;\n" "void main()\n" "{\n" "   v_color = a_color;\n" "   gl_Position = u_proj * vec4(a_pos.x, a_pos.y, 0.0, 1.0);\n" "}\0";
const char* primitive_shader_frag = "#version 330 core\n" "out vec4 FragColor;\n" "in vec4 v_color;\n" "void main()\n" "{\n" "   FragColor = v_color;\n" "}\0";
//

//static memory TEXTURE RENDERING
uint8_t texture_rendering_initialized = 0;
gfx_texture_vertex texture_vertices[TEXTURE_MAX_VERTEX_COUNT];
size_t texture_vertex_count = 0;
shader_t texture_shader;
mesh_t* texture_mesh;
const char* texture_shader_vert = "#version 400\nlayout (location = 0) in vec3 a_pos;layout (location = 1) in vec4 a_color;layout (location = 2) in vec2 a_uv;layout (location = 3) in float a_texture_index;layout (location = 4) in vec4 a_source_rect;out vec4 v_color;out vec2 v_uv;out float v_texture_index;out vec4 v_src_rect;uniform mat4 u_mvp;void main(){v_color = a_color;v_uv = a_uv;v_texture_index = a_texture_index;v_src_rect = a_source_rect;gl_Position = u_mvp * vec4(a_pos, 1.0);}";
const char* texture_shader_frag = "#version 400\nout vec4 FragColor;in vec4 v_color;in vec2 v_uv;in float v_texture_index;in vec4 v_src_rect;uniform sampler2D u_textures[" XSTR(TEXTURE_MAX_TEXTURE_CHUNK_SIZE) "];void main(){vec2 adjusted_uv = vec2(v_src_rect.x + (v_src_rect.z * v_uv.x), v_src_rect.y + (v_src_rect.w * v_uv.y));int new_index = int(v_texture_index);FragColor = texture(u_textures[new_index], adjusted_uv) * v_color;}";
uint32_t texture_rendering_textures[TEXTURE_MAX_TEXTURE_COUNT];
size_t texture_rendering_textures_count = 0;
//

//static memory TEXT RENDERING
uint8_t text_rendering_initialized = 0;
gfx_texture_vertex text_vertices[TEXT_MAX_VERTEX_COUNT];
size_t text_vertex_count = 0;
shader_t text_shader;
mesh_t* text_mesh;
const char* text_shader_vert = "#version 400\nlayout (location = 0) in vec3 a_pos;layout (location = 1) in vec4 a_color;layout (location = 2) in vec2 a_uv;layout (location = 3) in float a_texture_index;layout (location = 4) in vec4 a_source_rect;out vec4 v_color;out vec2 v_uv;out float v_texture_index;out vec4 v_src_rect;uniform mat4 u_mvp;void main(){v_color = a_color;v_uv = a_uv;v_texture_index = a_texture_index;v_src_rect = a_source_rect;gl_Position = u_mvp * vec4(a_pos, 1.0);}";
const char* text_shader_frag = "#version 400\nout vec4 FragColor;in vec4 v_color;in vec2 v_uv;in float v_texture_index;in vec4 v_src_rect;uniform sampler2D u_textures[" XSTR(TEXT_MAX_TEXTURE_CHUNK_SIZE) "];void main(){vec2 adjusted_uv = vec2(v_src_rect.x + (v_src_rect.z * v_uv.x), v_src_rect.y + (v_src_rect.w * v_uv.y));int new_index = int(v_texture_index);vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_textures[new_index], adjusted_uv).r); FragColor = vec4(v_color.rgb, 1) * sampled;}";
uint32_t text_rendering_textures[TEXT_MAX_TEXTURE_COUNT];
size_t text_rendering_textures_count = 0;
//



void gfx_clear(color_t clear_color)
{
	glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
	glClear(GL_COLOR_BUFFER_BIT);
}

void gfx_clear_depth()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

void gfx_clear_stencil()
{
	glClear(GL_STENCIL_BUFFER_BIT);
}



void __init_primitive_rendering()
{
	//init buffers and shaders
	primitive_shader = shader_create(allocator_get_default(), primitive_shader_vert, primitive_shader_frag, NULL, NULL, NULL, NULL);
	primitive_mesh = mesh_create(allocator_get_default());
	vertex_attribute_e attribs[2] = {VERTEX_ATTRIB_FLOAT2/*position*/, VERTEX_ATTRIB_FLOAT4/*color*/};
	mesh_set_attributes(primitive_mesh, attribs, 2);
	primitive_rendering_initialized = 1;
}

void gfx_draw_triangle(vec2 left, vec2 top, vec2 right, color_t color)
{
	gfx_primitive_vertex vertices[3] = {
		{{left[0], left[1]}, {color[0], color[1], color[2], color[3]}},		//left
		{{right[0], right[1]}, {color[0], color[1], color[2], color[3]}},	//right
		{{top[0], top[1]}, {color[0], color[1], color[2], color[3]}},		//top
	};
		
	gfx_draw_triangles(vertices, 3);
}

void gfx_draw_triangles(gfx_primitive_vertex* vertices, size_t num_vertices)
{
	if(primitive_rendering_initialized != 1)  __init_primitive_rendering();
	
	if(primitive_vertex_count + num_vertices > PRIMITIVE_MAX_VERTEX_COUNT)
	{
		ERROR("Primitive vertex buffer too small! increase PRIMITIVE_MAX_VERTEX_COUNT");
		return;
	}
	memcpy(primitive_vertices + primitive_vertex_count, vertices, num_vertices * sizeof(gfx_primitive_vertex));
	primitive_vertex_count += num_vertices;
}

void gfx_draw_circle(vec2 pos, float radius, color_t color)
{
	const uint32_t segments = PRIMITIVE_CIRCLE_SEGMENTS;
	const float dRad = 2*M_PI/segments;

	for	(uint32_t i = 0; i < segments; i++)
	{	
		float rads = dRad*i;
		float rads2 = dRad*(i+1);
	
		vec2 a, b;
		glm_vec2_add(pos, (vec2){cos(rads) * radius, sin(rads) * radius}, a);
		glm_vec2_add(pos, (vec2){cos(rads2) * radius, sin(rads2) * radius}, b);
		
		gfx_draw_triangle(pos, a, b, color);
	}
}

void gfx_draw_rectangle_from_points(vec2 tl, vec2 tr, vec2 ll, vec2 lr, color_t color)
{
	gfx_primitive_vertex vertices[6] = {
		{{tl[0], tl[1]}, {color[0], color[1], color[2], color[3]}},//tl
		{{lr[0], lr[1]}, {color[0], color[1], color[2], color[3]}},//lr
		{{tr[0], tr[1]}, {color[0], color[1], color[2], color[3]}},//tr
		{{tl[0], tl[1]}, {color[0], color[1], color[2], color[3]}},//tl
		{{ll[0], ll[1]}, {color[0], color[1], color[2], color[3]}},//ll
		{{lr[0], lr[1]}, {color[0], color[1], color[2], color[3]}},//lr
	};
	
	gfx_draw_triangles(vertices, 6);
}

void gfx_draw_rectangle(vec2 pos, vec2 size, color_t color)
{
	gfx_draw_rectangle_from_points((vec2){pos[0], pos[1]}, (vec2){pos[0] + size[0], pos[1]}, (vec2){pos[0], pos[1] + size[1]}, (vec2){pos[0] + size[0], pos[1] + size[1]}, color);
}

void gfx_draw_rectangle_rounded(vec2 pos, vec2 size, float radius, color_t color)
{
	//corner circles
	gfx_draw_circle((vec2){pos[0] + radius, pos[1] + radius}, radius, color);
	gfx_draw_circle((vec2){pos[0] + size[0] - radius, pos[1] + radius}, radius, color);
	gfx_draw_circle((vec2){pos[0] + radius, pos[1] + size[1] - radius}, radius, color);
	gfx_draw_circle((vec2){pos[0] + size[0] - radius, pos[1] + size[1] - radius}, radius, color);

	//two body rectangles
	gfx_draw_rectangle((vec2){pos[0] + radius, pos[1]}, (vec2){size[0] - 2 * radius, size[1]}, color);
	gfx_draw_rectangle((vec2){pos[0], pos[1] + radius}, (vec2){size[0], size[1] - 2 * radius}, color);
}

void gfx_draw_line(vec2 pos1, vec2 pos2, float thickness, color_t color)
{
	float d = sqrt((pos2[0] - pos1[0]) * (pos2[0] - pos1[0]) + (pos2[1] - pos1[1]) * (pos2[1] - pos1[1]));
	vec2 shift = {-thickness * (pos2[1] - pos1[1]) / (d * 2.0f), thickness * (pos2[0] - pos1[0]) / (d * 2.0f)};

	vec2 _a, _b, _c, _d;
	glm_vec2_add(pos1, shift, _a);
	glm_vec2_sub(pos1, shift, _b);
	glm_vec2_add(pos2, shift, _c);
	glm_vec2_sub(pos2, shift, _d);
	
	gfx_draw_rectangle_from_points(_a, _b, _c, _d, color);
}

void gfx_draw_lines(vec2* positions, size_t num_positions, float thickness, color_t color)
{
	if(num_positions < 2) return;
	
	for	(size_t i = 1; i < num_positions; i++)
	{
		gfx_draw_line(positions[i-1], positions[i], thickness, color);
	}
}

void __init_texture_rendering()
{
	//init buffers and shaders
	texture_shader = shader_create(allocator_get_default(), texture_shader_vert, texture_shader_frag, NULL, NULL, NULL, NULL);
	texture_mesh = mesh_create(allocator_get_default());
	vertex_attribute_e attribs[5] = {
		VERTEX_ATTRIB_FLOAT3, /*position*/
		VERTEX_ATTRIB_FLOAT4, /*color*/
		VERTEX_ATTRIB_FLOAT2, /*uv*/
		VERTEX_ATTRIB_FLOAT1, /*texture index*/
		VERTEX_ATTRIB_FLOAT4, /*source rect*/
		};
	mesh_set_attributes(texture_mesh, attribs, 5);
	texture_rendering_initialized = 1;
}

void gfx_draw_texture_from_points(uint32_t gl_texture, vec3 tl, vec3 tr, vec3 ll, vec3 lr, color_t color, vec2 source_size, vec4 source_rectangle)
{
	if(texture_rendering_initialized != 1) __init_texture_rendering();

	if(texture_vertex_count + 6 > TEXTURE_MAX_VERTEX_COUNT)
	{
		ERROR("Texture vertex buffer too small! increase TEXTURE_MAX_VERTEX_COUNT");
		return;
	}

	if(texture_rendering_textures_count + 1 > TEXTURE_MAX_TEXTURE_COUNT)
	{
		ERROR("Texture array too small! increase TEXTURE_MAX_TEXTURE_COUNT");
		return;
	}
	
	uint32_t cached_texture_index = 0;
	{
		int64_t found_index = -1;
		for (size_t i = 0; i < texture_rendering_textures_count; i++)
		{
			if(texture_rendering_textures[i] == gl_texture)
				found_index = i;
		}
		if(found_index == -1)
		{
			texture_rendering_textures[texture_rendering_textures_count] = gl_texture;
			cached_texture_index = texture_rendering_textures_count;
			texture_rendering_textures_count++;
		}
		else
		{
			cached_texture_index = found_index;
		}
	}

	if(source_rectangle[0] == -1 || source_rectangle[1] == -1 || source_rectangle[2] == -1 || source_rectangle[3] == -1)
	{
		source_rectangle[0] = 0;
		source_rectangle[1] = 0;
		source_rectangle[2] = 1;
		source_rectangle[3] = 1;
	}
	else if(source_size[0] != -1 && source_size[1] != -1)
	{
		//converting pixel sizes to relative sizes
		source_rectangle[0] /= source_size[0];
		source_rectangle[1] /= source_size[1];
		source_rectangle[2] /= source_size[0];
		source_rectangle[3] /= source_size[1];

		//from top left oriented coordinates to bottom left oriented uvs
		source_rectangle[1] = 1 - source_rectangle[1] - source_rectangle[3];
	}
	
	gfx_texture_vertex vertices[6] = {
		{{tr[0], tr[1], tr[2]}, {color[0], color[1], color[2], color[3]}, {1.0f, 1.0f}, (float)cached_texture_index, {source_rectangle[0], source_rectangle[1], source_rectangle[2], source_rectangle[3]}},//tr
		{{lr[0], lr[1], lr[2]}, {color[0], color[1], color[2], color[3]}, {1.0f, 0.0f}, (float)cached_texture_index, {source_rectangle[0], source_rectangle[1], source_rectangle[2], source_rectangle[3]}},//lr
		{{tl[0], tl[1], tl[2]}, {color[0], color[1], color[2], color[3]}, {0.0f, 1.0f}, (float)cached_texture_index, {source_rectangle[0], source_rectangle[1], source_rectangle[2], source_rectangle[3]}},//tl
		{{lr[0], lr[1], lr[2]}, {color[0], color[1], color[2], color[3]}, {1.0f, 0.0f}, (float)cached_texture_index, {source_rectangle[0], source_rectangle[1], source_rectangle[2], source_rectangle[3]}},//lr
		{{ll[0], ll[1], ll[2]}, {color[0], color[1], color[2], color[3]}, {0.0f, 0.0f}, (float)cached_texture_index, {source_rectangle[0], source_rectangle[1], source_rectangle[2], source_rectangle[3]}},//ll
		{{tl[0], tl[1], tl[2]}, {color[0], color[1], color[2], color[3]}, {0.0f, 1.0f}, (float)cached_texture_index, {source_rectangle[0], source_rectangle[1], source_rectangle[2], source_rectangle[3]}},//tl
	};
	
	memcpy(texture_vertices + texture_vertex_count, vertices, 6 * sizeof(gfx_texture_vertex));
	texture_vertex_count += 6;
}

void gfx_draw_texture(uint32_t gl_texture, vec2 pos, vec2 size)
{
	gfx_draw_texture_from_points(gl_texture, 
								(vec3){pos[0], pos[1], 0}, 
								(vec3){pos[0] + size[0], pos[1], 0}, 
								(vec3){pos[0], pos[1] + size[1], 0}, 
								(vec3){pos[0] + size[0], pos[1] + size[1], 0}, 
								(color_t){1, 1, 1, 1}, (vec2){-1, -1}, (vec4){-1, -1, -1, -1});
}

void gfx_draw_texture_sprite(uint32_t gl_texture, vec2 pos, vec2 size, color_t color, vec2 source_size, vec4 source_rectangle)
{
	gfx_draw_texture_from_points(gl_texture, 
								(vec3){pos[0], pos[1], 0}, 
								(vec3){pos[0] + size[0], pos[1], 0}, 
								(vec3){pos[0], pos[1] + size[1], 0}, 
								(vec3){pos[0] + size[0], pos[1] + size[1], 0}, 
								color, source_size, source_rectangle);
}

void __init_text_rendering()
{
	//init buffers and shaders
	text_shader = shader_create(allocator_get_default(), text_shader_vert, text_shader_frag, NULL, NULL, NULL, NULL);
	text_mesh = mesh_create(allocator_get_default());
	vertex_attribute_e attribs[5] = {
		VERTEX_ATTRIB_FLOAT3, /*position*/
		VERTEX_ATTRIB_FLOAT4, /*color*/
		VERTEX_ATTRIB_FLOAT2, /*uv*/
		VERTEX_ATTRIB_FLOAT1, /*texture index*/
		VERTEX_ATTRIB_FLOAT4, /*source rect*/
		};
	mesh_set_attributes(text_mesh, attribs, 5);
	text_rendering_initialized = 1;
}

void gfx_draw_character(char character, font_t* font, vec3 tl, vec3 tr, vec3 ll, vec3 lr, color_t color)
{
	if(text_rendering_initialized != 1) __init_text_rendering();

	if(text_vertex_count + 6 > TEXT_MAX_VERTEX_COUNT)
	{
		ERROR("Text vertex buffer too small! increase TEXT_MAX_VERTEX_COUNT");
		return;
	}
	
	if(text_rendering_textures_count + 1 > TEXT_MAX_TEXTURE_COUNT)
	{
		ERROR("Text array too small! increase TEXT_MAX_TEXTURE_COUNT");
		return;
	}
	uint32_t cached_texture_index = 0;
	{
		int64_t found_index = -1;
		for (size_t i = 0; i < text_rendering_textures_count; i++)
		{
			if(text_rendering_textures[i] == font->atlas->gl_texture)
				found_index = i;
		}
		if(found_index == -1)
		{
			text_rendering_textures[texture_rendering_textures_count] = font->atlas->gl_texture;
			cached_texture_index = text_rendering_textures_count;
			text_rendering_textures_count++;
		}
		else
		{
			cached_texture_index = found_index;
		}
	}

	int32_t isource_rectangle[4];
	font_get_character_on_atlas(font, character, isource_rectangle);

	vec4 source_rectangle;
	//converting pixel sizes to relative sizes
	source_rectangle[0] = (float)isource_rectangle[0] / font->atlas->width;
	source_rectangle[1] = (float)isource_rectangle[1] / font->atlas->height;
	source_rectangle[2] = (float)(isource_rectangle[2] - isource_rectangle[0]) / font->atlas->width;
	source_rectangle[3] = (float)(isource_rectangle[3] - isource_rectangle[1]) / font->atlas->height;
	
	gfx_texture_vertex vertices[6] = {
		{{tl[0], tl[1], tl[2]}, {color[0], color[1], color[2], color[3]}, {0.0f, 0.0f}, (float)cached_texture_index, {source_rectangle[0], source_rectangle[1], source_rectangle[2], source_rectangle[3]}},//tl
		{{lr[0], lr[1], lr[2]}, {color[0], color[1], color[2], color[3]}, {1.0f, 1.0f}, (float)cached_texture_index, {source_rectangle[0], source_rectangle[1], source_rectangle[2], source_rectangle[3]}},//lr
		{{tr[0], tr[1], tr[2]}, {color[0], color[1], color[2], color[3]}, {1.0f, 0.0f}, (float)cached_texture_index, {source_rectangle[0], source_rectangle[1], source_rectangle[2], source_rectangle[3]}},//tr
		{{tl[0], tl[1], tl[2]}, {color[0], color[1], color[2], color[3]}, {0.0f, 0.0f}, (float)cached_texture_index, {source_rectangle[0], source_rectangle[1], source_rectangle[2], source_rectangle[3]}},//tl
		{{ll[0], ll[1], ll[2]}, {color[0], color[1], color[2], color[3]}, {0.0f, 1.0f}, (float)cached_texture_index, {source_rectangle[0], source_rectangle[1], source_rectangle[2], source_rectangle[3]}},//ll
		{{lr[0], lr[1], lr[2]}, {color[0], color[1], color[2], color[3]}, {1.0f, 1.0f}, (float)cached_texture_index, {source_rectangle[0], source_rectangle[1], source_rectangle[2], source_rectangle[3]}},//lr
	};
	
	memcpy(text_vertices + text_vertex_count, vertices, 6 * sizeof(gfx_texture_vertex));
	text_vertex_count += 6;
}

void gfx_draw_string(const char* string, font_t* font, vec2 pos, color_t color)
{
	char c;
	size_t index = 0;
	int32_t x_offset = 0;
	while(1)
	{
		c = string[index++];
		if (c == '\0') break;
		if (c == '\n')
		{
			gfx_draw_string((const char*)&string[index], font, (vec2){pos[0], pos[1] + font->line_height}, color);
			break;
		}

		int32_t char_size[2];
		font_get_character_size(font, c, char_size);
		gfx_draw_character(c, font, 
			(vec3){pos[0] + x_offset, pos[1], 0}, 
			(vec3){pos[0] + x_offset + char_size[0], pos[1], 0}, 
			(vec3){pos[0] + x_offset, pos[1] + char_size[1], 0}, 
			(vec3){pos[0] + x_offset + char_size[0], pos[1] + char_size[1], 0}, 
			color);
		x_offset += char_size[0];
	}
}

void gfx_render(vec2 screen_size)
{
	mat4 proj;
	glm_ortho(0.0f, (float)screen_size[0], (float)screen_size[1], 0.0f, -1, 1, proj);

	gl_render_state_reset();
	gl_state()->caps[GL_CAP_BLEND] = 1;
	//gl_state()->caps[GL_CAP_CULL_FACE] = 1;
	//gl_state()->caps[GL_CAP_MULTISAMPLE] = 1;
	gl_state()->blend_func_alpha_src = GL_BLEND_FUNC_ONE_MINUS_SRC_ALPHA;
	//gl_state()->caps[GL_POLYGON_SMOOTH] = 1;
	//gl_state()->hints[GL_HINT_POLYGON_SMOOTH] = 1;
	gl_render_state_apply();
	
	
	if(texture_vertex_count != 0)
	{
		mesh_set_data_vertices(texture_mesh, (void*)texture_vertices, texture_vertex_count);
		
		shader_bind(texture_shader);
		shader_uniform_mat4(texture_shader, "u_mvp", proj);

		int uniform_location = glGetUniformLocation(texture_shader, "u_textures");
		int current_unit = 0;
		for	(size_t i = 0; i < texture_rendering_textures_count; i++)
		{
			glActiveTexture(GL_TEXTURE0 + current_unit);
			glBindTexture(GL_TEXTURE_2D, texture_rendering_textures[i]);
			
			glUniform1i(uniform_location + current_unit, current_unit);
			current_unit++;
			if (current_unit >= TEXTURE_MAX_TEXTURE_CHUNK_SIZE)
			{
				ERROR("IMPLEMENT texture chunking correctly in gfx_render()!");
				break;
			}
		}
		
		mesh_draw(texture_mesh, 0, DRAW_MODE_TRIANGLES, 0);
		texture_vertex_count = 0;
		texture_rendering_textures_count = 0;
	}
	
	if(primitive_vertex_count != 0)
	{
		mesh_set_data_vertices(primitive_mesh, (void*)primitive_vertices, primitive_vertex_count);
		
		shader_bind(primitive_shader);
		shader_uniform_mat4(primitive_shader, "u_proj", proj);
		mesh_draw(primitive_mesh, 0, DRAW_MODE_TRIANGLES, 0);
		
		primitive_vertex_count = 0;
	}

	if(text_vertex_count != 0)
	{
		mesh_set_data_vertices(text_mesh, (void*)text_vertices, text_vertex_count);
		
		shader_bind(text_shader);
		shader_uniform_mat4(text_shader, "u_mvp", proj);

		int uniform_location = glGetUniformLocation(text_shader, "u_textures");
		int current_unit = 0;
		for	(size_t i = 0; i < text_rendering_textures_count; i++)
		{
			glActiveTexture(GL_TEXTURE0 + current_unit);
			glBindTexture(GL_TEXTURE_2D, text_rendering_textures[i]);
			
			glUniform1i(uniform_location + current_unit, current_unit);
			current_unit++;
			if (current_unit >= TEXTURE_MAX_TEXTURE_CHUNK_SIZE)
			{
				ERROR("IMPLEMENT texture chunking correctly in gfx_render()!");
				break;
			}
		}
		
		mesh_draw(text_mesh, 0, DRAW_MODE_TRIANGLES, 0);
		
		
		text_vertex_count = 0;
		text_rendering_textures_count = 0;
	}
	gl_render_state_reset();
}

void gfx_destroy()
{
	if(primitive_rendering_initialized)
	{
		primitive_vertex_count = 0;
		shader_destroy(primitive_shader);
		mesh_destroy(primitive_mesh);
		primitive_rendering_initialized = 0;
	}

	if(texture_rendering_initialized)
	{
		texture_vertex_count = 0;
		shader_destroy(texture_shader);
		mesh_destroy(texture_mesh);
		texture_rendering_initialized = 0;
	}

	if(text_rendering_initialized)
	{
		text_vertex_count = 0;
		shader_destroy(text_shader);
		mesh_destroy(text_mesh);
		text_rendering_initialized = 0;
	}
}
