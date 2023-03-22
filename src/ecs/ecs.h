#ifndef __ECS_H__
#define __ECS_H__

#ifndef MAX_ENTITIES
#define MAX_ENTITIES 10
#endif

typedef struct 
{
	int Id;
} Entity;

typedef struct
{
	ArrayHeader* entities;
	ArrayHeader* components[COMPONENT_COUNT];
	ArrayHeader* entity2Component[COMPONENT_COUNT];
} Registry;

Entity entityCreate(Registry registry)
{
	Entity* result = arrayAddElement(registry.entities, &(Entity){0});
	result->Id = registry.entities->size - 1;
	return *result;
}

void* entityGetComponent(Entity _this, Registry registry, int componentId)
{
	int *id = arrayGetElementAt(registry.entity2Component[componentId], _this.Id);
	return arrayGetElementAt(registry.components[componentId], *id);
}

void* entityAddComponent(Entity _this, Registry registry, void* component, int componentId)
{
	// Add component to the component array. The index of the new element is the size - 1 of the array
	arrayAddElement(registry.components[componentId], component);
	// Register component as part of the entity
	int componentIndex = registry.components[componentId]->size - 1;
	arrayAddElementAt(registry.entity2Component[componentId], &componentIndex, _this.Id);
	// The size of the component map must be at least as big as the entities array.
	assert(registry.entity2Component[componentId]->size >= registry.entities->size || "the size of the components is smaller than the entities array");
	
	return entityGetComponent(_this, registry, componentId);
}  

#endif