#include "physics.h"

#include "logging.h" //TODO remove when finished. not needed except for "not implemented"

#define PHYSICS_EPSILON (1e-6f)

physics_object_t physics_get_default_sphere(vec3_t pos, float radius, float mass){
	return (physics_object_t){
		.mass = mass,
		.center_of_mass = pos,
		.collider_type = PHYSICS_SPHERE,
		.as.sphere = (sphere_collider_t){.radius = radius},
	};
}

physics_object_t physics_get_default_box(vec3_t pos, vec3_t size/*half extents*/, float mass){
	return (physics_object_t){
		.mass = mass,
		.center_of_mass = pos,
		.collider_type = PHYSICS_BOX,
		.as.box = (box_collider_t){.size = size},
	};
}

physics_world_t physics_world_create(allocator_t* allocator){
	return (physics_world_t) {
		.allocator = allocator,
		.objects = hashmap_create(allocator),
		.collisions_arena = arena_create(allocator, 4096, 0),
		.num_collisions = 0,
		.gravity = vec3(0, -9.81f, 0),
		.solver_iterations = 8,
		.linear_damping = 0.99f,
		.restitution = 0.0f,
		.friction = 0.3f,
	};
}

void physics_world_destroy(physics_world_t* physics_world){
	hashmap_foreach(&physics_world->objects, object){
		physics_world->allocator->afree(object->value_ptr);
	}
	hashmap_destroy(&physics_world->objects);
	arena_destroy(&physics_world->collisions_arena);
	*physics_world = (physics_world_t){0};
}

physics_object_t* physics_object_add(physics_world_t* physics_world, physics_object_t object_parameters){
	physics_object_t* object = physics_world->allocator->amalloc(sizeof(physics_object_t));
	memcpy(object, &object_parameters, sizeof(physics_object_t));
	hashmap_set(&physics_world->objects, (hashmap_key_t)object, (void*)object);

	return object;
}

void physics_object_remove(physics_world_t* physics_world, physics_object_t* object){
	physics_world->allocator->afree(object);
	hashmap_remove(&physics_world->objects, (hashmap_key_t)object);
}

void physics_object_apply_force(physics_object_t* object, vec3_t force){
	object->force = vec3_add(object->force, force);
}

void physics_object_apply_offset_force(physics_object_t* object, vec3_t force, vec3_t offset_to_center_of_mass){
	object->torque = vec3_add(object->torque, vec3_cross(force, offset_to_center_of_mass));
}

void physics_object_apply_torque(physics_object_t* object, vec3_t torque){
	object->torque = vec3_add(object->torque, torque);
}

void apply_gravity_and_integrate_velocities(physics_world_t* physics_world, float dt);
void collision_detection(physics_world_t* physics_world);
void solve_constraints(physics_world_t* physics_world);
void integrate_positions(physics_world_t* physics_world, float dt);

void physics_world_step(physics_world_t* physics_world, float dt){
	if (dt <= PHYSICS_EPSILON) return; //TODO check if this is needed
	arena_reset(&physics_world->collisions_arena);
	physics_world->num_collisions = 0;

	apply_gravity_and_integrate_velocities(physics_world, dt);
	collision_detection(physics_world);
	solve_constraints(physics_world);
	integrate_positions(physics_world, dt);	
}

void apply_gravity_and_integrate_velocities(physics_world_t* physics_world, float dt){
	hashmap_foreach(&physics_world->objects, obj_entry){
		physics_object_t* obj = (physics_object_t*)obj_entry->value_ptr;
		if(obj->is_static) continue;
		
		physics_object_apply_force(obj, physics_world->gravity);

		// a = F/m
		// v = a*dt
		// => v += ((F/m)*dt)*linear_damping
		
		obj->velocity = vec3_add(obj->velocity, vec3_scale(vec3_scale(vec3_scale(obj->force, 1/obj->mass), dt), physics_world->linear_damping));
		//clear forces
		obj->force = vec3(0, 0, 0);
	}
}

size_t collision_sphere_sphere(physics_object_t* a, physics_object_t* b, arena_t* collisions_arena);
void collision_detection(physics_world_t* physics_world){
	hashmap_foreach(&physics_world->objects, obj_a_entry){
		physics_object_t* obj_a = (physics_object_t*)obj_a_entry->value_ptr;
		hashmap_foreach(&physics_world->objects, obj_b_entry){
			physics_object_t* obj_b = (physics_object_t*)obj_b_entry->value_ptr;
			if(obj_a == obj_b) continue; //prevent self collisions

			if(obj_a->collider_type == PHYSICS_SPHERE && obj_b->collider_type == PHYSICS_SPHERE)
				physics_world->num_collisions += collision_sphere_sphere(obj_a, obj_b, &physics_world->collisions_arena);
			else
				ERROR("collision case not implemented!");
		}
	}
}

float object_get_inverse_mass(physics_object_t* obj){
	if(obj->is_static) return 0.0f;
	else return 1.0f/ (obj->mass > 0 ? obj->mass : PHYSICS_EPSILON);
}

vec3_t object_get_velocity_at_point(physics_object_t* obj, vec3_t point){
	vec3_t r = vec3_sub(point, obj->center_of_mass);
	vec3_t angular_component = vec3_cross(/*obj->angular_vel*/vec3(0, 0, 0), r);
        
	return vec3_add(obj->velocity, angular_component);
}

void solve_constraints(physics_world_t* physics_world){
	for(size_t i = 0; i < physics_world->num_collisions; i++){
		physics_collision_t* col = &((physics_collision_t*)physics_world->collisions_arena.data)[i];
		vec3_t pos_a = col->a->center_of_mass;
		vec3_t pos_b = col->b->center_of_mass;
		vec3_t r_a = vec3_sub(col->contact_point, pos_a);
		vec3_t r_b = vec3_sub(col->contact_point, pos_b);
		vec3_t v_a = object_get_velocity_at_point(col->a, col->contact_point);
		vec3_t v_b = object_get_velocity_at_point(col->b, col->contact_point);
		vec3_t relative_velocity = vec3_sub(v_b, v_a);
		float normal_velocity = vec3_dot(relative_velocity, c->contact_normal);
		vec3_t linear_impulse = vec3_scale(normalImpulse, -1.0f);
		col->a->velocity = vec3_add(col->a->velocity, vec3_scale(linear_impulse, object_get_inverse_mass(col->a)));
		Vec3 normalImpulse = Vec3Scale(c->contact_normal, jn);
		ApplyConstraintImpulse(world, c->bodyA_type, c->bodyA_index, 
		                      , Vec3Cross(rA, Vec3Scale(normalImpulse, -1.0f)));
		ApplyConstraintImpulse(world, c->bodyB_type, c->bodyB_index, 
		                      normalImpulse, Vec3Cross(rB, normalImpulse));
	}
}

void integrate_positions(physics_world_t* physics_world, float dt){
	hashmap_foreach(&physics_world->objects, obj_entry){
		physics_object_t* obj = (physics_object_t*)obj_entry->value_ptr;
		if(obj->is_static) continue;

		// pos += v*dt
		obj->center_of_mass = vec3_add(obj->center_of_mass, vec3_scale(obj->velocity, dt));
	}
}

physics_collision_t* physics_get_current_collisions(physics_world_t* physics_world, size_t* num_collisions){
	*num_collisions = physics_world->num_collisions;
	return physics_world->collisions_arena.data;
}


// -= collision cases =-
// sphere-sphere
size_t collision_sphere_sphere(physics_object_t* a, physics_object_t* b, arena_t* collisions_arena){
	vec3_t normal = vec3_sub(b->center_of_mass, a->center_of_mass);
	float combined_radius = a->as.sphere.radius + b->as.sphere.radius;
	float distance = vec3_length(normal);
	if(distance < combined_radius){
		//collision
		physics_collision_t* collision = arena_allocate(collisions_arena, sizeof(physics_collision_t));
		collision->a = a;
		collision->b = b;
		if(distance > PHYSICS_EPSILON)
			collision->contact_normal = vec3_scale(normal, 1/distance);
		else
			collision->contact_normal = vec3(1, 0, 0);//TODO set this random! this is normal in perfectly linear case
		collision->contact_point = vec3_add(a->center_of_mass, vec3_scale(collision->contact_normal, a->as.sphere.radius));
		collision->penetration = combined_radius - distance;
		return 1; //1 collision
	}
	return 0;
}
