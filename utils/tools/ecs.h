#pragma once
#include "../allocator.h"
#include "../hashmap.h"
#include <stdint.h>
#include <stddef.h>

//forward declaration
typedef struct ecs_t ecs_t;

//types
typedef uint64_t ecs_id_t;
typedef ecs_id_t entity_t;
typedef ecs_id_t component_id_t;
typedef ecs_id_t system_id_t;


typedef struct{
	component_id_t id;
	
	const char* type_name;
	size_t type_size;

	hashmap_t entities;//key: entity_t, value: void* to allocated struct
}component_t;

typedef enum{
	SYSTEM_ON_START,
	SYSTEM_ON_UPDATE,
	SYSTEM_ON_FRAME,
	SYSTEM_ON_SHUTDOWN,

	SYSTEM_PRE_FRAME,
	SYSTEM_PRE_UPDATE,

	SYSTEM_POST_FRAME,
	SYSTEM_POST_UPDATE,

	__SYSTEM_NUM_PHASES,
}system_phase_t;

typedef struct{
	system_id_t id;

	//TODO: add later: bool enabled;

	const char* system_name;

	component_id_t required_component;
	//TODO: move to this later
	//component_id_t* required_components;
	//size_t num_required_components;
	
	system_phase_t call_phase;
	void (*callback_function)(ecs_t*, entity_t, hashmap_t*, void*);//ecs_t* ecs, entity_t entity, hashmap_t* other_entities, void* userdata
}system_t;

struct ecs_t{
	allocator_t* allocator;
	ecs_id_t id_counter;
	
	hashmap_t components;//key: component_id_t, value: component_t
	hashmap_t systems[__SYSTEM_NUM_PHASES];//key: system_id_t, value: system_t

	void* userdata;//global user data public to all systems

	//TODO: add later: hashmap_t entity_component_map;//key: entity_t, value: component_id_t* array
};

ecs_t ecs_create(allocator_t* allocator);
void ecs_destroy(ecs_t* ecs);

void ecs_set_userdata(ecs_t* ecs, void* userdata);

void ecs_start(ecs_t* ecs);
void ecs_update(ecs_t* ecs);
void ecs_frame(ecs_t* ecs);
void ecs_shutdown(ecs_t* ecs);

entity_t ecs_entity_add(ecs_t* ecs);
void ecs_entity_add_component(ecs_t* ecs, entity_t entity, const char* component_name);
void ecs_entity_remove_component(ecs_t* ecs, entity_t entity, const char* component_name);
void ecs_entity_clear_components(ecs_t* ecs, entity_t entity);
void* ecs_entity_get_component(ecs_t* ecs, entity_t entity, const char* component_name);
bool ecs_entity_has_component(ecs_t* ecs, entity_t entity, const char* component_name);

void ecs_register_component(ecs_t* ecs, const char* type_name, size_t type_size);

void ecs_register_system(ecs_t* ecs, const char* system_name, void (*callback_function)(ecs_t*, entity_t, hashmap_t*, void*), system_phase_t call_phase, const char* required_component_name);
void ecs_unregister_system(ecs_t* ecs, const char* system_name, system_phase_t call_phase);



// -- macro api --

#define ECS_REGISTER_COMPONENT(ecs, type) ecs_register_component(ecs, #type, sizeof(type))

#define ECS_REGISTER_SYSTEM(ecs, func, call_phase, required_component) (ecs_register_system(ecs, #func, &(func), call_phase, #required_component))
#define ECS_UNREGISTER_SYSTEM(ecs, func, call_phase) (ecs_unregister_system(ecs, #func, call_phase))

#define ECS_ENTITY_ADD(ecs) (ecs_entity_add(ecs))
#define ECS_ENTITY_ADD_COMPONENT(ecs, entity, type) (ecs_entity_add_component(ecs, entity, #type))
#define ECS_ENTITY_REMOVE_COMPONENT(ecs, entity, type) (ecs_entity_remove_component(ecs, entity, #type))
#define ECS_ENTITY_CLEAR_COMPONENTS(ecs, entity) (ecs_entity_clear_components(ecs, entity))
#define ECS_ENTITY_GET_COMPONENT(ecs, entity, type) ((type *)ecs_entity_get_component(ecs, entity, #type))
#define ECS_ENTITY_HAS_COMPONENT(ecs, entity, type) (ecs_entity_has_component(ecs, entity, #type))
