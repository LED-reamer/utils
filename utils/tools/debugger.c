#include "debugger.h"

#include <stdio.h>

#include "../window.h"
#include "aura.h"

#define DEBUGGER_MAX_TIMESTAMPS (1024)

#define DEBUGGER_ALLOCATION_GRAPH_ENTRIES (1024)
#define DEBUGGER_FPS_GRAPH_ENTRIES (1024)

#define DEBUGGER_SPACING (20)
#define DEBUGGER_FPS_TARGET (60)

color_t background_color = color(0.95, 0.95, 0.95, 1.0);
color_t background_color2 = color(0.92, 0.92, 0.92, 1.0);
color_t text_color = color(0.35, 0.35, 0.35, 1.0);
color_t critical_text_color = color(0.976, 0.443, 0.145, 1.0);
color_t graph_color = color(0.235, 0.478, 0.627, 1.0);
color_t graph_color2 = color(0.235, 0.478, 0.627, 0.15);

typedef struct{
	uint64_t id;//created from name
	const char* timestamp_name;
	r128 time_begin_s;
	r128 time_end_s;

	const char* function_name;
	const char* file_name;
	size_t line;
} _debugger_timestamp_t;

typedef struct {
	window_t window;
	aura_context_t aura;

	size_t memory_graph[DEBUGGER_ALLOCATION_GRAPH_ENTRIES];
	size_t memory_graph_len;

	float fps_graph[DEBUGGER_FPS_GRAPH_ENTRIES];
	size_t fps_graph_len;

	r128 time_last_s;

	_debugger_timestamp_t timestamps[DEBUGGER_MAX_TIMESTAMPS];
	size_t num_timestamps;
} _debugger_context_t;

static _debugger_context_t ctx;

void debugger_init() {
	memory_tracker_init(false);
	ctx = (_debugger_context_t){0};
	ctx.window = window_create("debugger", 600, 400);
	ctx.aura = aura_init(ctx.window.SDL3_window);
	ctx.time_last_s = window_get_time_s();
}

static char* format_bytes(size_t bytes) {
	static char formatted_size[10];	 // max format is "999.99 PB\0" -> 10 characters
	const char* units[] = {"B", "KB", "MB", "GB", "TB", "PB"};
	int i = 0;
	double size_in_units = (double)bytes;

	while (size_in_units >= 1024 && i < 5) {
		size_in_units /= 1024;
		i++;
	}
	if (i == 0)	 // bytes
		snprintf(formatted_size, sizeof(formatted_size), "%zu%s", bytes, units[i]);
	else
		snprintf(formatted_size, sizeof(formatted_size), "%.2f%s", size_in_units, units[i]);

	return formatted_size;
}

void _draw_memory_graph(size_t* graph, size_t currently_allocated, size_t max_value, rectangle_t dest) {
	aura_rectangle(&ctx.aura, dest, background_color2);

	float dx = dest.w / DEBUGGER_ALLOCATION_GRAPH_ENTRIES;

#ifdef DEBUGGER_ALLOCATION_GRAPH_SCALE_TO_MAX
	size_t highest_current_value = 0;
	for (size_t i = 0; i < DEBUGGER_ALLOCATION_GRAPH_ENTRIES; i++) {
		if (highest_current_value < graph[i]) highest_current_value = graph[i];
	}
#endif

	float prev_y = 0;
	for (size_t i = 0; i < DEBUGGER_ALLOCATION_GRAPH_ENTRIES; i++) {
		// graph space (y up)
		float x = i * dx;
		float prev_x = (i == 0) ? 0 : (i - 1) * dx;
#ifdef DEBUGGER_ALLOCATION_GRAPH_SCALE_TO_MAX
		float y = (graph[i] / (float)highest_current_value) * dest.h;
#else
		float y = (graph[i] / (float)max_value) * dest.h;
#endif

		// handle coordinate transform here (destination rect and y down)
		color_t color_top = graph_color;
		color_t color_bottom = graph_color2;

		vec2_t top_left = vec2(prev_x + dest.x, (-prev_y) + dest.y + dest.h);
		vec2_t top_right = vec2(x + dest.x, (-y) + dest.y + dest.h);
		vec2_t bottom_left = vec2(prev_x + dest.x, dest.y + dest.h);
		vec2_t bottom_right = vec2(x + dest.x, dest.y + dest.h);

		aura_vertex_t vertices[6];
		vertices[0] = (aura_vertex_t){.position = top_left, .color = color_top};
		vertices[1] = (aura_vertex_t){.position = bottom_left, .color = color_bottom};
		vertices[2] = (aura_vertex_t){.position = top_right, .color = color_top};
		vertices[3] = vertices[2];
		vertices[4] = vertices[1];
		vertices[5] = (aura_vertex_t){.position = bottom_right, .color = color_bottom};

		aura_geometry(&ctx.aura, NULL, vertices, 6, NULL, 0);

		prev_y = y;
	}

	// axis
	aura_debug_text_fmt(&ctx.aura, vec2(dest.x, dest.y), text_color, "%s max", format_bytes(max_value));
	aura_debug_text_fmt(&ctx.aura, vec2(dest.x, dest.y + DEBUGGER_SPACING / 2), text_color, "%s currently", format_bytes(currently_allocated));
}

void _draw_fps_graph(float* graph, size_t graph_len, float target, rectangle_t dest) {
	aura_rectangle(&ctx.aura, dest, background_color2);

	float dx = dest.w / DEBUGGER_FPS_GRAPH_ENTRIES;

	float avg_fps = 0;
	float min_fps = FLT_MAX;
	float max_fps = 0;

	float prev_y = 0;
	for (size_t i = 0; i < DEBUGGER_FPS_GRAPH_ENTRIES; i++) {
		// graph space (y up)
		float x = i * dx;
		float prev_x = (i == 0) ? 0 : (i - 1) * dx;
		avg_fps += graph[i];
		float capped_fps = graph[i];
		if (capped_fps > target * 1.25f) capped_fps = target * 1.25f;
		float y = (capped_fps / (target * 1.25f)) * dest.h;

		if (max_fps < graph[i]) max_fps = graph[i];
		if (min_fps > graph[i]) min_fps = graph[i];

		// handle coordinate transform here (destination rect and y down)
		color_t color_top = graph_color;
		color_t color_bottom = graph_color2;

		vec2_t top_left = vec2(prev_x + dest.x, (-prev_y) + dest.y + dest.h);
		vec2_t top_right = vec2(x + dest.x, (-y) + dest.y + dest.h);
		vec2_t bottom_left = vec2(prev_x + dest.x, dest.y + dest.h);
		vec2_t bottom_right = vec2(x + dest.x, dest.y + dest.h);

		aura_vertex_t vertices[6];
		vertices[0] = (aura_vertex_t){.position = top_left, .color = color_top};
		vertices[1] = (aura_vertex_t){.position = bottom_left, .color = color_bottom};
		vertices[2] = (aura_vertex_t){.position = top_right, .color = color_top};
		vertices[3] = vertices[2];
		vertices[4] = vertices[1];
		vertices[5] = (aura_vertex_t){.position = bottom_right, .color = color_bottom};

		aura_geometry(&ctx.aura, NULL, vertices, 6, NULL, 0);

		prev_y = y;
	}
	avg_fps /= graph_len;

	// axis
	aura_debug_text_fmt(&ctx.aura, vec2(dest.x, dest.y), text_color, "%.1f target", target);
	aura_debug_text_fmt(&ctx.aura, vec2(dest.x, dest.y + DEBUGGER_SPACING / 2), text_color, "%.1ffps average, %.1ffps max, %.1ffps min", avg_fps, max_fps, min_fps);
}

void graph_add_value(size_t* graph, size_t* graph_len, size_t value, size_t max_entries) {
	if (*graph_len == max_entries) {
		for (size_t i = 1; i < *graph_len; i++) {
			graph[i - 1] = graph[i];
		}
	}
	if (*graph_len < max_entries) {
		(*graph_len)++;
	}
	graph[*graph_len - 1] = value;
}

void graph_add_float(float* graph, size_t* graph_len, float value, size_t max_entries) {
	if (*graph_len == max_entries) {
		for (size_t i = 1; i < *graph_len; i++) {
			graph[i - 1] = graph[i];
		}
	}
	if (*graph_len < max_entries) {
		(*graph_len)++;
	}
	graph[*graph_len - 1] = value;
}

void draw_timestamps(vec2_t destination, r128 frame_time){
	if(frame_time == 0) frame_time = -1;
	aura_debug_text_fmt(&ctx.aura, vec2(destination.x, destination.y), text_color, "frametime %Lf ms", frame_time * 1000);
	for(size_t i = 0; i < DEBUGGER_MAX_TIMESTAMPS; i++){
		if(ctx.timestamps[i].timestamp_name != NULL)
			aura_debug_text_fmt(&ctx.aura, vec2(destination.x, destination.y + DEBUGGER_SPACING * (i+1)), text_color, 
				"[%s in %s:%zu %s] %Lf ms => (%.2Lf%% of frametime)", 
				ctx.timestamps[i].timestamp_name, 
				ctx.timestamps[i].file_name, 
				ctx.timestamps[i].line, 
				ctx.timestamps[i].function_name, 
				1000*(ctx.timestamps[i].time_end_s - ctx.timestamps[i].time_begin_s),
				((ctx.timestamps[i].time_end_s - ctx.timestamps[i].time_begin_s) / frame_time) * 100
			);
	}

	memset(ctx.timestamps, 0, DEBUGGER_MAX_TIMESTAMPS * sizeof(_debugger_timestamp_t));
	ctx.num_timestamps = 0;
}

void debugger_update() {
	r128 now = window_get_time_s();
	if (!window_open(&ctx.window)) return;
	if (key_just_down(&ctx.window, KEY_ESCAPE)) window_close(&ctx.window);
	window_always_on_top(&ctx.window, true);
	window_update(&ctx.window);

	// memory stats
	size_t currently_allocated, max_allocated;
	memory_tracker_get_stats(&currently_allocated, &max_allocated);
	graph_add_value(ctx.memory_graph, &ctx.memory_graph_len, currently_allocated, DEBUGGER_ALLOCATION_GRAPH_ENTRIES);

	// fps stats
	float fps = 1.0f / (now - ctx.time_last_s);
	graph_add_float(ctx.fps_graph, &ctx.fps_graph_len, fps, DEBUGGER_FPS_GRAPH_ENTRIES);

	// render
	aura_clear(&ctx.aura, background_color);

	float y_offset = DEBUGGER_SPACING;
	aura_debug_text_fmt(&ctx.aura, vec2(DEBUGGER_SPACING, y_offset), text_color, "Memory usage");
	y_offset += DEBUGGER_SPACING;
	_draw_memory_graph(ctx.memory_graph, currently_allocated, max_allocated, rectangle(DEBUGGER_SPACING, y_offset, window_get_width(&ctx.window) - DEBUGGER_SPACING * 2, window_get_height(&ctx.window) / 5));
	y_offset += DEBUGGER_SPACING + window_get_height(&ctx.window) / 5;
	aura_debug_text_fmt(&ctx.aura, vec2(DEBUGGER_SPACING, y_offset), text_color, "fps");
	y_offset += DEBUGGER_SPACING;
	_draw_fps_graph(ctx.fps_graph, ctx.fps_graph_len, DEBUGGER_FPS_TARGET, rectangle(DEBUGGER_SPACING, y_offset, window_get_width(&ctx.window) - DEBUGGER_SPACING * 2, window_get_height(&ctx.window) / 5));
	y_offset += DEBUGGER_SPACING + window_get_height(&ctx.window) / 5;
	draw_timestamps(vec2(DEBUGGER_SPACING, y_offset), (now - ctx.time_last_s));
	y_offset += DEBUGGER_SPACING + ctx.num_timestamps * DEBUGGER_SPACING;
	
	aura_render(&ctx.aura);
	ctx.time_last_s = window_get_time_s();	// skip this function
}

void debugger_deinit() {
	aura_deinit(&ctx.aura);
	ctx = (_debugger_context_t){0};
	memory_tracker_deinit();
}

uint64_t __hash_id_from_string(const char* string) {
	uint64_t hash = 14695981039346656037U;	// FNV-1a
	unsigned char c;

	while ((c = *string++)) {
		hash ^= c;
		hash *= 1099511628211U;	 // FNV-1a prime
	}
	hash ^= hash >> 33;
	hash *= 0xff51afd7ed558ccd;
	hash ^= hash >> 33;

	return hash & 0xFFFFFFFFFFFFFFFF;
}

void __debugger_timestamp_begin(const char* name, const char* function, const char* file, size_t line){
	if(ctx.num_timestamps + 1 == DEBUGGER_MAX_TIMESTAMPS){
		return;
	}
	
	_debugger_timestamp_t new_timestamp = {
		.timestamp_name = name,
		.id = __hash_id_from_string(name),
		.function_name = function,
		.file_name = file,
		.line = line,
		.time_begin_s = window_get_time_s(),
	};
	ctx.timestamps[ctx.num_timestamps] = new_timestamp;
	ctx.num_timestamps++;
}

void debugger_timestamp_end(const char* name){
	r128 time_end_s	= window_get_time_s();
	uint64_t id = __hash_id_from_string(name);

	for(size_t i = 0; i < DEBUGGER_MAX_TIMESTAMPS; i++){
		if(ctx.timestamps[i].id == id){
			ctx.timestamps[i].time_end_s = time_end_s;
		}
	}
}
