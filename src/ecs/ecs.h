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
	int frameCount;
	ArrayHeader* groups[GROUP_COUNT];
	ArrayHeader* components[COMPONENT_COUNT];
	ArrayHeader* entity2Component[COMPONENT_COUNT];
	ArrayHeader* entitiesPerSystem[SYSTEM_COUNT];
	ArrayHeader* componentSignatures;
	ArrayHeader* tags;
	ArrayHeader* entitiesToDelete;
	Bitset systemInterestSignatures[SYSTEM_COUNT];
	bool isDirty;
} Registry;

int registryCreateEntity(Registry* _this)
{
	for(int i = 0; i < COMPONENT_COUNT; i++)
		_this->entity2Component[i] = arrayAddElementAt(_this->entity2Component[i], &(int){-1},_this->entityCount);
	
	return _this->entityCount++;
}

Bitset registryGetEntitySignature(Registry _this, int entityId)
{
	Bitset returnValue = 0;
	returnValue = *(Bitset *)arrayGetElementAt(_this.componentSignatures, entityId);
	return returnValue;
}

bool entityHasComponent(int entityId, ComponentEnum componentId, Registry registry)
{
	Bitset componentSignature = registryGetEntitySignature(registry, entityId);
	return bitsetIsSet(componentSignature, componentId);
}

void registryDeleteEntity(Registry* registry, int entityId)
{
	registry->isDirty = true;
	// a. delete component at entityId
	for(ComponentEnum componentId = 0; componentId < COMPONENT_COUNT; componentId++)
	{
		if(entityHasComponent(entityId, componentId, *registry))
		{
			int componentIndex = arrayGetElementAtI(registry->entity2Component[componentId], entityId);
			ComponentEnum lastEntityIndex = registry->components[componentId]->size - 1;
			arrayDeleteElement(registry->components[componentId], componentIndex);
			// b. update maping with the new location of component in a and only the location of the component in a.
			for(int mapId = 0; mapId < registry->entity2Component[componentId]->size; mapId++)
			{
				int compMapId = arrayGetElementAtI(registry->entity2Component[componentId], mapId);
				if(compMapId == lastEntityIndex)
				{
					int* oldMapIndex = arrayGetElementAt(registry->entity2Component[componentId], mapId);
					(*oldMapIndex) = componentIndex;
				}
			}
		}
		// c. delete the maping entry at entityId
		arrayDeleteElement(registry->entity2Component[componentId], entityId);
	}

	// d. delete the signature at entityId
	arrayDeleteElement(registry->componentSignatures, entityId);

	// e. decrement the entity count
	registry->entityCount--;
}

void* entityGetComponent(int entityId, Registry registry, ComponentEnum componentId)
{
	int id = arrayGetElementAtI(registry.entity2Component[componentId], entityId);
	return arrayGetElementAt(registry.components[componentId], id);
}

void entityAddTag(int entityId, Registry registry, int tag)
{
	Bitset *returnValue = arrayGetElementAt(registry.tags, entityId);
	bitsetSet(returnValue, tag);
}

bool entityHasTag(int entityId, Registry registry, int tag)
{
	Bitset entityTags = 0;
	entityTags = *(Bitset *)arrayGetElementAt(registry.tags, entityId);
	return bitsetIsSet(entityTags, tag);
}

void* entityAddComponent(int entityId, Registry* registry, void* component, ComponentEnum componentId)
{
	registry->isDirty = true;
	// Add component to the component array. The index of the new element is the size - 1 of the array
	registry->components[componentId] = arrayAddElement(registry->components[componentId], component);
	// Register component as part of the entity
	int componentIndex = registry->components[componentId]->size - 1;
	registry->entity2Component[componentId] = arrayAddElementAt(registry->entity2Component[componentId], &componentIndex, entityId);
	Bitset* signature = arrayGetElementOrCreateAt(&registry->componentSignatures, entityId);
	bitsetSet(signature, componentId);
	// The size of the component map must be at least as big as the entities array.
	return entityGetComponent(entityId, *registry, componentId);
}  

ArrayHeader* systemGetEntities(SystemEnum systemId, Registry registry)
{
	ArrayHeader* returnValue = registry.entitiesPerSystem[systemId];
	return returnValue;
}

bool systemIsInterestedInEntity(SystemEnum systemId, int entityId, Registry registry)
{
	Bitset entitySignature = registryGetEntitySignature(registry, entityId);
	Bitset systemInterestSignature = registry.systemInterestSignatures[systemId];
	return (entitySignature & systemInterestSignature) == systemInterestSignature;
}

void registryCleanSystemArrays(Registry registry)
{
	for(int i = 0; i < SYSTEM_COUNT; i++)
	{
		arrayClear(registry.entitiesPerSystem[i]);
	}
}

void systemAddEntity(SystemEnum systemId, int entityId, Registry* registry)
{
	 registry->entitiesPerSystem[systemId] = arrayAddElement(registry->entitiesPerSystem[systemId], &entityId);
}

void registryUpdate(Registry* registry)
{
	if(!registry->isDirty) return;
	// SORT ENTITIES TO DELETE FROM HIGH TO LOW
	{
		int comp(const int* elm1, const int* elm2) 
		{
			return *elm2 - *elm1;
		}
		if(registry->entitiesToDelete->size > 0)
			qsort(registry->entitiesToDelete->data, registry->entitiesToDelete->size, registry->entitiesToDelete->dataTypeSize, comp);
	}

	if(registry->entitiesToDelete->size > 1)
	{
		loggerWarn("entities: %d, frame: %d", registry->entitiesToDelete->size, registry->frameCount);
	}

	for(int i = 0; i < registry->entitiesToDelete->size; i++)
	{
		int entityId = arrayGetElementAtI(registry->entitiesToDelete, i);
		registryDeleteEntity(registry, entityId);
	}

	arrayClear(registry->entitiesToDelete);

	registryCleanSystemArrays(*registry);

	for(int entityId = 0; entityId < registry->entityCount; entityId++)
	{
		for(SystemEnum systemId = 0; systemId < SYSTEM_COUNT; systemId++)
		{
			if(systemIsInterestedInEntity(systemId, entityId, *registry))
			{
				systemAddEntity(systemId, entityId, registry);
			}
		}
	}
	registry->isDirty = false;
}

Registry registryAddEntityToGroup(Registry _this, int entityId, int groupId)
{
	_this.groups[groupId] = arrayAddElement(_this.groups[groupId], entityId);
	return _this;
}

void entityQueueForDeletion(int entityId, Registry* registry)
{
	registry->isDirty = true;
	registry->entitiesToDelete = arrayAddElement(registry->entitiesToDelete, &entityId);
}
#endif