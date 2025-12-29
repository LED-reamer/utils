#pragma once
#include "allocator.h"
#include "arena.h"
#include "hashmap.h"
#include "types.h"

typedef enum{
	PHYSICS_SPHERE,
	PHYSICS_BOX,
}collider_type_e;

typedef struct{
	float radius;
}sphere_collider_t;

typedef struct{
	vec3_t size; // half extents
}box_collider_t;

typedef struct physics_object_t physics_object_t;

typedef struct physics_object_t{
	float mass;
	vec3_t force;
	vec3_t torque;
	vec3_t velocity;
	vec3_t center_of_mass;
	//vec4_t quaternion;// xyz - axis, w - radians
	bool is_static;
	
	collider_type_e collider_type;
	union{
		sphere_collider_t sphere;
		box_collider_t box;
	}as;

	//physics_object_t* parent;
}physics_object_t;

physics_object_t physics_get_default_sphere(vec3_t pos, float radius, float mass);
physics_object_t physics_get_default_box(vec3_t pos, vec3_t size/*half extents*/, float mass);

typedef struct{
	allocator_t* allocator;
	hashmap_t objects;
	arena_t collisions_arena;
	size_t num_collisions;

	//config
	vec3_t gravity;
	size_t solver_iterations;
	float linear_damping;
	//default material properties //TODO add individual materials later
	float restitution;
	float friction;
}physics_world_t;

physics_world_t physics_world_create(allocator_t* allocator);
void physics_world_destroy(physics_world_t* physics_world);

physics_object_t* physics_object_add(physics_world_t* physics_world, physics_object_t object_parameters);
void physics_object_remove(physics_world_t* physics_world, physics_object_t* object);
void physics_object_apply_force(physics_object_t* object, vec3_t force);
void physics_object_apply_offset_force(physics_object_t* object, vec3_t force, vec3_t offset_to_center_of_mass);
void physics_object_apply_torque(physics_object_t* object, vec3_t torque);

void physics_world_step(physics_world_t* physics_world, float dt);

typedef struct{
	physics_object_t* a;
	physics_object_t* b;
	vec3_t contact_point;
	vec3_t contact_normal;	//a to b
	float penetration;

	float normal_mass;		// effective mass for normal constraint
	float tangent_mass[2];	// effective mass for friction constraints
	vec3_t tangent[2];		// friction direction vectors

	float normal_impulse;
	float tangent_impulse[2];
	    
	float position_bias;
	
	float restitution;
	float friction;
}physics_collision_t;

physics_collision_t* physics_get_current_collisions(physics_world_t* physics_world, size_t* num_collisions);
