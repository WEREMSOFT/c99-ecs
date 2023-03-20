#ifndef __ECS_H__
#define __ECS_H__

#ifndef MAX_ENTITIES
#define MAX_ENTITIES 10
#endif

typedef struct 
{
	int Id;
} Entity;

Entity entityCreate()
{
	static int id;
	Entity returnValue = { id++ };
	return returnValue;
}

typedef struct
{
	ArrayHeader* entities;
	ArrayHeader* components[COMPONENT_COUNT];
} Registry;

void* entityAddComponent(Entity _this, Registry registry, void* component, int componentId)
{
	arrayAddElement(registry.components[componentId], component);
	return arrayGetElement(registry.components[componentId], _this.Id);
}  

void* entityGetComponent(Entity _this, Registry registry, int componentId)
{
	return arrayGetElement(registry.components[componentId], _this.Id);
}

#endif