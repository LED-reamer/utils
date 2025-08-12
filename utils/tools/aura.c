#include "aura.h"

#include <SDL3/SDL_render.h>
#include <stdarg.h>	 //va list

#include "../logging.h"

aura_context_t aura_init(void *sdl3_window) {
	aura_context_t ctx = {0};
	SDL_HideWindow(sdl3_window);  // minimizes flicker... TODO intended: create window in // hidden state -> add renderer -> show window
	ctx.sdl3_window = sdl3_window;
	ctx.sdl3_renderer = SDL_CreateRenderer(sdl3_window, NULL);
	SDL_ShowWindow(sdl3_window);
	if (!ctx.sdl3_renderer)
		ERROR("Could not init SDL3 renderer");

	SDL_SetRenderDrawBlendMode(ctx.sdl3_renderer, SDL_BLENDMODE_BLEND);
	return ctx;
}

void aura_deinit(aura_context_t *ctx) {
	aura_reset_textures(ctx);
	SDL_DestroyRenderer(ctx->sdl3_renderer);
	*ctx = (aura_context_t){0};
}

void aura_render(aura_context_t *ctx) {
	aura_set_target(ctx, NULL);

	SDL_RenderPresent(ctx->sdl3_renderer);
}

void aura_clip(aura_context_t *ctx, rectangle_t rectangle) {
	if (rectangle.x == 0 && rectangle.y == 0 && rectangle.w == 0 && rectangle.h == 0)
		SDL_SetRenderClipRect(ctx->sdl3_renderer, NULL);
	else
		SDL_SetRenderClipRect(ctx->sdl3_renderer, (SDL_Rect *)&rectangle);
}

void __set_color(aura_context_t *ctx, color_t color) {
	SDL_SetRenderDrawColor(ctx->sdl3_renderer, color.r * 255 + 0.5,
						   color.g * 255 + 0.5, color.b * 255 + 0.5,
						   color.a * 255 + 0.5);
}

void aura_clear(aura_context_t *ctx, color_t clear_color) {
	__set_color(ctx, clear_color);
	SDL_RenderClear(ctx->sdl3_renderer);
}

void aura_debug_text(aura_context_t *ctx, const char *text, vec2_t position, color_t color) {
	__set_color(ctx, color);
	SDL_RenderDebugText(ctx->sdl3_renderer, position.x, position.y, text);
}

void aura_debug_text_fmt(aura_context_t *ctx, vec2_t position, color_t color, const char *fmt, ...) {
	char buffer[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer) - 1 /*reserve last byte NULL-terminator*/, fmt, args);
	va_end(args);

	aura_debug_text(ctx, buffer, position, color);
}

void aura_point(aura_context_t *ctx, vec2_t position, color_t color) {
	__set_color(ctx, color);
	SDL_RenderPoint(ctx->sdl3_renderer, position.x, position.y);
}

void aura_line(aura_context_t *ctx, vec2_t position_1, vec2_t position_2, color_t color) {
	__set_color(ctx, color);
	SDL_RenderLine(ctx->sdl3_renderer, position_1.x, position_1.y, position_2.x, position_2.y);
}

void aura_rectangle(aura_context_t *ctx, rectangle_t rectangle, color_t color) {
	__set_color(ctx, color);
	SDL_RenderFillRect(ctx->sdl3_renderer, (SDL_FRect *)&rectangle);
}

aura_texture_t *aura_add_texture(aura_context_t *ctx, uint32_t width, uint32_t height, void *pixels) {
	if (ctx->num_textures + 1 > AURA_MAX_TEXTURES) {
		ERROR("increase AURA_MAX_TEXTURES (%u)", AURA_MAX_TEXTURES);
		return NULL;
	}

	aura_texture_t *tex = &ctx->textures[ctx->num_textures];
	ctx->num_textures++;

	tex->width = width;
	tex->height = height;

	SDL_Surface *surface = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_RGBA8888, pixels, width * 4);
	tex->sdl3_texture = SDL_CreateTextureFromSurface(ctx->sdl3_renderer, surface);
	SDL_DestroySurface(surface);

	return tex;
}

aura_target_t *aura_add_target(aura_context_t *ctx, uint32_t width, uint32_t height) {
	if (ctx->num_textures + 1 > AURA_MAX_TEXTURES) {
		ERROR("increase AURA_MAX_TEXTURES (%u)", AURA_MAX_TEXTURES);
		return NULL;
	}

	aura_texture_t *tex = &ctx->textures[ctx->num_textures];
	ctx->num_textures++;

	tex->width = width;
	tex->height = height;
	tex->sdl3_texture = SDL_CreateTexture(ctx->sdl3_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);

	return tex;
}

void aura_reset_textures(aura_context_t* ctx) {
	for (uint32_t i = 0; i < ctx->num_textures; i++) {
		SDL_DestroyTexture((SDL_Texture *)ctx->textures[i].sdl3_texture);
	}
	ctx->num_textures = 0;
}

void aura_set_texture_scale_mode(aura_texture_t *texture, aura_scale_mode_e scale_mode) {
	switch (scale_mode) {
		case AURA_NEAREST:
			SDL_SetTextureScaleMode(texture->sdl3_texture, SDL_SCALEMODE_NEAREST);
			break;
		case AURA_LINEAR:
			SDL_SetTextureScaleMode(texture->sdl3_texture, SDL_SCALEMODE_LINEAR);
			break;
		case AURA_PIXELART:
			FATAL_ERROR("check if SDL3 version is good enough to support this..."); /*SDL_SetTextureScaleMode(texture->sdl3_texture, SDL_SCALEMODE_PIXELART);*/
			break;
		default:
			UNREACHABLE("unreachable");
			break;
	}
}

void aura_set_texture_blend_mode(aura_texture_t *texture, aura_blend_mode_e blend_mode) {
	switch (blend_mode) {
		case AURA_NO_BLEND:
			SDL_SetTextureBlendMode(texture->sdl3_texture, SDL_BLENDMODE_NONE);
			break;
		case AURA_BLEND:
			SDL_SetTextureBlendMode(texture->sdl3_texture, SDL_BLENDMODE_BLEND);
			break;
		case AURA_BLEND_PREMULTIPLIED:
			SDL_SetTextureBlendMode(texture->sdl3_texture, SDL_BLENDMODE_BLEND_PREMULTIPLIED);
			break;
		case AURA_ADD:
			SDL_SetTextureBlendMode(texture->sdl3_texture, SDL_BLENDMODE_ADD);
			break;
		case AURA_ADD_PREMULTIPLIED:
			SDL_SetTextureBlendMode(texture->sdl3_texture, SDL_BLENDMODE_ADD_PREMULTIPLIED);
			break;
		case AURA_MOD:
			SDL_SetTextureBlendMode(texture->sdl3_texture, SDL_BLENDMODE_MOD);
			break;
		case AURA_MUL:
			SDL_SetTextureBlendMode(texture->sdl3_texture, SDL_BLENDMODE_MUL);
			break;
		default:
			UNREACHABLE("unreachable");
			break;
	}
}

void aura_set_target(aura_context_t *ctx, aura_target_t *target) {
	if (target == NULL) {
		SDL_SetRenderTarget(ctx->sdl3_renderer, NULL);
		int32_t width, height;
		SDL_GetWindowSizeInPixels(ctx->sdl3_window, &width, &height);
		SDL_SetRenderViewport(ctx->sdl3_renderer, &(SDL_Rect){0, 0, width, height});
	} else {
		SDL_SetRenderViewport(ctx->sdl3_renderer, &(SDL_Rect){0, 0, target->width, target->height});
		SDL_SetRenderTarget(ctx->sdl3_renderer, target->sdl3_texture);
	}
}

void aura_texture(aura_context_t *ctx, aura_texture_t *texture, rectangle_t dst) {
	SDL_RenderTexture(ctx->sdl3_renderer, texture->sdl3_texture, NULL, (SDL_FRect *)&dst);
}

void aura_texture_src(aura_context_t *ctx, aura_texture_t *texture, rectangle_t dst, rectangle_t src) {
	SDL_RenderTexture(ctx->sdl3_renderer, texture->sdl3_texture, (SDL_FRect *)&src, (SDL_FRect *)&dst);
}

void aura_texture_sprite(aura_context_t *ctx, aura_texture_t *texture, rectangle_t dst, rectangle_t src, double degrees, vec2_t center, bool flip_vertical, bool flip_horizontal) {
	SDL_FlipMode flip_mode = 0;
	if (flip_vertical)
		flip_mode |= SDL_FLIP_VERTICAL;
	if (flip_horizontal)
		flip_mode |= SDL_FLIP_HORIZONTAL;

	SDL_RenderTextureRotated(ctx->sdl3_renderer, texture->sdl3_texture, (SDL_FRect *)&src, (SDL_FRect *)&dst, degrees, (SDL_FPoint *)&center, flip_mode);
}

void aura_texture_grid(aura_context_t *ctx, aura_texture_t *texture, float left_width, float right_width, float top_height, float bottom_height, rectangle_t destination_rectangle) {
	// TODO use tiled variant in newer SDL3 version
	SDL_RenderTexture9Grid(ctx->sdl3_renderer, texture->sdl3_texture, NULL, left_width, right_width, top_height, bottom_height, 1, (const SDL_FRect *)&destination_rectangle);
}

void aura_geometry(aura_context_t *ctx, aura_texture_t *texture, aura_vertex_t *vertices, size_t num_vertices, uint32_t *indices, size_t num_indices) {
	SDL_RenderGeometry(ctx->sdl3_renderer, (texture == NULL) ? NULL : texture->sdl3_texture, (SDL_Vertex *)vertices, num_vertices, (int32_t *)indices, num_indices);
}

void aura_rounded_rectangle(aura_context_t *ctx, rectangle_t rectangle, float radius, color_t color) {
	aura_rounded_rectangle_corners(ctx, rectangle, radius, radius, radius, radius, color);
}

static aura_vertex_t aura_rounded_rectangle_vertices[AURA_CORNER_VERTICES * 4];
static uint32_t aura_rounded_rectangle_indices[AURA_CORNER_VERTICES * 4 * 3 - 6];

void aura_rounded_rectangle_corners(aura_context_t *ctx, rectangle_t rectangle, float radius_tl, float radius_tr, float radius_bl, float radius_br, color_t color) {
	for (uint32_t i = 0; i < AURA_CORNER_VERTICES * 4; i++) {
		if (AURA_CORNER_VERTICES <= 1) {
			__set_color(ctx, color);
			SDL_RenderRect(ctx->sdl3_renderer, (SDL_FRect *)&rectangle);
		}

		float angle = 0;
		float angle_per_vertex = 90.0f / AURA_CORNER_VERTICES;
		if (i % AURA_CORNER_VERTICES == AURA_CORNER_VERTICES - 1)  // last corner vertex
			angle = 90;
		else
			angle = (i % AURA_CORNER_VERTICES) * angle_per_vertex;

		vec2_t position;
		switch (i / AURA_CORNER_VERTICES) {	 // corner
			case 0:
				position = vec2(radius_tl, radius_tl);
				angle -= 180;
				position = vec2_add(position, vec2(cosf(angle * M_PI / 180) * radius_tl, sinf(angle * M_PI / 180) * radius_tl));
				break;
			case 1:
				position = vec2(rectangle.w - radius_tr, radius_tr);
				angle -= 90;
				position = vec2_add(position, vec2(cosf(angle * M_PI / 180) * radius_tr, sinf(angle * M_PI / 180) * radius_tr));
				break;
			case 2:
				position = vec2(rectangle.w - radius_br, rectangle.h - radius_br);
				position = vec2_add(position, vec2(cosf(angle * M_PI / 180) * radius_br, sinf(angle * M_PI / 180) * radius_br));
				break;
			default:
				position = vec2(radius_bl, rectangle.h - radius_bl);
				angle += 90;
				position = vec2_add(position, vec2(cosf(angle * M_PI / 180) * radius_bl, sinf(angle * M_PI / 180) * radius_bl));
				break;
		}
		position = vec2_add(position, vec2(rectangle.x, rectangle.y));
		aura_rounded_rectangle_vertices[i] = (aura_vertex_t){
			.position = position,
			.color = color,
		};

		for (uint32_t i = 0; i < AURA_CORNER_VERTICES * 4 - 2; i++) {
			aura_rounded_rectangle_indices[i * 3 + 0] = 0;
			aura_rounded_rectangle_indices[i * 3 + 1] = i + 1;
			aura_rounded_rectangle_indices[i * 3 + 2] = (i == (AURA_CORNER_VERTICES * 4 - 1)) ? 1 : (i + 2);
		}
	}
	aura_geometry(ctx, NULL, aura_rounded_rectangle_vertices, AURA_CORNER_VERTICES * 4, aura_rounded_rectangle_indices, AURA_CORNER_VERTICES * 4 * 3 - 6);
}
