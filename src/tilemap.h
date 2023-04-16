// TILEMAP
#include <math.h>

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

void tilemapCreateInferno(Registry* registry, Vector2 scaleV)
{
	int cols = 10, rows = 10;
	int tilemap[rows][cols];

	loadCSV("./assets/inferno.csv", rows, cols, tilemap);

	for(int y = 0; y < rows; y++)
	{
		for(int x = 0; x < cols; x++)
		{
			int entityId = registryCreateEntity(registry);
			entityAddTag(entityId, *registry, TAG_TILE);

			float xf = (y + x) * -32 + 400;
			float yf = (x - y) * 32 + 200;
			SDL_Rect srcRect = (SDL_Rect){0, 0, 64, 64};
			SDL_Rect destRect = (SDL_Rect){ xf, yf, 64, 64 };
			
			TextureIdEnum asset = TEXTURE_INFERNO_FLOOR_2;

			switch(tilemap[y][x])
			{
				case 21:
					asset = TEXTURE_INFERNO_FLOOR_1;
					break;
				case 16:
					asset = TEXTURE_INFERNO_FLOOR_2;
					break;
			}

			SpriteComponent spriteComponent = spriteComponentCreate(asset, 64, 64, 0, srcRect.x, srcRect.y, scaleV.x);
			entityAddComponent(entityId, registry, &spriteComponent, COMPONENT_SPRITE);

			Vector2 position = {destRect.x * scaleV.x, destRect.y * scaleV.y};

			TransformComponent transformComponent = {position, scaleV, 0};
			entityAddComponent(entityId, registry, &transformComponent, COMPONENT_TRANSFORM);
		}
	}
}