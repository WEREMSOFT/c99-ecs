#ifndef __ECS_H__
#define __ECS_H__

#ifndef MAX_ENTITIES
#define MAX_ENTITIES 10
#endif

// Entities
typedef struct 
{
	int entities[MAX_ENTITIES];
	int components[MAX_ENTITIES];
	int entitiesToComponentsMapping[MAX_ENTITIES];
} Registry;

typedef struct 
{
	int Id;
} Entity;


#endif