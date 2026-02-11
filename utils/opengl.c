#include "opengl.h"
#include "logging.h"

#include <GL/glew.h>
#include <stdbool.h>
#include <string.h>

//macros
#define _OPENGL_DEBUG
#ifdef _OPENGL_DEBUG
char const* gl_error_string(GLenum const err){switch (err) {case GL_NO_ERROR:return "GL_NO_ERROR";case GL_INVALID_ENUM:return "GL_INVALID_ENUM";case GL_INVALID_VALUE:return "GL_INVALID_VALUE";case GL_INVALID_OPERATION:return "GL_INVALID_OPERATION";case GL_STACK_OVERFLOW:return "GL_STACK_OVERFLOW";case GL_STACK_UNDERFLOW:return "GL_STACK_UNDERFLOW";case GL_OUT_OF_MEMORY:return "GL_OUT_OF_MEMORY";case GL_TABLE_TOO_LARGE:return "GL_TABLE_TOO_LARGE";case GL_CONTEXT_LOST:return "GL_CONTEXT_LOST";case GL_INVALID_FRAMEBUFFER_OPERATION:return "GL_INVALID_FRAMEBUFFER_OPERATION";default:return NULL;}}
#define GL_CALL(_CALL) do { _CALL; GLenum gl_err = glGetError(); if (gl_err != 0) ERROR("GL error 0x%x %s in %s at %s:%d: returned from '%s'.\n", gl_err, gl_error_string(gl_err), __func__, __FILE__, __LINE__, #_CALL); } while (0)  // Call with error check
#else
#define GL_CALL(_CALL) _CALL   // Call without error check
#endif

void gl_init(){
	if(glewInit() != GLEW_OK) ERROR("Could not init glew");
}

void gl_viewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h){
	glViewport(x, y, w, h);
}

void gl_set_state(gl_state_e state, bool active) {
    switch (state) {
        case GL_STATE_BLENDING:
            if (active) {
                GL_CALL(glEnable(GL_BLEND));
                GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
            } else {
                GL_CALL(glDisable(GL_BLEND));
                GL_CALL(glBlendFunc(GL_ONE, GL_ZERO));
            }
            break;
        case GL_STATE_DEPTH_TEST:
            if (active) {
                GL_CALL(glEnable(GL_DEPTH_TEST));
                GL_CALL(glDepthFunc(GL_LESS));
            } else {
                GL_CALL(glDisable(GL_DEPTH_TEST));
                GL_CALL(glDepthFunc(GL_LESS));
            }
            break;
        case GL_STATE_DEPTH_WRITING:
        	if (active){
        		GL_CALL(glDepthMask(GL_TRUE));
        	} else {
        		GL_CALL(glDepthMask(GL_FALSE));
        	}
        	break;
        case GL_STATE_CULLING:
            if (active) {
                GL_CALL(glEnable(GL_CULL_FACE));
                GL_CALL(glCullFace(GL_BACK));
                GL_CALL(glFrontFace(GL_CCW));
            } else {
                GL_CALL(glDisable(GL_CULL_FACE));
                GL_CALL(glCullFace(GL_BACK));
                GL_CALL(glFrontFace(GL_CCW));
            }
            break;
        default:
            ERROR("unknown gl_state change!");
            break;
    }
}

void gl_clear(color_t color) {
	GL_CALL(glClearColor(color.r, color.g, color.b, color.a));
	GL_CALL(glClearDepth(1.0f));
	GL_CALL(glClearStencil(0));
	GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
}

void gl_clear_color(color_t color) {
	GL_CALL(glClearColor(color.r, color.g, color.b, color.a));
	GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
}

void gl_clear_depth() {
	GL_CALL(glClearDepth(1.0f));
	GL_CALL(glClear(GL_DEPTH_BUFFER_BIT));
}

void gl_clear_stencil() {
	GL_CALL(glClearStencil(0));
	GL_CALL(glClear(GL_STENCIL_BUFFER_BIT));
}

shader_t shader_create(allocator_t* allocator, const char* vertex, const char* fragment, const char* geometry, const char* tess_control, const char* tess_eval, const char* compute) {
	#define SHADER_VERTEX 0
	#define SHADER_FRAGMENT 1
	#define SHADER_GEOMETRY 2
	#define SHADER_TESS_CONTROL 3
	#define SHADER_TESS_EVAL 4
	#define SHADER_COMPUTE 5


    uint32_t shader_objects[6] = { 0 };

    // Vertex Shader
    if (vertex != NULL && strlen(vertex) > 0)
    {
    	uint32_t vertex_shader_object;
        GL_CALL(vertex_shader_object = glCreateShader(GL_VERTEX_SHADER));
        GL_CALL(glShaderSource(vertex_shader_object, 1, (const char * const*)&vertex, NULL));
        GL_CALL(glCompileShader(vertex_shader_object));

        int32_t compile_status;
        GL_CALL(glGetShaderiv(vertex_shader_object, GL_COMPILE_STATUS, &compile_status));

        if (compile_status == GL_FALSE)
        {
            int32_t info_log_length;
            GL_CALL(glGetShaderiv(vertex_shader_object, GL_INFO_LOG_LENGTH, &info_log_length));

            uint8_t* info_log = (uint8_t*)allocator->amalloc(info_log_length);
            GL_CALL(glGetShaderInfoLog(vertex_shader_object, info_log_length, NULL, (char*)info_log));

            ERROR("Vertex shader compilation failed: %s", info_log);

            GL_CALL(glDeleteShader(vertex_shader_object));
            allocator->afree(info_log);

            return 0;
        }
        shader_objects[SHADER_VERTEX] = vertex_shader_object;
    }
    // Fragment Shader
    if (fragment != NULL && strlen(fragment) > 0)
    {
        uint32_t fragment_shader_object;
        GL_CALL(fragment_shader_object = glCreateShader(GL_FRAGMENT_SHADER));
        GL_CALL(glShaderSource(fragment_shader_object, 1, (const char * const*)&fragment, NULL));
        GL_CALL(glCompileShader(fragment_shader_object));

        int32_t compile_status;
        GL_CALL(glGetShaderiv(fragment_shader_object, GL_COMPILE_STATUS, &compile_status));

        if (compile_status == GL_FALSE)
        {
            int32_t info_log_length;
            GL_CALL(glGetShaderiv(fragment_shader_object, GL_INFO_LOG_LENGTH, &info_log_length));

            uint8_t* info_log = (uint8_t*)allocator->amalloc(info_log_length);
            GL_CALL(glGetShaderInfoLog(fragment_shader_object, info_log_length, NULL, (char*)info_log));

            ERROR("Fragment shader compilation failed: %s", info_log);

            GL_CALL(glDeleteShader(fragment_shader_object));
            allocator->afree(info_log);

            return 0;
        }

        shader_objects[SHADER_FRAGMENT] = fragment_shader_object;
    }

    // Geometry Shader
    if (geometry != NULL && strlen(geometry) > 0)
    {
        uint32_t geometry_shader_object;
        GL_CALL(geometry_shader_object = glCreateShader(GL_GEOMETRY_SHADER));
        GL_CALL(glShaderSource(geometry_shader_object, 1, (const char * const*)&geometry, NULL));
        GL_CALL(glCompileShader(geometry_shader_object));

        int32_t compile_status;
        GL_CALL(glGetShaderiv(geometry_shader_object, GL_COMPILE_STATUS, &compile_status));

        if (compile_status == GL_FALSE)
        {
            int32_t info_log_length;
            GL_CALL(glGetShaderiv(geometry_shader_object, GL_INFO_LOG_LENGTH, &info_log_length));

            uint8_t* info_log = (uint8_t*)allocator->amalloc(info_log_length);
            GL_CALL(glGetShaderInfoLog(geometry_shader_object, info_log_length, NULL, (char*)info_log));

            ERROR("Geometry shader compilation failed: %s", info_log);

            GL_CALL(glDeleteShader(geometry_shader_object));
            allocator->afree(info_log);

            return 0;
        }

        shader_objects[SHADER_GEOMETRY] = geometry_shader_object;
    }

    // Tessellation Control Shader
    if (tess_control != NULL && strlen(tess_control) > 0)
    {
        uint32_t tess_control_shader_object;
        GL_CALL(tess_control_shader_object = glCreateShader(GL_TESS_CONTROL_SHADER));
        GL_CALL(glShaderSource(tess_control_shader_object, 1, (const char * const*)&tess_control, NULL));
        GL_CALL(glCompileShader(tess_control_shader_object));

        int32_t compile_status;
        GL_CALL(glGetShaderiv(tess_control_shader_object, GL_COMPILE_STATUS, &compile_status));

        if (compile_status == GL_FALSE)
        {
            int32_t info_log_length;
            GL_CALL(glGetShaderiv(tess_control_shader_object, GL_INFO_LOG_LENGTH, &info_log_length));

            uint8_t* info_log = (uint8_t*)allocator->amalloc(info_log_length);
            GL_CALL(glGetShaderInfoLog(tess_control_shader_object, info_log_length, NULL, (char*)info_log));

            ERROR("Tessellation Control shader compilation failed: %s", info_log);

            GL_CALL(glDeleteShader(tess_control_shader_object));
            allocator->afree(info_log);

            return 0;
        }

        shader_objects[SHADER_TESS_CONTROL] = tess_control_shader_object;
    }

    // Tessellation Evaluation Shader
    if (tess_eval != NULL && strlen(tess_eval) > 0)
    {
        uint32_t tess_eval_shader_object;
        GL_CALL(tess_eval_shader_object = glCreateShader(GL_TESS_EVALUATION_SHADER));
        GL_CALL(glShaderSource(tess_eval_shader_object, 1, (const char * const*)&tess_eval, NULL));
        GL_CALL(glCompileShader(tess_eval_shader_object));

        int32_t compile_status;
        GL_CALL(glGetShaderiv(tess_eval_shader_object, GL_COMPILE_STATUS, &compile_status));

        if (compile_status == GL_FALSE)
        {
            int32_t info_log_length;
            GL_CALL(glGetShaderiv(tess_eval_shader_object, GL_INFO_LOG_LENGTH, &info_log_length));

            uint8_t* info_log = (uint8_t*)allocator->amalloc(info_log_length);
            GL_CALL(glGetShaderInfoLog(tess_eval_shader_object, info_log_length, NULL, (char*)info_log));

            ERROR("Tessellation Evaluation shader compilation failed: %s", info_log);

            GL_CALL(glDeleteShader(tess_eval_shader_object));
            allocator->afree(info_log);

            return 0;
        }

        shader_objects[SHADER_TESS_EVAL] = tess_eval_shader_object;
    }

    // Compute Shader
    if (compute != NULL && strlen(compute) > 0)
    {
        uint32_t compute_shader_object;
        GL_CALL(compute_shader_object = glCreateShader(GL_COMPUTE_SHADER));
        GL_CALL(glShaderSource(compute_shader_object, 1, (const char * const*)&compute, NULL));
        GL_CALL(glCompileShader(compute_shader_object));

        int32_t compile_status;
        GL_CALL(glGetShaderiv(compute_shader_object, GL_COMPILE_STATUS, &compile_status));

        if (compile_status == GL_FALSE)
        {
            int32_t info_log_length;
            GL_CALL(glGetShaderiv(compute_shader_object, GL_INFO_LOG_LENGTH, &info_log_length));

            uint8_t* info_log = (uint8_t*)allocator->amalloc(info_log_length);
            GL_CALL(glGetShaderInfoLog(compute_shader_object, info_log_length, NULL, (char*)info_log));

            ERROR("Compute shader compilation failed: %s", info_log);

            GL_CALL(glDeleteShader(compute_shader_object));
            allocator->afree(info_log);

            return 0;
        }

        shader_objects[SHADER_COMPUTE] = compute_shader_object;
    }
    uint32_t program_object;
    GL_CALL(program_object = glCreateProgram());

    for (int i = 0; i < 6; i++)
    {
        if (shader_objects[i] != 0)
        {
            GL_CALL(glAttachShader(program_object, shader_objects[i]));
        }
    }

    GL_CALL(glLinkProgram(program_object));

    for (int i = 0; i < 6; i++)
    {
        if (shader_objects[i] != 0)
        {
            GL_CALL(glDetachShader(program_object, shader_objects[i]));
            GL_CALL(glDeleteShader(shader_objects[i]));
        }
    }

    int32_t link_status;
    GL_CALL(glGetProgramiv(program_object, GL_LINK_STATUS, &link_status));

    if (link_status == GL_FALSE)
    {
        int32_t info_log_length;
        GL_CALL(glGetProgramiv(program_object, GL_INFO_LOG_LENGTH, &info_log_length));

        uint8_t* info_log = (uint8_t*)allocator->amalloc(info_log_length);
        GL_CALL(glGetProgramInfoLog(program_object, info_log_length, NULL, (char*)info_log));

        ERROR("Program linking failed: %s", info_log);

        GL_CALL(glDeleteProgram(program_object));

        for (int i = 0; i < 6; i++)
        {
            if (shader_objects[i] != 0)
            {
                GL_CALL(glDeleteShader(shader_objects[i]));
            }
        }

        allocator->afree(info_log);

        return 0;
    }
    
    return program_object;
    
    #undef SHADER_VERTEX
    #undef SHADER_FRAGMENT
    #undef SHADER_GEOMETRY
    #undef SHADER_TESS_CONTROL
    #undef SHADER_TESS_EVAL
    #undef SHADER_COMPUTE
}

void shader_destroy(shader_t shader) {
	GL_CALL(glDeleteProgram(shader));
}

void shader_bind(shader_t shader) {
	GL_CALL(glUseProgram(shader));
}

void shader_uniform_float32(shader_t shader, const char* uniform_name, float value) {
    GL_CALL(glUniform1f(glGetUniformLocation(shader, uniform_name), value));
}

void shader_uniform_int32(shader_t shader, const char* uniform_name, int32_t value) {
    GL_CALL(glUniform1i(glGetUniformLocation(shader, uniform_name), value));
}

void shader_uniform_uint32(shader_t shader, const char* uniform_name, uint32_t value) {
    GL_CALL(glUniform1ui(glGetUniformLocation(shader, uniform_name), value));
}

void shader_uniform_vec2(shader_t shader, const char* uniform_name, vec2_t value) {
    GL_CALL(glUniform2f(glGetUniformLocation(shader, uniform_name), value.x, value.y));
}

void shader_uniform_vec3(shader_t shader, const char* uniform_name, vec3_t value) {
    GL_CALL(glUniform3f(glGetUniformLocation(shader, uniform_name), value.x, value.y, value.z));
}

void shader_uniform_vec4(shader_t shader, const char* uniform_name, vec4_t value) {
    GL_CALL(glUniform4f(glGetUniformLocation(shader, uniform_name), value.x, value.y, value.z, value.w));
}

void shader_uniform_mat4x4(shader_t shader, const char* uniform_name, mat4x4_t value) {
    GL_CALL(glUniformMatrix4fv(glGetUniformLocation(shader, uniform_name), 1, GL_FALSE, (const float*)value.m16));
}

void shader_uniform_float32_array(shader_t shader, const char* uniform_name, float* data_ptr, size_t count) {
	GL_CALL(glUniform1fv(glGetUniformLocation(shader, uniform_name), count, data_ptr));
}

void shader_uniform_int32_array(shader_t shader, const char* uniform_name, int32_t* data_ptr, size_t count) {
	GL_CALL(glUniform1iv(glGetUniformLocation(shader, uniform_name), count, data_ptr));
}

mesh_t mesh_create(allocator_t* allocator) {
	mesh_t mesh = {0};
	mesh.allocator = allocator;

	GL_CALL(glGenVertexArrays(1, &mesh.gl_vertex_array));
	GL_CALL(glGenBuffers(1, &mesh.gl_vertex_buffer));
	GL_CALL(glGenBuffers(1, &mesh.gl_index_buffer));
	
	return mesh;
}

void mesh_destroy(mesh_t* mesh) {
	GL_CALL(glDeleteVertexArrays(1, &mesh->gl_vertex_array));
	GL_CALL(glDeleteBuffers(1, &mesh->gl_vertex_buffer));
	GL_CALL(glDeleteBuffers(1, &mesh->gl_index_buffer));
	
	*mesh = (mesh_t){0};
}

void mesh_set_attributes(mesh_t* mesh, vertex_attribute_e* vertex_attributes, size_t num_attributes) {
    GL_CALL(glBindVertexArray(mesh->gl_vertex_array));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, mesh->gl_vertex_buffer));

    //count vertex layout size
    for (size_t i = 0; i < num_attributes; i++)
    {
        switch (vertex_attributes[i])
        {
        case VERTEX_ATTRIB_FLOAT1:
            mesh->vertex_size += sizeof(float);
            break;
        case VERTEX_ATTRIB_FLOAT2:
            mesh->vertex_size += sizeof(float) * 2;
            break;
        case VERTEX_ATTRIB_FLOAT3:
            mesh->vertex_size += sizeof(float) * 3;
            break;
        case VERTEX_ATTRIB_FLOAT4:
            mesh->vertex_size += sizeof(float) * 4;
            break;
        case VERTEX_ATTRIB_INT:
            mesh->vertex_size += sizeof(int);
            break;
        case VERTEX_ATTRIB_UINT:
            mesh->vertex_size += sizeof(uint32_t);
            break;
        case VERTEX_ATTRIB_FLOAT1_NORM:
            mesh->vertex_size += sizeof(GLbyte); // GLbyte for GL_BYTE normalized
            break;
        case VERTEX_ATTRIB_FLOAT2_NORM:
            mesh->vertex_size += sizeof(GLbyte) * 2;
            break;
        case VERTEX_ATTRIB_FLOAT3_NORM:
            mesh->vertex_size += sizeof(GLbyte) * 3;
            break;
        case VERTEX_ATTRIB_FLOAT4_NORM:
            mesh->vertex_size += sizeof(GLbyte) * 4;
            break;
        case VERTEX_ATTRIB_INT_NORM:
            mesh->vertex_size += sizeof(GLint); // GLint for GL_INT normalized
            break;
        case VERTEX_ATTRIB_UINT_NORM:
            mesh->vertex_size += sizeof(GLuint); // GLuint for GL_UNSIGNED_INT normalized
            break;
        default:
            break;
        }
    }


    //apply vertex layout
    size_t offset = 0;
    for (size_t i = 0; i < num_attributes; i++)
    {
        switch (vertex_attributes[i])
        {
        case VERTEX_ATTRIB_FLOAT1:
            GL_CALL(glEnableVertexAttribArray(i));
            GL_CALL(glVertexAttribPointer(i, 1, GL_FLOAT, GL_FALSE, mesh->vertex_size, (const void*)offset));
            offset += sizeof(float);
            break;
        case VERTEX_ATTRIB_FLOAT2:
            GL_CALL(glEnableVertexAttribArray(i));
            GL_CALL(glVertexAttribPointer(i, 2, GL_FLOAT, GL_FALSE, mesh->vertex_size, (const void*)offset));
            offset += sizeof(float) * 2;
            break;
        case VERTEX_ATTRIB_FLOAT3:
            GL_CALL(glEnableVertexAttribArray(i));
            GL_CALL(glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, mesh->vertex_size, (const void*)offset));
            offset += sizeof(float) * 3;
            break;
        case VERTEX_ATTRIB_FLOAT4:
            GL_CALL(glEnableVertexAttribArray(i));
            GL_CALL(glVertexAttribPointer(i, 4, GL_FLOAT, GL_FALSE, mesh->vertex_size, (const void*)offset));
            offset += sizeof(float) * 4;
            break;
        case VERTEX_ATTRIB_INT:
            GL_CALL(glEnableVertexAttribArray(i));
            GL_CALL(glVertexAttribIPointer(i, 1, GL_INT, mesh->vertex_size, (const void*)offset));
            offset += sizeof(int);
            break;
        case VERTEX_ATTRIB_UINT:
            GL_CALL(glEnableVertexAttribArray(i));
            GL_CALL(glVertexAttribIPointer(i, 1, GL_UNSIGNED_INT, mesh->vertex_size, (const void*)offset));
            offset += sizeof(uint32_t);
            break;
        case VERTEX_ATTRIB_FLOAT1_NORM:
            GL_CALL(glEnableVertexAttribArray(i));
            GL_CALL(glVertexAttribPointer(i, 1, GL_BYTE, GL_TRUE, mesh->vertex_size, (const void*)offset));
            offset += sizeof(GLbyte);
            break;
        case VERTEX_ATTRIB_FLOAT2_NORM:
            GL_CALL(glEnableVertexAttribArray(i));
            GL_CALL(glVertexAttribPointer(i, 2, GL_BYTE, GL_TRUE, mesh->vertex_size, (const void*)offset));
            offset += sizeof(GLbyte) * 2;
            break;
        case VERTEX_ATTRIB_FLOAT3_NORM:
            GL_CALL(glEnableVertexAttribArray(i));
            GL_CALL(glVertexAttribPointer(i, 3, GL_BYTE, GL_TRUE, mesh->vertex_size, (const void*)offset));
            offset += sizeof(GLbyte) * 3;
            break;
        case VERTEX_ATTRIB_FLOAT4_NORM:
            GL_CALL(glEnableVertexAttribArray(i));
            GL_CALL(glVertexAttribPointer(i, 4, GL_BYTE, GL_TRUE, mesh->vertex_size, (const void*)offset));
            offset += sizeof(GLbyte) * 4;
            break;
        case VERTEX_ATTRIB_INT_NORM:
            GL_CALL(glEnableVertexAttribArray(i));
            GL_CALL(glVertexAttribIPointer(i, 1, GL_INT, mesh->vertex_size, (const void*)offset));
            offset += sizeof(GLint);
            break;
        case VERTEX_ATTRIB_UINT_NORM:
            GL_CALL(glEnableVertexAttribArray(i));
            GL_CALL(glVertexAttribIPointer(i, 1, GL_UNSIGNED_INT, mesh->vertex_size, (const void*)offset));
            offset += sizeof(GLuint);
            break;
        default:
            break;
        }
    }

    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL_CALL(glBindVertexArray(0));
}

void mesh_set_data_vertices(mesh_t* mesh, void* vertices, size_t num_vertices) {

    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, mesh->gl_vertex_buffer));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, num_vertices * mesh->vertex_size, vertices, GL_STATIC_DRAW));//GL_STREAM_DRAW, STATIC ???
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));

    mesh->num_vertices = num_vertices;
}

void mesh_set_data_indices(mesh_t* mesh, uint32_t* indices, size_t num_indices) {
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->gl_index_buffer));
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices * sizeof(uint32_t), indices, GL_STATIC_DRAW));//GL_STREAM_DRAW, STATIC ???
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    mesh->num_indices = num_indices;
}

void mesh_draw(mesh_t* mesh, bool indexed, draw_mode_e draw_mode, bool instanced) {
	uint32_t gl_draw_mode = 0;
	switch(draw_mode){
		case DRAW_MODE_POINTS: gl_draw_mode = GL_POINTS; break;
		case DRAW_MODE_LINES: gl_draw_mode = GL_LINES; break;
		case DRAW_MODE_LINE_STRIP: gl_draw_mode = GL_LINE_STRIP; break;
		case DRAW_MODE_LINE_LOOP: gl_draw_mode = GL_LINE_LOOP; break;
		case DRAW_MODE_TRIANGLES: gl_draw_mode = GL_TRIANGLES; break;
		case DRAW_MODE_TRIANGLE_STRIP: gl_draw_mode = GL_TRIANGLE_STRIP; break;
		case DRAW_MODE_TRIANGLE_FAN: gl_draw_mode = GL_TRIANGLE_FAN; break;
		default: ERROR("unknown draw_mode_e in mesh_draw"); gl_draw_mode = GL_TRIANGLES; break;
	}
    if (instanced)
    {
        FATAL_ERROR("TODO: IMPLEMENT instanced drawing");
    }
    else
    {
        if (indexed)
        {
            if (mesh->num_vertices == 0 || mesh->num_indices == 0 || mesh->gl_vertex_array == 0) return;

            GL_CALL(glBindVertexArray(mesh->gl_vertex_array));
            GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->gl_index_buffer));
            GL_CALL(glDrawElements(gl_draw_mode, mesh->num_indices, GL_UNSIGNED_INT, 0));
            GL_CALL(glBindVertexArray(0));
            GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
        }
        else
        {
            if (mesh->num_vertices == 0 || mesh->gl_vertex_array == 0) return;

            GL_CALL(glBindVertexArray(mesh->gl_vertex_array));
            GL_CALL(glDrawArrays(draw_mode, 0, mesh->num_vertices));
            GL_CALL(glBindVertexArray(0));
        }
    }
}

texture_channels_e texture_get_channels_from_num(uint8_t num_channels) {
	switch(num_channels)
	{
		case 1:
			return TEXTURE_CHANNELS_R;
			break;
		case 2:
			return TEXTURE_CHANNELS_RG;
			break;
		case 3:
			return TEXTURE_CHANNELS_RGB;
			break;
		case 4:
			return TEXTURE_CHANNELS_RGBA;
			break;
		default:
			return TEXTURE_CHANNELS_RGBA;
			break;
	}
}
GLenum __texture_get_internal_format(texture_channels_e channel, texture_bits_e bits, texture_format_type_e type) {
    static GLenum formats[TEXTURE_CHANNELS_COUNT][TEXTURE_BITS_COUNT][TEXTURE_FORMAT_TYPE_COUNT] = {
        {   // TEXTURE_CHANNELS_R
            {GL_R8, GL_R16},           // INT
            {GL_R16F, GL_R16F}         // FLOAT
        },
        {   // TEXTURE_CHANNELS_RG
            {GL_RG8, GL_RG16},         // INT
            {GL_RG16F, GL_RG16F}       // FLOAT
        },
        {   // TEXTURE_CHANNELS_RGB
            {GL_RGB8, GL_RGB16},       // INT
            {GL_RGB16F, GL_RGB16F}     // FLOAT
        },
        {   // TEXTURE_CHANNELS_RGBA
            {GL_RGBA8, GL_RGBA16},     // INT
            {GL_RGBA16F, GL_RGBA16F}   // FLOAT
        }
    };
    
    return formats[channel][bits][type];
}

GLenum __texture_get_format(texture_channels_e channels) {
    static GLenum formats[TEXTURE_CHANNELS_COUNT] = {
        GL_RED,    // TEXTURE_CHANNELS_R
        GL_RG,     // TEXTURE_CHANNELS_RG
        GL_RGB,    // TEXTURE_CHANNELS_RGB
        GL_RGBA,   // TEXTURE_CHANNELS_RGBA
    };
    
    return formats[channels];
}

GLenum __texture_get_type(texture_bits_e bits, texture_format_type_e type) {
    static GLenum types[TEXTURE_BITS_COUNT][TEXTURE_FORMAT_TYPE_COUNT] = {
        {   // TEXTURE_BITS_8
            GL_UNSIGNED_BYTE,    // INT
            GL_FLOAT             // FLOAT
        },
        {   // TEXTURE_BITS_16
            GL_UNSIGNED_SHORT,   // INT
            GL_FLOAT             // FLOAT
        }
    };
    
    return types[bits][type];
}
/*GLenum __channels_to_gl[] = {
	GL_RED,
	GL_RG,
	GL_RGB,
	GL_RGBA,
};

GLenum __channels_bits_to_gl_bits[TEXTURE_CHANNELS_COUNT][TEXTURE_BITS_COUNT] = {
	{GL_R8, GL_R16},
	{GL_RG8, GL_RG16},
	{GL_RGB8, GL_RGB16},
	{GL_RGBA8, GL_RGBA16},
};*/

GLenum __filtering_to_gl[] = {
	GL_NEAREST,
	GL_LINEAR,
	GL_NEAREST_MIPMAP_NEAREST,
	GL_LINEAR_MIPMAP_NEAREST,
	GL_NEAREST_MIPMAP_LINEAR,
	GL_LINEAR_MIPMAP_LINEAR,
};

GLenum __wrapping_to_gl[] = {
	GL_CLAMP_TO_EDGE,
	GL_CLAMP_TO_BORDER,
	GL_REPEAT,
	GL_MIRRORED_REPEAT,
};

texture_t texture_create(allocator_t* allocator, uint32_t width, uint32_t height, texture_channels_e channels, texture_bits_e bits_per_channel, texture_format_type_e format_type) {
	texture_t texture = {0};
	texture.allocator = allocator;
	GL_CALL(glGenTextures(1, &texture.gl_texture));
	texture.width = width;
	texture.height = height;
	texture.channels = channels;
	texture.bits_per_channel = bits_per_channel;
	texture.format_type = format_type;

	texture_resize(&texture, width, height);

	texture_filtering(&texture, TEXTURE_FILTERING_NEAREST, TEXTURE_FILTERING_NEAREST);
	texture_wrapping(&texture, TEXTURE_WRAPPING_REPEAT, TEXTURE_WRAPPING_REPEAT);

	return texture;
}

void texture_destroy(texture_t* texture) {
	GL_CALL(glDeleteTextures(1, &texture->gl_texture));
	*texture = (texture_t){0};
}

void texture_bind(texture_t* texture, uint32_t slot) {
	GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, texture->gl_texture));
}

void texture_resize(texture_t* texture, uint32_t width, uint32_t height) {
	texture_set_pixels(texture, width, height, NULL);
}

void texture_set_pixels(texture_t* texture, uint32_t width, uint32_t height, void* pixels) {
	int previous_id;
	GL_CALL(glGetIntegerv(GL_TEXTURE_BINDING_2D, &previous_id));
	texture_bind(texture, 0);
	//need to be unsigned byte / short!
	GL_CALL(glTexImage2D(
		GL_TEXTURE_2D, 0, 
		__texture_get_internal_format(texture->channels, texture->bits_per_channel, texture->format_type), 
		width, height, 0, 
		__texture_get_format(texture->channels), 
		__texture_get_type(texture->bits_per_channel, texture->format_type), 
		pixels)
	);
	texture->width = width;
	texture->height = height;

	glBindTexture(GL_TEXTURE_2D, previous_id);
}

void texture_get_pixels(texture_t* texture, void* pixels) {
	int previous_id;
	GL_CALL(glGetIntegerv(GL_TEXTURE_BINDING_2D, &previous_id));
	texture_bind(texture, 0);
	
	GL_CALL(glGetTexImage(GL_TEXTURE_2D, 0, 
		__texture_get_format(texture->channels), 
		__texture_get_type(texture->bits_per_channel, texture->format_type), 
		pixels)
	);

	GL_CALL(glBindTexture(GL_TEXTURE_2D, previous_id));
}

vec2_t texture_get_size(texture_t* texture) {
	return vec2(texture->width, texture->height);
}

uint32_t texture_get_width(texture_t* texture) {
	return texture->width;
}

uint32_t texture_get_height(texture_t* texture) {
	return texture->height;
}

void texture_filtering(texture_t* texture, texture_filtering_e filtering_magnifying, texture_filtering_e filtering_minifying) {
	int previous_id;
	GL_CALL(glGetIntegerv(GL_TEXTURE_BINDING_2D, &previous_id));
	texture_bind(texture, 0);

	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, __filtering_to_gl[filtering_minifying]));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, __filtering_to_gl[filtering_magnifying]));

	GL_CALL(glBindTexture(GL_TEXTURE_2D, previous_id));
}

void texture_wrapping(texture_t* texture, texture_wrapping_e wrapping_x, texture_wrapping_e wrapping_y) {
	int previous_id;
	GL_CALL(glGetIntegerv(GL_TEXTURE_BINDING_2D, &previous_id));
	texture_bind(texture, 0);

	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, __wrapping_to_gl[wrapping_x]));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, __wrapping_to_gl[wrapping_y]));

	GL_CALL(glBindTexture(GL_TEXTURE_2D, previous_id));
}

uint32_t texture_get_num_channels(texture_t* texture) {
	switch(texture->channels)
	{
		case TEXTURE_CHANNELS_R: return 1; break;
		case TEXTURE_CHANNELS_RG: return 2; break;
		case TEXTURE_CHANNELS_RGB: return 3; break;
		case TEXTURE_CHANNELS_RGBA: return 4; break;
		default: return 4; break;
	}
}

uint8_t texture_get_bits_per_channel(texture_t* texture) {
	return texture->bits_per_channel;
}

rendertarget_t rendertarget_create(allocator_t* allocator, uint32_t width, uint32_t height, texture_channels_e channels, texture_bits_e bits_per_channel, texture_format_type_e format_type) {
	rendertarget_t rendertarget = {0};
	rendertarget.allocator = allocator;
	GL_CALL(glGenFramebuffers(1, &rendertarget.gl_framebuffer));
	rendertarget.texture = texture_create(allocator, width, height, channels, bits_per_channel, format_type);

	GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, rendertarget.gl_framebuffer));
	GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rendertarget.texture.gl_texture, 0));
	GL_CALL(glDrawBuffer(GL_COLOR_ATTACHMENT0));

	uint32_t result;
	GL_CALL(result = glCheckFramebufferStatus(GL_FRAMEBUFFER));
	if (result != GL_FRAMEBUFFER_COMPLETE)
	{
		ERROR("Could not create framebuffer for rendertarget");
	}
	GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	
	return rendertarget;
}

void rendertarget_destroy(rendertarget_t* rendertarget) {
	texture_destroy(&rendertarget->texture);
	GL_CALL(glDeleteFramebuffers(1, &rendertarget->gl_framebuffer));
	*rendertarget = (rendertarget_t){0};
}

void rendertarget_bind(rendertarget_t* rendertarget) {
	GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, rendertarget->gl_framebuffer));
	GL_CALL(glViewport(0, 0, rendertarget->texture.width, rendertarget->texture.height));
}

void rendertarget_unbind(vec2_t new_viewport) {
	GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GL_CALL(glViewport(0, 0, new_viewport.x, new_viewport.y));
}

texture_t* rendertarget_get_texture(rendertarget_t* rendertarget) {
	return &rendertarget->texture;
}

framebuffer_t framebuffer_create(){
	framebuffer_t framebuffer = {0};
	GL_CALL(glGenFramebuffers(1, &framebuffer.gl_framebuffer));
	return framebuffer;
}

void framebuffer_destroy(framebuffer_t* framebuffer){
	GL_CALL(glDeleteFramebuffers(1, &framebuffer->gl_framebuffer));
}

GLenum __attachment_to_gl[] = {
	GL_COLOR_ATTACHMENT0,
	GL_COLOR_ATTACHMENT1,
	GL_COLOR_ATTACHMENT2,
	GL_COLOR_ATTACHMENT3,
	GL_COLOR_ATTACHMENT4,
	GL_COLOR_ATTACHMENT5,
	GL_COLOR_ATTACHMENT6,
	GL_COLOR_ATTACHMENT7,
	GL_DEPTH_ATTACHMENT,
	GL_STENCIL_ATTACHMENT,
	GL_DEPTH_STENCIL_ATTACHMENT,
};

void framebuffer_attach(framebuffer_t* framebuffer, framebuffer_attachment_e attachment_type, texture_t* texture){
	GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->gl_framebuffer));
	GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, __attachment_to_gl[attachment_type], GL_TEXTURE_2D, texture->gl_texture, 0));
	if(attachment_type <= FRAMEBUFFER_COLOR_7) /*use on all color attachments*/{
		GL_CALL(glDrawBuffer(__attachment_to_gl[attachment_type]));
	}
	uint32_t result = 0;
	GL_CALL(result = glCheckFramebufferStatus(GL_FRAMEBUFFER));
	if (result != GL_FRAMEBUFFER_COMPLETE){
		ERROR("Could not add attachments to framebuffer ");
	}
	GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}
