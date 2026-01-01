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
	vec3_t center_of_mass;
	float mass;
	vec3_t force;
	vec3_t velocity;
	
	vec4_t rotation;//quaternion xyz - axis, w - radians
	vec3_t inertia;
	vec3_t torque;
	vec3_t angular_velocity;
	bool is_static;
	bool is_sleeping;
	float sleep_timer;//how long object had low energy before sleep
	
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
	float angular_damping;
	//default material properties //TODO add individual materials later
	float restitution;
	float friction;

	float baumgarte_bias;
	float allowed_penetration;
	float velocity_threshold;

	float sleep_linear_threshold;
	float sleep_angular_threshold;
	float sleep_time_required;
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

	//accumulated impulses for warm starting
	float normal_impulse;
	float tangent_impulse[2];
}physics_collision_t;

physics_collision_t* physics_get_current_collisions(physics_world_t* physics_world, size_t* num_collisions);
