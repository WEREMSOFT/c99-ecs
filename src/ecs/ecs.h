#ifndef __ECS_H__
#define __ECS_H__

#ifndef MAX_ENTITIES
#define MAX_ENTITIES 10
#endif

typedef struct 
{
	int Id;
	int components[COMPONENT_COUNT];
} Entity;

typedef struct
{
	ArrayHeader* entities;
	ArrayHeader* components[COMPONENT_COUNT];
} Registry;

Entity entityCreate(Registry registry)
{
	static int id;
	Entity returnValue = { id++ };
	arrayAddElement(registry.entities, &returnValue);
	return returnValue;
}

void* entityAddComponent(Entity _this, Registry registry, void* component, int componentId)
{
	arrayAddElement(registry.components[componentId], component);
	_this.components[componentId] = registry.components[componentId]->size - 1;
	return arrayGetElement(registry.components[componentId], _this.components[componentId]);
}  

void* entityGetComponent(Entity _this, Registry registry, int componentId)
{
	return arrayGetElement(registry.components[componentId], _this.Id);
}

#endif