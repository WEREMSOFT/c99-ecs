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
	int *id = arrayGetElementAt(registry.entity2Component[componentId], entityId);
	return arrayGetElementAt(registry.components[componentId], *id);
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

int entityDelete(int entityId, Registry* registry)
{
	Bitset componentSignature = entityGetComponentSignature(entityId, *registry);

	// Clean components for entity
	for(int componentId = 0; componentId < COMPONENT_COUNT; componentId++)
	{
		if(bitsetIsSet(componentSignature, componentId))
		{
			int* componentIndex = arrayGetElementAt(registry->entity2Component[componentId], entityId);
			arrayDeleteElement(registry->components[componentId], *componentIndex);

			for(int mapIndex = 0; mapIndex < registry->entity2Component[componentId]->size; mapIndex++)
			{
				int* index = arrayGetElementAt(registry->entity2Component[componentId], mapIndex);
				if(index >= componentIndex)
				{
					index--;
				}
			}
			registry->entity2Component[componentId]->size--;
		}
	}
	// Clean system where entity is processed
	for(int systemId = 0; systemId < SYSTEM_COUNT; systemId++)
	{
		Bitset systemInterestSignature = registry->systemInterestSignatures[systemId];
		// if the entity match the interest signature of the system we add it to the systems entity array;
		if((componentSignature & systemInterestSignature) == systemInterestSignature)
		{
			for(int j = 0; j < registry->entitiesPerSystem[entityId]->size; j++)
			{
				 int* entityIdInSystemGroup = (int *)arrayGetElementAt(registry->entitiesPerSystem[systemId], j);
				 if(*entityIdInSystemGroup >= entityId)
				 {
					*entityIdInSystemGroup--;
				 }
				 registry->entitiesPerSystem[entityId]->size--;
			}
		}	
	}

	return registry->entityCount--;
}

Registry registryUpdate(Registry _this)
{
	for(int i = 0; i < SYSTEM_COUNT; i++)
	{
		arrayClear(_this.entitiesPerSystem[i]);
	}

	for(int entityId = 0; entityId < _this.entityCount; entityId++)
	{
		Bitset componentSignature = entityGetComponentSignature(entityId, _this);
		for(int systemIndex = 0; systemIndex < SYSTEM_COUNT; systemIndex++)
		{
			// if the entity match the interest signature of the system we add it to the systems entity array;
			if((componentSignature & _this.systemInterestSignatures[systemIndex]) == _this.systemInterestSignatures[systemIndex])
			{
				_this.entitiesPerSystem[systemIndex] = arrayAddElement(_this.entitiesPerSystem[systemIndex], &entityId);
			}
		}
	}
	return _this;
}

ArrayHeader* systemGetEntities(int systemId, Registry registry)
{
	ArrayHeader* returnValue = registry.entitiesPerSystem[systemId];
	return returnValue;
}

Registry registryAddEntityToGroup(Registry _this, int entityId, int groupId)
{
	_this.groups[groupId] = arrayAddElement(_this.groups[groupId], entityId);
	return _this;
}
#endif