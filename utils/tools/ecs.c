#include "ecs.h"
#include "../logging.h"
#include "../random.h"

#include <string.h>

ecs_id_t __ecs_unique_id(ecs_t* ecs){
	return ecs->id_counter++;
}

ecs_id_t __ecs_string_to_id(const char* string){
	return random_hash_from_string(string);
}

ecs_t ecs_create(allocator_t* allocator){
	ecs_t ecs = (ecs_t){
		.allocator = allocator,
		.components = hashmap_create(ecs.allocator),
	};

	for(size_t i = 0; i < __SYSTEM_NUM_PHASES; i++){
		ecs.systems[i] = hashmap_create(ecs.allocator);
	}
	
	return ecs;
}

void ecs_destroy(ecs_t* ecs){
	// free component structs
	hashmap_foreach(&ecs->components, component){ 
		hashmap_foreach(&((component_t*)component->value_ptr)->entities, entity){
			ecs->allocator->afree(entity->value_ptr);
		}
		hashmap_destroy(&((component_t*)component->value_ptr)->entities);

		if(component->value_ptr != NULL){
			ecs->allocator->afree(component->value_ptr);
		}
	}
	// free component list
	hashmap_destroy(&ecs->components);
	// free systems
	for(size_t i = 0; i < __SYSTEM_NUM_PHASES; i++){
		hashmap_foreach(&ecs->systems[i], system){
			if(system->value_ptr != NULL)
				ecs->allocator->afree(system->value_ptr);
		}
	}
	// free system hashmap
	for(size_t i = 0; i < __SYSTEM_NUM_PHASES; i++){
		hashmap_destroy(&ecs->systems[i]);
	}

	
	*ecs = (ecs_t){0};
}

void ecs_set_userdata(ecs_t* ecs, void* userdata){
	ecs->userdata = userdata;
}

void __update_system(ecs_t* ecs, system_phase_t call_phase){
	hashmap_foreach(&ecs->systems[call_phase], system){
		system_t* system_ptr = ((system_t*)system->value_ptr);
		component_t* component = hashmap_get(&ecs->components, system_ptr->required_component);
		hashmap_foreach(&component->entities, entity){
			system_ptr->callback_function(ecs, entity->key, &component->entities, ecs->userdata);
		}
	}
}

void ecs_start(ecs_t* ecs){
	__update_system(ecs, SYSTEM_ON_START);
}

void ecs_update(ecs_t* ecs){
	__update_system(ecs, SYSTEM_PRE_UPDATE);
	__update_system(ecs, SYSTEM_ON_UPDATE);
	__update_system(ecs, SYSTEM_POST_UPDATE);
}

void ecs_frame(ecs_t* ecs){
	__update_system(ecs, SYSTEM_PRE_FRAME);
	__update_system(ecs, SYSTEM_ON_FRAME);
	__update_system(ecs, SYSTEM_POST_FRAME);
}

void ecs_shutdown(ecs_t* ecs){
	__update_system(ecs, SYSTEM_ON_SHUTDOWN);
}

entity_t ecs_entity_add(ecs_t* ecs){
	return __ecs_unique_id(ecs);
}

void ecs_entity_add_component(ecs_t* ecs, entity_t entity, const char* component_name){
	component_id_t component_id = __ecs_string_to_id(component_name);
	component_t* component = hashmap_get(&ecs->components, component_id);
	if(component){
		if(hashmap_contains_key(&component->entities, entity)){
			ERROR("Could not add component %s since it was already added to this entity %lu", component_name, entity);
			return;
		}
		void* component_data = ecs->allocator->amalloc(component->type_size);
		memset(component_data, 0, component->type_size);
		hashmap_set(&component->entities, (hashmap_key_t)entity, component_data);
		return;
	}

	ERROR("Could not add component %s since it was not registered", component_name);
}

void ecs_entity_remove_component(ecs_t* ecs, entity_t entity, const char* component_name){
	component_id_t component_id = __ecs_string_to_id(component_name);
	component_t* component = hashmap_get(&ecs->components, component_id);
	if(component != NULL){
		void* struct_data = hashmap_pop(&component->entities, entity);
		if(struct_data != NULL)
			ecs->allocator->afree(struct_data);
	}
}

void ecs_entity_clear_components(ecs_t* ecs, entity_t entity){
	hashmap_foreach_reverse(&ecs->components, component){
		void* struct_data = hashmap_pop(&((component_t*)component->value_ptr)->entities, entity);
		if(struct_data != NULL)
			ecs->allocator->afree(struct_data);
	}
}


void* ecs_entity_get_component(ecs_t* ecs, entity_t entity, const char* component_name){
	component_id_t component_id = __ecs_string_to_id(component_name);
	component_t* component = hashmap_get(&ecs->components, component_id);
	if(component != NULL){
		return hashmap_get(&component->entities, entity);
	}

	return NULL;
}

bool ecs_entity_has_component(ecs_t* ecs, entity_t entity, const char* component_name){
	if(ecs_entity_get_component(ecs, entity, component_name) == NULL)
		return false;
	return true;
}

void ecs_register_component(ecs_t* ecs, const char* type_name, size_t type_size){
	component_t* component = ecs->allocator->amalloc(sizeof(component_t));

	component->id = __ecs_string_to_id(type_name);
	component->type_name = type_name;
	component->type_size = type_size;
	component->entities = hashmap_create(ecs->allocator);

	hashmap_set(&ecs->components, component->id, component);
}

void ecs_register_system(ecs_t* ecs, const char* system_name, void (*callback_function)(ecs_t*, entity_t, hashmap_t*, void*), system_phase_t call_phase, const char* required_component_name){
	system_t* new_system = ecs->allocator->amalloc(sizeof(system_t));

	new_system->id = __ecs_unique_id(ecs);
	new_system->system_name = system_name;
	new_system->required_component = __ecs_string_to_id(required_component_name);
	new_system->call_phase = call_phase;
	new_system->callback_function = callback_function;
	
	hashmap_set(&ecs->systems[call_phase], new_system->id, new_system);
}

void ecs_unregister_system(ecs_t* ecs, const char* system_name, system_phase_t call_phase){
	system_t* system = hashmap_pop(&ecs->systems[call_phase], __ecs_string_to_id(system_name));
	if(system != NULL){
		ecs->allocator->afree(system);
	} else{
		ERROR("Could not unregister system (%s)", system_name);
	}
}
