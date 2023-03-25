#ifndef __ECS_H__
#define __ECS_H__

#include "../core/bitset.h"

#ifndef MAX_ENTITIES
#define MAX_ENTITIES 10
#endif

typedef struct 
{
	int id;
} Entity;

typedef struct
{
	ArrayHeader* entities;
	ArrayHeader* components[COMPONENT_COUNT];
	ArrayHeader* entity2Component[COMPONENT_COUNT];
	ArrayHeader* entitiesPerSystem[SYSTEM_COUNT];
	ArrayHeader* componentSignatures;
	Bitset systemInterestSignatures[SYSTEM_COUNT];
} Registry;

Entity entityCreate(Registry registry)
{
	Entity* result = arrayAddElement(registry.entities, &(Entity){0});
	result->id = registry.entities->size - 1;
	return *result;
}

void* entityGetComponent(Entity _this, Registry registry, int componentId)
{
	int *id = arrayGetElementAt(registry.entity2Component[componentId], _this.id);
	return arrayGetElementAt(registry.components[componentId], *id);
}

void* entityAddComponent(Entity _this, Registry registry, void* component, int componentId)
{
	// Add component to the component array. The index of the new element is the size - 1 of the array
	arrayAddElement(registry.components[componentId], component);
	// Register component as part of the entity
	int componentIndex = registry.components[componentId]->size - 1;
	arrayAddElementAt(registry.entity2Component[componentId], &componentIndex, _this.id);
	Bitset* signature = arrayGetElementOrCreateAt(registry.componentSignatures, _this.id);
	*signature = bitsetSet(*signature, componentId);
	// The size of the component map must be at least as big as the entities array.
	assert(registry.entity2Component[componentId]->size >= registry.entities->size || "the size of the components is smaller than the entities array");
	return entityGetComponent(_this, registry, componentId);
}  

Bitset entityGetComponentSignature(Entity _this, Registry registry)
{
	Bitset returnValue = 0;
	returnValue = *(Bitset *)arrayGetElementAt(registry.componentSignatures, _this.id);
	return returnValue;
}

void registryUpdate(Registry _this)
{
	for(int i = 0; i < SYSTEM_COUNT; i++)
	{
		arrayClear(_this.entitiesPerSystem[i]);
	}

	for(int j = 0; j < _this.entities->size; j++)
	{
		Entity *entity = arrayGetElementAt(_this.entities, j);
		Bitset componentSignature = entityGetComponentSignature(*entity, _this);
		for(int systemIndex = 0; systemIndex < SYSTEM_COUNT; systemIndex++)
		{
			// if the entity match the interest signature of the system we add it to the systems entity array;
			if(componentSignature & _this.systemInterestSignatures[systemIndex])
			{
				arrayAddElement(_this.entitiesPerSystem[systemIndex], entity);
			}
		}
	}
}

#endif