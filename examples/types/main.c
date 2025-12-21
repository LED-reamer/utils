#include "utils/types.h"
#include "utils/logging.h"
#include "cglm/include/cglm/cglm.h"

static size_t cases_failed = 0;
static size_t cases_handled = 0;

void print_types_matrix(mat4x4_t* m){
	for(int row = 0; row < 4; row++){
		for(int column = 0; column < 4; column++){
			printf("%f ", m->m[column][row]);
		}
		printf("\n");
	}
	printf("\n");
}

void print_cglm_matrix(mat4 m){
	for(int row = 0; row < 4; row++){
		for(int column = 0; column < 4; column++){
			printf("%f ", m[column][row]);
		}
		printf("\n");
	}
	printf("\n");
}

bool compare_matrices(mat4x4_t* m, mat4 cglm_m, const char* case_name){
	cases_handled++;
	for(int row = 0; row < 4; row++){
		for(int column = 0; column < 4; column++){
			if(m->m[column][row] != cglm_m[column][row]){
				printf(ANSI_RED "Mismatch (%s)" ANSI_RESET "\n", case_name);
				printf("difference: %f\n", m->m[column][row] - cglm_m[column][row]);
				printf("types matrix\n");
				print_types_matrix(m);
				printf("cglm matrix\n");
				print_cglm_matrix(cglm_m);
				cases_failed++;
				return false;
			}
		}
	}
	return true;
}

bool compare_vec4(vec4_t* v4, vec4 cglm_v4, const char* case_name){
	cases_handled++;
	for(int row = 0; row < 4; row++){
		if(v4->xyzw[row] != cglm_v4[row]){
			printf(ANSI_RED "Mismatch (%s)" ANSI_RESET "\n", case_name);
			printf("types vec4\n");
			printf("(%f, %f, %f, %f)\n", v4->x, v4->y, v4->z, v4->w);
			printf("cglm vec4\n");
			printf("(%f, %f, %f, %f)\n", cglm_v4[0], cglm_v4[1], cglm_v4[2], cglm_v4[3]);
			cases_failed++;
			return false;
		}
	}
	return true;
}

float test_matrix_a[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
float test_matrix_b[16] = {17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32};

void set_matrices(mat4x4_t* m, mat4 cglm_m, mat4x4_t* m_b, mat4 cglm_m_b){
	*m = mat4x4_copy_array(test_matrix_a);
	glm_mat4_make(test_matrix_a, cglm_m);

	*m_b = mat4x4_copy_array(test_matrix_b);
	glm_mat4_make(test_matrix_b, cglm_m_b);
}

int main(){
	#define SET_MATRICES() set_matrices(&types_matrix, cglm_matrix, &types_matrix_b, cglm_matrix_b)
	mat4x4_t types_matrix_b;
	mat4 cglm_matrix_b;
	
	mat4x4_t types_matrix = mat4x4_identity();
	mat4 cglm_matrix;
	glm_mat4_identity(cglm_matrix);
	compare_matrices(&types_matrix, cglm_matrix, "mat4x4_identity");

	SET_MATRICES();
	compare_matrices(&types_matrix, cglm_matrix, "mat4x4_copy_array");

	SET_MATRICES();
	types_matrix = mat4x4_mul(&types_matrix, &types_matrix_b);
	glm_mat4_mul(cglm_matrix, cglm_matrix_b, cglm_matrix);
	compare_matrices(&types_matrix, cglm_matrix, "mat4x4_mul");

	SET_MATRICES();
	vec4_t vector4 = vec4(185, 90, 64, 987);
	vec4_t result_vec4 = mat4x4_mul_vec4(&types_matrix, &vector4);
	vec4 vec4_cglm = {185, 90, 64, 987};
	vec4 result_vec4_cglm;
	glm_mat4_mulv(cglm_matrix, vec4_cglm, result_vec4_cglm);
	compare_vec4(&result_vec4, result_vec4_cglm, "mat4x4_mul_vec4");

	SET_MATRICES();
	vec3_t vector3 = vec3(1, 2, 3);
	vec3 vector3_cglm = {1, 2, 3};
	mat4x4_translate(&types_matrix, vector3.x, vector3.y, vector3.z);
	mat4 cglm_helper_matrix;
	glm_translate_make(cglm_helper_matrix, vector3_cglm);
	glm_mat4_mul(cglm_matrix, cglm_helper_matrix, cglm_matrix);
	compare_matrices(&types_matrix, cglm_matrix, "mat4x4_translate");

	SET_MATRICES();
	mat4x4_scale(&types_matrix, vector3.x, vector3.y, vector3.z);
	glm_scale(cglm_matrix, vector3_cglm);
	compare_matrices(&types_matrix, cglm_matrix, "mat4x4_scale");

	SET_MATRICES();
	mat4x4_rotate(&types_matrix, 15.0f, vector3);
	glm_rotate(cglm_matrix, 15.0f, vector3_cglm);
	compare_matrices(&types_matrix, cglm_matrix, "mat4x4_rotate");
	
	SET_MATRICES();
	types_matrix = mat4x4_lookat(vec3(789, 564, 21), vec3(94, 321, 8), vec3(978,6 ,12));
	vec3 eye = {789, 564, 21};
	vec3 center = {94, 321, 8};
	vec3 up = {978,6 ,12};
	glm_lookat(eye, center, up, cglm_matrix);
	compare_matrices(&types_matrix, cglm_matrix, "mat4x4_lookat");

	SET_MATRICES();
	types_matrix = mat4x4_ortho(1, 2, 3, 4, 0.1, 1000);
	glm_ortho(1, 2, 3, 4, 0.1, 1000, cglm_matrix);
	compare_matrices(&types_matrix, cglm_matrix, "mat4x4_ortho");
	
	SET_MATRICES();
	types_matrix = mat4x4_perspective(1, 2, 3, 4);
	glm_perspective(1, 2, 3, 4, cglm_matrix);
	compare_matrices(&types_matrix, cglm_matrix, "mat4x4_perspective");

	SET_MATRICES();
	types_matrix = mat4x4_transpose(types_matrix);
	glm_mat4_transpose(cglm_matrix);
	compare_matrices(&types_matrix, cglm_matrix, "mat4x4_transpose");

	SET_MATRICES();
	types_matrix = mat4x4_identity();
	glm_mat4_identity(cglm_matrix);
	types_matrix = mat4x4_inverse(types_matrix);
	glm_mat4_inv(cglm_matrix, cglm_matrix);
	compare_matrices(&types_matrix, cglm_matrix, "mat4x4_inverse");

	if(cases_failed == 0){
		printf(ANSI_GREEN "All tests passed (%zu/%zu)" ANSI_RESET "\n", cases_handled, cases_handled);
	} else {
		printf(ANSI_RED "Failed %zu tests (%zu/%zu)" ANSI_RESET "\n", cases_failed, cases_handled-cases_failed, cases_handled);
	}
	return 0;
}
