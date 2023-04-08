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
	bool isDirty;
} Registry;

int entityCreate(Registry* registry)
{
	return registry->entityCount++;
}

Bitset entityGetComponentSignature(int entityId, Registry registry)
{
	Bitset returnValue = 0;
	returnValue = *(Bitset *)arrayGetElementAt(registry.componentSignatures, entityId);
	return returnValue;
}

bool entityHasComponent(int entityId, ComponentEnum componentId, Registry registry)
{
	Bitset componentSignature = entityGetComponentSignature(entityId, registry);
	return bitsetIsSet(componentSignature, componentId);
}

void entityDelete(int entityId, Registry* registry)
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

void* entityAddComponent(int entityId, Registry* registry, void* component, ComponentEnum componentId)
{
	registry->isDirty = true;
	// Add component to the component array. The index of the new element is the size - 1 of the array
	registry->components[componentId] = arrayAddElement(registry->components[componentId], component);
	// Register component as part of the entity
	int componentIndex = registry->components[componentId]->size - 1;
	registry->entity2Component[componentId] = arrayAddElementAt(registry->entity2Component[componentId], &componentIndex, entityId);
	Bitset* signature = arrayGetElementOrCreateAt(registry->componentSignatures, entityId);
	*signature = bitsetSet(*signature, componentId);
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
	Bitset componentSignature = entityGetComponentSignature(entityId, registry);
	Bitset systemInterestSignature = registry.systemInterestSignatures[systemId];
	return (componentSignature & systemInterestSignature) == systemInterestSignature;
}

void registryCleanSystemArrays(Registry registry)
{
	for(int i = 0; i < SYSTEM_COUNT; i++)
	{
		arrayClear(registry.entitiesPerSystem[i]);
	}
}

void systemAddEntity(SystemEnum systemId, int entityId, Registry registry)
{
	 registry.entitiesPerSystem[systemId] = arrayAddElement(registry.entitiesPerSystem[systemId], &entityId);
}

void registryUpdate(Registry* registry)
{
	if(!registry->isDirty) return registry;
	registryCleanSystemArrays(*registry);

	for(int entityId = 0; entityId < registry->entityCount; entityId++)
	{
		for(SystemEnum systemId = 0; systemId < SYSTEM_COUNT; systemId++)
		{
			if(systemIsInterestedInEntity(systemId, entityId, *registry))
			{
				systemAddEntity(systemId, entityId, *registry);
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
#endif