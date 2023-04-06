#ifndef __ECS_H__
#define __ECS_H__

#include "../core/bitset.h"
#include "../core/array.h"

#ifndef MAX_ENTITIES
#define MAX_ENTITIES 10
#endif

typedef struct
{
	int entityCount;
	ArrayHeader* groups[GROUP_COUNT];
	ArrayHeader* components[COMPONENT_COUNT];
	ArrayHeader* entity2Component[COMPONENT_COUNT];
	ArrayHeader* entitiesPerSystem[SYSTEM_COUNT];
	ArrayHeader* componentSignatures;
	ArrayHeader* tags;
	Bitset systemInterestSignatures[SYSTEM_COUNT];
} Registry;

int entityCreate(Registry* registry)
{
	return registry->entityCount++;
}

void* entityGetComponent(int entityId, Registry registry, int componentId)
{
	int id = arrayGetElementAtI(registry.entity2Component[componentId], entityId);
	return arrayGetElementAt(registry.components[componentId], id);
}

Bitset entityAddTag(int entityId, Registry registry, int tag)
{
	Bitset *returnValue = arrayGetElementAt(registry.tags, entityId);
	*returnValue = bitsetSet(*returnValue, tag);
	return *returnValue;
}

bool entityHasTag(int entityId, Registry registry, int tag)
{
	Bitset entityTags = 0;
	entityTags = *(Bitset *)arrayGetElementAt(registry.tags, entityId);
	return bitsetIsSet(entityTags, tag);
}

void* entityAddComponent(int entityId, Registry* registry, void* component, int componentId)
{
	// Add component to the component array. The index of the new element is the size - 1 of the array
	registry->components[componentId] = arrayAddElement(registry->components[componentId], component);
	// Register component as part of the entity
	int componentIndex = registry->components[componentId]->size - 1;
	arrayAddElementAt(registry->entity2Component[componentId], &componentIndex, entityId);
	Bitset* signature = arrayGetElementOrCreateAt(registry->componentSignatures, entityId);
	*signature = bitsetSet(*signature, componentId);
	// The size of the component map must be at least as big as the entities array.
	assert(registry->entity2Component[componentId]->size >= registry->entityCount || "the size of the components is smaller than the entities array");
	return entityGetComponent(entityId, *registry, componentId);
}  

Bitset entityGetComponentSignature(int entityId, Registry registry)
{
	Bitset returnValue = 0;
	returnValue = *(Bitset *)arrayGetElementAt(registry.componentSignatures, entityId);
	return returnValue;
}

ArrayHeader* systemGetEntities(int systemId, Registry registry)
{
	ArrayHeader* returnValue = registry.entitiesPerSystem[systemId];
	return returnValue;
}

bool entityHasComponent(int entityId, int componentId, Registry registry)
{
	Bitset componentSignature = entityGetComponentSignature(entityId, registry);
	return bitsetIsSet(componentSignature, componentId);
}

bool systemIsInterestedInEntity(int systemId, int entityId, Registry registry)
{
	Bitset componentSignature = entityGetComponentSignature(entityId, registry);
	Bitset systemInterestSignature = registry.systemInterestSignatures[systemId];
	return (componentSignature & systemInterestSignature) == systemInterestSignature;
}

#define PRINT_COMPONENT_ARRAY \
	{\
		ArrayHeader* entities = systemGetEntities(SYSTEM_CIRCULAR_MOVEMENT, registry);\
		for(int i = 0; i < entities->size; i++)\
		{\
			int entityId = arrayGetElementAtI(entities, i);\
			int circularComponentId = arrayGetElementAtI(registry.entity2Component[COMPONENT_CIRCULAR_MOVEMENT], entityId);\
			loggerWarn("entity at %d: %d. Component Id: %d", i, entityId, circularComponentId);\
		}\
		loggerErr("cantidad de entities en el sistema de circular mov %d", entities->size);\
	}\

void entityRemoveComponent(int entityId, int componentId, Registry registry)
{
	int componentIndex = arrayGetElementAtI(registry.entity2Component[componentId], entityId);
	arrayDeleteElement(registry.components[componentId], componentIndex);
}

void entityToComponentCorrectIndex(ArrayHeader* entityToComponent, int componentId)
{
	for(int i = 0; i < entityToComponent->size; i ++)
	{
		int* index = arrayGetElementAt(entityToComponent, i);
		if(*index >= componentId)
		{
			*index -= 1;
		}
	}
}

void registryDeleteComponentsForEntity(Registry registry, int entityId)
{
	for(int componentId = 0; componentId < COMPONENT_COUNT; componentId++)
	{
		if(entityHasComponent(entityId, componentId, registry))
		{
			int componentIndex = arrayGetElementAtI(registry.entity2Component[componentId], entityId);
			arrayDeleteElement(registry.components[componentId], componentIndex);
			for(int mapIndex = 0; mapIndex < registry.entity2Component[componentId]->size; mapIndex++)
			{
				int* index = arrayGetElementAt(registry.entity2Component[componentId], mapIndex);
				if(*index > componentIndex)
					(*index)--;
			}

		}
	}
}

Registry registryDeleteEntity(Registry registry, int entityId)
{
	registryDeleteComponentsForEntity(registry, entityId);

	for(int componentId = 0; componentId < COMPONENT_COUNT; componentId++)
	{
		arrayDeleteElement(registry.entity2Component[componentId], entityId);
	}
	
	

	arrayDeleteElement(registry.componentSignatures, entityId);

	registry.entityCount--;
	return registry;
}

void registryCleanSystemArrays(Registry registry)
{
	for(int i = 0; i < SYSTEM_COUNT; i++)
	{
		arrayClear(registry.entitiesPerSystem[i]);
	}
}

void systemAddEntity(int systemId, int entityId, Registry registry)
{
	 registry.entitiesPerSystem[systemId] = arrayAddElement(registry.entitiesPerSystem[systemId], &entityId);
}

Registry registryUpdate(Registry registry)
{
	registryCleanSystemArrays(registry);

	for(int entityId = 0; entityId < registry.entityCount; entityId++)
	{
		for(int systemId = 0; systemId < SYSTEM_COUNT; systemId++)
		{
			if(systemIsInterestedInEntity(systemId, entityId, registry))
			{
				systemAddEntity(systemId, entityId, registry);
			}
		}
	}
	return registry;
}

Registry registryAddEntityToGroup(Registry _this, int entityId, int groupId)
{
	_this.groups[groupId] = arrayAddElement(_this.groups[groupId], entityId);
	return _this;
}
#endif