#include "types.h"
#include "math.h"
#include "logger.h"

int main(){
	vec2f a = VEC2F(5, 5);
	vec2f b = VEC2F(2, 2);

	a = vec2_scale(a, 3);
	logger.info("%f, %f", a.x, a.y);
	return 0;
}
