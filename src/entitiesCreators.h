#ifndef __ENTITIES_CREATORS__
#define __ENTITIES_CREATORS__

// TREE
void registryAddTree(Registry* _this, int x, int y, Vector2 scaleV) 
{
	static float phase;
	phase += .3;
	int entityId = registryCreateEntity(_this);
	SpriteComponent spriteComponent = spriteComponentCreate(TEXTURE_TREE, 16, 32, 10, 0, 0, 2.);
	entityAddComponent(entityId, _this, &spriteComponent, COMPONENT_SPRITE);
	TransformComponent transformComponent = {{10, 10}, scaleV, 0};
	entityAddComponent(entityId, _this, &transformComponent, COMPONENT_TRANSFORM);
	CircularMovementComponent cmc = {.phase = phase, .center = {(x) * 100., (y) * 100.}, .radius = 100. };
	entityAddComponent(entityId, _this, &cmc, COMPONENT_CIRCULAR_MOVEMENT);
}
// BULLET
void registryAddBullet(Registry* _this, int x, int y, Vector2 scaleV, Vector2 velocity) 
{
	int entityId = registryCreateEntity(_this);
	
	SpriteComponent spriteComponent = spriteComponentCreate(TEXTURE_BULLET, 4, 4, 10, 0, 0, 2.);
	entityAddComponent(entityId, _this, &spriteComponent, COMPONENT_SPRITE);
	
	TransformComponent transformComponent = {{x, y}, scaleV, 0};
	entityAddComponent(entityId, _this, &transformComponent, COMPONENT_TRANSFORM);

	RigidBodyComponent rigidBodyComponent = {0};
	rigidBodyComponent.velocity = velocity;
	entityAddComponent(entityId, _this, &rigidBodyComponent, COMPONENT_RIGID_BODY);
}

//HELICOPTER
void helicopterCreate(Registry* registry, Vector2 scaleV)
{
	int entityId = registryCreateEntity(registry);
	SpriteComponent spriteComponent = spriteComponentCreate(TEXTURE_CHOPPER, 32, 32, 10, 0, 0, 2.);
	entityAddComponent(entityId, registry, &spriteComponent, COMPONENT_SPRITE);
	TransformComponent transformComponent = {{100, 100}, scaleV, 0};
	entityAddComponent(entityId, registry, &transformComponent, COMPONENT_TRANSFORM);

	entityAddTag(entityId, *registry, TAG_PLAYER);

	KeyboardComponent keyboardComponent = {.velocity = 80.};
	entityAddComponent(entityId, registry, &keyboardComponent, COMPONENT_KEYBOARD_CONTROLLER);

	RigidBodyComponent rigidBodyComponent = {0};
	entityAddComponent(entityId, registry, &rigidBodyComponent, COMPONENT_RIGID_BODY);

	AnimationComponent animationComponent = animationComponentCreate(2, 7, true);
	entityAddComponent(entityId, registry, &animationComponent, COMPONENT_ANIMATION);

	ProjectileEmitterComponent projectileComponent = projectileEmitterCreate((Vector2){100., 0}, 20., 10., 10., true);
	entityAddComponent(entityId, registry, &projectileComponent, COMPONENT_PROJECTILE_EMITTER);
}

// TILEMAP
static void loadCSV(char* filename, int rows, int cols, int matrix[][cols]) {
    FILE* fp;
    char buffer[1024];
    int row = 0, col = 0;

    fp = fopen(filename, "r");

	assert(fp != NULL && "Error: failed to open file");

    while (fgets(buffer, 1024, fp) != NULL && row < rows) {
        char* token = strtok(buffer, ",");
        while (token != NULL && col < cols) {
            matrix[row][col] = atoi(token);
            col++;
            token = strtok(NULL, ",");
        }
        col = 0;
        row++;
    }

    fclose(fp);
}

void tilemapCreate(Registry* registry, Vector2 scaleV)
{
	int cols = 25, rows = 20;
	int tilemap[rows][cols];

	loadCSV("./assets/tilemaps/jungle.map", rows, cols, tilemap);

	for(int y = 0; y < rows; y++)
	{
		for(int x = 0; x < cols; x++)
		{
			int entityId = registryCreateEntity(registry);
			entityAddTag(entityId, *registry, TAG_TILE);

			SDL_Rect srcRect = (SDL_Rect){(tilemap[y][x] % 10) * 32, (tilemap[y][x] / 10) * 32, 32, 32};;
			SDL_Rect destRect = (SDL_Rect){ x * 32, y * 32, 32, 32 };
			
			SpriteComponent spriteComponent = spriteComponentCreate(TEXTURE_TILE_MAP, 32, 32, 0, srcRect.x, srcRect.y, scaleV.x);
			entityAddComponent(entityId, registry, &spriteComponent, COMPONENT_SPRITE);

			Vector2 position = {destRect.x * scaleV.x, destRect.y * scaleV.y};

			TransformComponent transformComponent = {position, scaleV, 0};
			entityAddComponent(entityId, registry, &transformComponent, COMPONENT_TRANSFORM);
		}
	}
}

#endif