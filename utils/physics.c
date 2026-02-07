#include "physics.h"
#include "logging.h" //TODO remove when finished. not needed except for "not implemented"

#define PHYSICS_EPSILON (1e-6f)

physics_object_t physics_get_default_sphere(vec3_t pos, float radius, float mass){
	if(mass == 0) mass = PHYSICS_EPSILON;
	return (physics_object_t){
		.center_of_mass = pos,
		.mass = mass,
		.rotation = vec4(0, 0, 0, 1),
		.inertia = vec3((2.0f / 5.0f) * mass * radius * radius, 
						(2.0f / 5.0f) * mass * radius * radius,
						(2.0f / 5.0f) * mass * radius * radius
					),
		.collider_type = PHYSICS_SPHERE,
		.as.sphere = (sphere_collider_t){.radius = radius},
	};
}

physics_object_t physics_get_default_box(vec3_t pos, vec3_t size/*half extents*/, float mass){
	if(mass == 0) mass = PHYSICS_EPSILON;
	return (physics_object_t){
		.center_of_mass = pos,
		.mass = mass,
		.rotation = vec4(0, 0, 0, 1),
		.inertia = vec3((mass/12.0f) * (size.y*size.y + size.z*size.z), 
						(mass/12.0f) * (size.x*size.x + size.z*size.z),
						(mass/12.0f) * (size.x*size.x + size.y*size.y)
					),
		.collider_type = PHYSICS_BOX,
		.as.box = (box_collider_t){.size = size},
	};
}

physics_object_t physics_get_default_plane(vec3_t pos, vec3_t normal){
	return (physics_object_t){
		.center_of_mass = pos,
		.mass = FLT_MAX,
		.collider_type = PHYSICS_PLANE,
		.as.plane = (plane_collider_t){.normal = normal},
		.is_static = true,
	};
}

physics_world_t physics_world_create(allocator_t* allocator){
	return (physics_world_t) {
		.allocator = allocator,
		.objects = hashmap_create(allocator),
		.collisions_arena = arena_create(allocator, 4096, ARENA_LINEAR),
		.num_collisions = 0,
		.gravity = vec3(0, -9.81f, 0),
		.solver_iterations = 8,
		.linear_damping = 0.99f,
		.angular_damping = 0.99f,
		.restitution = 0.0f,
		.friction = 0.3f,
		.baumgarte_bias = 0.2f,
		.allowed_penetration = 0.01f,
		.velocity_threshold = 1.0f,
		.low_energy_threshold = 0.01f,
		.sleep_time_required = 0.5f,
	};/*
	return (physics_world_t) {
		.allocator = allocator,
		.objects = hashmap_create(allocator),
		.collisions_arena = arena_create(allocator, 4096, ARENA_LINEAR),
		.num_collisions = 0,
		.gravity = vec3(0, -9.81f, 0),
		.solver_iterations = 8,
		.linear_damping = 0.98f,
		.angular_damping = 0.98f,
		.restitution = 0.0f,
		.friction = 0.1f,
		.baumgarte_bias = 0.3f,
		.allowed_penetration = 0.01f,
		.velocity_threshold = 0.2f,
		//.sleep_linear_threshold = 0.01f,
		//.sleep_angular_threshold = 0.01f,
		.low_energy_threshold = 0.01f,
		.sleep_time_required = 0.05f,
	};*/
}

void physics_world_destroy(physics_world_t* physics_world){
	hashmap_foreach(&physics_world->objects, object){
		physics_world->allocator->afree(object->value_ptr);
	}
	hashmap_destroy(&physics_world->objects);
	arena_destroy(&physics_world->collisions_arena);
	*physics_world = (physics_world_t){0};
}

static inline void wake_up_object(physics_object_t* obj){
	if(obj->is_sleeping){
		obj->is_sleeping = false;
		obj->sleep_timer = 0;
	}
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
	wake_up_object(object);
	object->force = vec3_add(object->force, force);
}

void physics_object_apply_offset_force(physics_object_t* object, vec3_t force, vec3_t offset_to_center_of_mass){
	wake_up_object(object);
	object->torque = vec3_add(object->torque, vec3_cross(force, offset_to_center_of_mass));
}

void physics_object_apply_torque(physics_object_t* object, vec3_t torque){
	wake_up_object(object);
	object->torque = vec3_add(object->torque, torque);
}

void apply_gravity_and_integrate_velocities(physics_world_t* physics_world, float dt);
void collision_detection(physics_world_t* physics_world);
void solve_constraints(physics_world_t* physics_world, float dt);
void integrate_positions(physics_world_t* physics_world, float dt);
void update_sleep_state(physics_world_t* physics_world, float dt);

void physics_world_step(physics_world_t* physics_world, float dt){
	if (dt <= PHYSICS_EPSILON) return; //TODO check if this is needed
	arena_reset(&physics_world->collisions_arena);
	physics_world->num_collisions = 0;
	apply_gravity_and_integrate_velocities(physics_world, dt);
	collision_detection(physics_world);
	for(size_t i = 0; i < physics_world->solver_iterations; i++){
		solve_constraints(physics_world, dt); 
	}
	integrate_positions(physics_world, dt);
	update_sleep_state(physics_world, dt);
}

void apply_gravity_and_integrate_velocities(physics_world_t* physics_world, float dt){
	hashmap_foreach(&physics_world->objects, obj_entry){
		physics_object_t* obj = (physics_object_t*)obj_entry->value_ptr;
		if(obj->is_static || obj->is_sleeping) continue;

		// - linear
		
		physics_object_apply_force(obj, physics_world->gravity);

		// a = F/m
		// v = a*dt
		// => v += ((F/m)*dt)*linear_damping
		
		obj->velocity = vec3_add(obj->velocity, vec3_scale(vec3_scale(vec3_scale(obj->force, 1/obj->mass), dt), physics_world->linear_damping));

		// - angular

		vec3_t angular_acceleration;
		angular_acceleration.x = obj->inertia.x > PHYSICS_EPSILON ? obj->torque.x / obj->inertia.x : 0;
		angular_acceleration.y = obj->inertia.y > PHYSICS_EPSILON ? obj->torque.y / obj->inertia.y : 0;
		angular_acceleration.z = obj->inertia.z > PHYSICS_EPSILON ? obj->torque.z / obj->inertia.z : 0;
        
        // ω += (α * dt)*angular_damping
        obj->angular_velocity = vec3_add(obj->angular_velocity, vec3_scale(vec3_scale(angular_acceleration, dt), physics_world->angular_damping));
        
		//clear force/torque
		obj->force = vec3(0, 0, 0);
		obj->torque = vec3(0, 0, 0);
	}
}

size_t collision_sphere_sphere(physics_object_t* a, physics_object_t* b, arena_t* collisions_arena);
size_t collision_sphere_plane(physics_object_t* sphere, physics_object_t* plane, arena_t* collisions_arena);
void collision_detection(physics_world_t* physics_world){
	hashmap_foreach(&physics_world->objects, obj_a_entry){
		physics_object_t* obj_a = (physics_object_t*)obj_a_entry->value_ptr;
		hashmap_foreach(&physics_world->objects, obj_b_entry){
			physics_object_t* obj_b = (physics_object_t*)obj_b_entry->value_ptr;
			if(obj_a == obj_b) continue; //prevent self collisions
			if(obj_a->is_static && obj_b->is_static) continue;// TODO figure out if we want this
			if(obj_a->is_sleeping && obj_b->is_sleeping) continue;// TODO figure out if we want this
			if((obj_a->is_static && obj_b->is_sleeping) || (obj_a->is_sleeping && obj_b->is_static)) continue;// TODO figure out if we want this

			if(obj_a->collider_type == PHYSICS_SPHERE && obj_b->collider_type == PHYSICS_SPHERE)
				physics_world->num_collisions += collision_sphere_sphere(obj_a, obj_b, &physics_world->collisions_arena);
			else if(obj_a->collider_type == PHYSICS_SPHERE && obj_b->collider_type == PHYSICS_PLANE)
				physics_world->num_collisions += collision_sphere_plane(obj_a, obj_b, &physics_world->collisions_arena);
			else if(obj_a->collider_type == PHYSICS_PLANE && obj_b->collider_type == PHYSICS_SPHERE)
				physics_world->num_collisions += collision_sphere_plane(obj_b, obj_a, &physics_world->collisions_arena);
			else
				ERROR("collision case not implemented!");
		}
	}
}

static inline float object_get_inverse_mass(physics_object_t* obj){
	if(obj->is_static) return 0.0f;
	else return 1.0f / (obj->mass > 0 ? obj->mass : PHYSICS_EPSILON);
}

static inline vec3_t object_get_inverse_inertia(physics_object_t* obj){
	if(obj->is_static) return vec3(0, 0, 0);
	else return vec3(1.0f / (obj->inertia.x > 0 ? obj->inertia.x : PHYSICS_EPSILON),
					1.0f / (obj->inertia.y > 0 ? obj->inertia.y : PHYSICS_EPSILON),
					1.0f / (obj->inertia.z > 0 ? obj->inertia.z : PHYSICS_EPSILON)
				);
}

vec3_t object_get_velocity_at_point(physics_object_t* obj, vec3_t point){
	vec3_t r = vec3_sub(point, obj->center_of_mass);
	vec3_t angular_component = vec3_cross(obj->angular_velocity, r);
        
	return vec3_add(obj->velocity, angular_component);
}

void object_apply_constraint_impulse(physics_world_t* physics_world, physics_object_t* obj, vec3_t linear_impulse, vec3_t angular_impulse){
	float total_impulse = vec3_length(linear_impulse) + vec3_length(angular_impulse);
	float kin_energy = (total_impulse*total_impulse) / (2.0f*obj->mass);
	if(kin_energy < physics_world->low_energy_threshold) return;

	wake_up_object(obj);

	obj->velocity = vec3_add(obj->velocity, vec3_scale(linear_impulse, object_get_inverse_mass(obj)));

	vec3_t local_angular_impulse = vec4_rotate_vec3(vec4(-obj->rotation.x, -obj->rotation.y, -obj->rotation.z, obj->rotation.w), angular_impulse);
	vec3_t inv_inertia = object_get_inverse_inertia(obj);
	vec3_t local_angular_delta = vec3(
										local_angular_impulse.x * inv_inertia.x, 
										local_angular_impulse.y * inv_inertia.y, 
										local_angular_impulse.z * inv_inertia.z
									);
	vec3_t world_angular_delta = vec4_rotate_vec3(obj->rotation, local_angular_delta);
	obj->angular_velocity = vec3_add(obj->angular_velocity, world_angular_delta);
}

void solve_constraints(physics_world_t* physics_world, float dt){
	LOG("num collisions %zu", physics_world->num_collisions);
	for(size_t i = 0; i < physics_world->num_collisions; i++){
		physics_collision_t* col = &((physics_collision_t*)physics_world->collisions_arena.data)[i];

		vec3_t r_a = vec3_sub(col->contact_point, col->a->center_of_mass);
		vec3_t r_b = vec3_sub(col->contact_point, col->b->center_of_mass);
		vec3_t relative_velocity = vec3_sub(object_get_velocity_at_point(col->b, col->contact_point), object_get_velocity_at_point(col->a, col->contact_point));

		float normal_velocity = vec3_dot(relative_velocity, col->contact_normal);
		float velocity_bias = (normal_velocity < -physics_world->velocity_threshold) ? -physics_world->restitution * normal_velocity : 0.0f;
		//needed: normal_mass, position_bias
		vec3_t rn_a = vec3_cross(r_a, col->contact_normal);
		vec3_t rn_b = vec3_cross(r_b, col->contact_normal);
		float inv_mass_sum = object_get_inverse_mass(col->a) + object_get_inverse_mass(col->b) + 
		                          vec3_dot(rn_a, vec3(rn_a.x * object_get_inverse_inertia(col->a).x, rn_a.y * object_get_inverse_inertia(col->a).y, rn_a.z * object_get_inverse_inertia(col->a).z)) +
		                          vec3_dot(rn_b, vec3(rn_b.x * object_get_inverse_inertia(col->b).x, rn_b.y * object_get_inverse_inertia(col->b).y, rn_b.z * object_get_inverse_inertia(col->b).z));
		float normal_mass = (inv_mass_sum > 0.0f) ? 1.0f / inv_mass_sum : 0.0f;

		float position_bias = physics_world->baumgarte_bias/dt * fmaxf(0.0f, col->penetration - physics_world->allowed_penetration);
		//
		float jn = normal_mass * (-(normal_velocity + velocity_bias) + position_bias);
		float old_normal_impulse = col->normal_impulse;
		col->normal_impulse = fmax(0.0f, col->normal_impulse + jn);
		jn = col->normal_impulse - old_normal_impulse;

		vec3_t normal_impulse = vec3_scale(col->contact_normal, jn);
		object_apply_constraint_impulse(physics_world, col->a, vec3_scale(normal_impulse, -1.0f), vec3_cross(r_a, vec3_scale(normal_impulse, -1.0f)));
		object_apply_constraint_impulse(physics_world, col->b, normal_impulse, vec3_cross(r_b, normal_impulse));

		vec3_t tangent[2];
		if (fabs(col->contact_normal.x) >= 0.57735f)// 1/sqrt(3)
			tangent[0] = vec3(col->contact_normal.y, -col->contact_normal.x, 0.0f);
		else
			tangent[0] = vec3(0.0f, col->contact_normal.z, -col->contact_normal.y);
			   
		tangent[0] = vec3_normalize(tangent[0]);
		tangent[1] = vec3_cross(col->contact_normal, tangent[0]);
		for(uint8_t j = 0; j < 2; j++){
			float tangent_velocity = vec3_dot(relative_velocity, tangent[j]);
			// needed tangent_mass
			vec3_t rt_a = vec3_cross(r_a, tangent[j]);
	        vec3_t rt_b = vec3_cross(r_b, tangent[j]);
	        float tangent_inv_mass_sum = object_get_inverse_mass(col->a) + object_get_inverse_mass(col->b) +
	                                 vec3_dot(rt_a, vec3(rt_a.x * object_get_inverse_inertia(col->a).x, rt_a.y * object_get_inverse_inertia(col->a).y, rt_a.z * object_get_inverse_inertia(col->a).z)) +
	                                 vec3_dot(rt_b, vec3(rt_b.x * object_get_inverse_inertia(col->b).x, rt_b.y * object_get_inverse_inertia(col->b).y, rt_b.z * object_get_inverse_inertia(col->b).z));
	        float tangent_mass = (tangent_inv_mass_sum > 0.0f) ? 1.0f / tangent_inv_mass_sum : 0.0f;
	        
			//
			float jt = tangent_mass * (-tangent_velocity);
			//coulomb friction limit
			float max_friction = physics_world->friction * col->normal_impulse;
			//clamp tangent impulse
			float old_tangent_impulse = col->tangent_impulse[j];
			col->tangent_impulse[j] = fmax(-max_friction, fmin(max_friction, col->tangent_impulse[j] + jt));
			jt = col->tangent_impulse[j] - old_tangent_impulse;
			
			vec3_t tangent_impulse = vec3_scale(tangent[j], jt);
			object_apply_constraint_impulse(physics_world, col->a, vec3_scale(tangent_impulse, -1.0f), vec3_cross(r_a, vec3_scale(tangent_impulse, -1.0f)));
			object_apply_constraint_impulse(physics_world, col->b, tangent_impulse, vec3_cross(r_b, tangent_impulse));
		}
	}
}

void integrate_positions(physics_world_t* physics_world, float dt){
	hashmap_foreach(&physics_world->objects, obj_entry){
		physics_object_t* obj = (physics_object_t*)obj_entry->value_ptr;
		if(obj->is_static || obj->is_sleeping) continue;

		// - linear
		
		// pos += v*dt
		obj->center_of_mass = vec3_add(obj->center_of_mass, vec3_scale(obj->velocity, dt));

		// - angular

		// q += 0.5 * ω * q * dt
        // dq/dt = 0.5 * ω * q
        // q += dq/dt * dt
        vec4_t omega_quat = vec4(obj->angular_velocity.x, obj->angular_velocity.y, obj->angular_velocity.z, 0.0f);
        vec4_t q_dot = vec4_scale(vec4_mul(omega_quat, obj->rotation), 0.5f * dt);
		obj->rotation = vec4_normalize(vec4_add(obj->rotation, q_dot));
	}
}

void update_sleep_state(physics_world_t* physics_world, float dt){
	hashmap_foreach(&physics_world->objects, obj_entry){
		physics_object_t* obj = (physics_object_t*)obj_entry->value_ptr;
		if(obj->is_static) continue;
		//bool low_energy = (vec3_length(obj->velocity) < physics_world->sleep_linear_threshold && vec3_length(obj->angular_velocity) < physics_world->sleep_angular_threshold);
		float kin_energy = 0.5f * obj->mass * (vec3_length(obj->velocity) * vec3_length(obj->velocity));
		if(kin_energy < physics_world->low_energy_threshold){
			obj->sleep_timer += dt;
			if(obj->sleep_timer >= physics_world->sleep_time_required && !obj->is_sleeping){
				obj->is_sleeping = true;
				obj->velocity = vec3(0, 0, 0);
				obj->angular_velocity = vec3(0, 0, 0);
			}
		} else {
			wake_up_object(obj);
		}
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
			collision->contact_normal = vec3(1, 0, 0);//this could be set to random, but this case will never occur since tresholds
		collision->contact_point = vec3_add(a->center_of_mass, vec3_scale(collision->contact_normal, a->as.sphere.radius));
		collision->penetration = combined_radius - distance;

		//must be init with zero
		collision->normal_impulse = 0;
		collision->tangent_impulse[0] = 0;
		collision->tangent_impulse[1] = 0;
		return 1; //1 collision
	}
	return 0;
}

size_t collision_sphere_plane(physics_object_t* sphere, physics_object_t* plane, arena_t* collisions_arena){
	vec3_t normal = vec3_negate(plane->as.plane.normal);
	float distance = fabs(vec3_distance_to_plane(sphere->center_of_mass, plane->center_of_mass, plane->as.plane.normal));
	if(distance < sphere->as.sphere.radius){
		//collision
		physics_collision_t* collision = arena_allocate(collisions_arena, sizeof(physics_collision_t));
		collision->a = sphere;
		collision->b = plane;
		collision->contact_normal = normal;
		collision->contact_point = vec3_add(sphere->center_of_mass, vec3_scale(collision->contact_normal, sphere->as.sphere.radius));
		collision->penetration = sphere->as.sphere.radius - distance;

		//must be init with zero
		collision->normal_impulse = 0;
		collision->tangent_impulse[0] = 0;
		collision->tangent_impulse[1] = 0;
		return 1; //1 collision
	}
	return 0;
}
