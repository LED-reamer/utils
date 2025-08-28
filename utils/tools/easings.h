#pragma once
//link with -lm
#include <math.h>

// === EASING FUNCTIONS ===
// parameter t = [0, 1]
// ease _ + in/out/in_out + _ sine/quad/cubic/quart/quint/expo/circ/back/elastic/bounce

// - sine -

static inline float ease_in_sine(float t){
	return 1.0f - cosf((t * M_PI)/2.0f);
}

static inline float ease_out_sine(float t){
	return sinf((t * M_PI)/2.0f);
}

static inline float ease_in_out_sine(float t){
	return -(cosf(M_PI * t) - 1.0f) / 2.0f;
}

// - quad -

static inline float ease_in_quad(float t){
	return t*t;
}

static inline float ease_out_quad(float t){
	return 1.0f - (1.0f - t) * (1.0f - t);
}

static inline float ease_in_out_quad(float t){
	return (t < 0.5f) ? 2.0f * t * t : 1.0f - powf(-2.0f * t + 2.0f, 2.0f) / 2.0f;
}

// - cubic -

static inline float ease_in_cubic(float t){
	return t*t*t;
}

static inline float ease_out_cubic(float t){
	return 1.0f - powf(1.0f - t, 3.0f);
}

static inline float ease_in_out_cubic(float t){
	return (t < 0.5f) ? 4.0f * t * t * t : 1.0f - powf(-2.0f * t + 2.0f, 3.0f) / 2.0f;
}

// - quart -

static inline float ease_in_quart(float t){
	return t*t*t*t;
}

static inline float ease_out_quart(float t){
	return 1.0f - powf(1.0f - t, 4.0f);
}

static inline float ease_in_out_quart(float t){
	return (t < 0.5f) ? 8.0f * t * t * t * t : 1.0f - powf(-2.0f * t + 2.0f, 4.0f) / 2.0f;
}

// - quint -

static inline float ease_in_quint(float t){
	return t*t*t*t*t;
}

static inline float ease_out_quint(float t){
	return 1.0f - powf(1.0f - t, 5.0f);
}

static inline float ease_in_out_quint(float t){
	return (t < 0.5f) ? 16.0f * t * t * t * t * t : 1.0f - powf(-2.0f * t + 2.0f, 5.0f) / 2.0f;
}

// - expo -

static inline float ease_in_expo(float t){
	return (t == 0) ? 0 : powf(2.0f, 10.0f * t - 10.0f);
}

static inline float ease_out_expo(float t){
	return (t == 1.0f) ? 1.0f : 1.0f - powf(2.0f, -10.0f * t);
}

static inline float ease_in_out_expo(float t){
	return (t == 0.0f)
	  ? 0.0f
	  : (t == 1.0f)
	  ? 1.0f
	  : t < 0.5f ? powf(2.0f, 20.0f * t - 10.0f) / 2.0f
	  : (2.0f - powf(2.0f, -20.0f * t + 10.0f)) / 2.0f;
}

// - circ -

static inline float ease_in_circ(float t){
	return 1.0f - sqrtf(1.0f - powf(t, 2.0f));
}

static inline float ease_out_circ(float t){
	return sqrtf(1.0f - powf(t - 1.0f, 2.0f));
}

static inline float ease_in_out_circ(float t){
	return (t < 0.5f)
	  ? (1.0f - sqrtf(1.0f - powf(2.0f * t, 2.0f))) / 2.0f
	  : (sqrtf(1.0f - powf(-2.0f * t + 2.0f, 2.0f)) + 1.0f) / 2.0f;
}

// - back -

static inline float ease_in_back(float t){
	return 2.70158f * t * t * t - 1.70158f * t * t;
}

static inline float ease_out_back(float t){
	return 1.0f + 2.70158f * powf(t - 1.0f, 3.0f) + 1.70158f * powf(t - 1.0f, 2.0f);
}

static inline float ease_in_out_back(float t){
	const float c1 = 1.70158f;
	const float c2 = c1 * 1.525f;
	return (t < 0.5f)
	  ? (powf(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) / 2.0f
	  : (powf(2.0f * t - 2.0f, 2.0f) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
}

// - elastic -

static inline float ease_in_elastic(float t){
	const float c4 = (2.0f * M_PI) / 3.0f;
	
	return (t == 0.0f)
	  ? 0.0f
	  : (t == 1.0f)
	  ? 1.0f
	  : -powf(2.0f, 10.0f * t - 10.0f) * sinf((t * 10.0f - 10.75f) * c4);
}

static inline float ease_out_elastic(float t){
	const float c4 = (2.0f * M_PI) / 3.0f;
	
	return (t == 0.0f)
	  ? 0.0f
	  : (t == 1.0f)
	  ? 1.0f
	  : powf(2.0f, -10.0f * t) * sinf((t * 10.0f - 0.75f) * c4) + 1.0f;
}

static inline float ease_in_out_elastic(float t){
	const float c5 = (2.0f * M_PI) / 4.5f;
	
	return (t == 0.0f)
	  ? 0.0f
	  : (t == 1.0f)
	  ? 1.0f
	  : (t < 0.5f)
	  ? -(powf(2.0f, 20.0f * t - 10.0f) * sinf((20.0f * t - 11.125f) * c5)) / 2.0f
	  : (powf(2.0f, -20.0f * t + 10.0f) * sinf((20.0f * t - 11.125f) * c5)) / 2.0f + 1.0f;
}

// - bounce -
static inline float ease_out_bounce(float t){
	const float n1 = 7.5625f;
	const float d1 = 2.75f;
	
	if (t < 1.0f / d1) {
	    return n1 * t * t;
	} else if (t < 2.0f / d1) {
		t -= 1.5f / d1;
	    return n1 * t * t + 0.75f;
	} else if (t < 2.5 / d1) {
		t -= 2.25f / d1;
	    return n1 * t * t + 0.9375f;
	} else {
		t -= 2.625f / d1;
	    return n1 * t * t + 0.984375f;
	}
}

static inline float ease_in_bounce(float t){
	return 1.0f - ease_out_bounce(1.0f - t);
}


static inline float ease_in_out_bounce(float t){
	return (t < 0.5f)
	  ? (1.0f - ease_out_bounce(1.0f - 2.0f * t)) / 2.0f
	  : (1.0f + ease_out_bounce(2.0f * t - 1.0f)) / 2.0f;
}
