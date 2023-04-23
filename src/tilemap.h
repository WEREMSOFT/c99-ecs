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

void tilemapCreate(Game* game, Vector2 scaleV)
{
	int cols = 25, rows = 20;
	int tilemap[rows][cols];

	loadCSV("./assets/tilemaps/jungle.map", rows, cols, tilemap);

	for(int y = 0; y < rows; y++)
	{
		for(int x = 0; x < cols; x++)
		{
			int entityId = registryCreateEntity(&game->registry);
			entityAddTag(entityId, &game->registry, TAG_TILE);

			SDL_Rect srcRect = (SDL_Rect){(tilemap[y][x] % 10) * 32, (tilemap[y][x] / 10) * 32, 32, 32};
			SDL_Rect destRect = (SDL_Rect){ x * 32, y * 32, 32, 32 };
			
			SpriteComponent spriteComponent = spriteComponentCreate(TEXTURE_TILE_MAP, 32, 32, 0, srcRect.x, srcRect.y, scaleV.x);
			entityAddComponent(entityId, &game->registry, &spriteComponent, COMPONENT_SPRITE);

			Vector2 position = {destRect.x * scaleV.x, destRect.y * scaleV.y};

			TransformComponent transformComponent = {position, scaleV, 0};
			entityAddComponent(entityId, &game->registry, &transformComponent, COMPONENT_TRANSFORM);
		}
	}
}

#define MAX_COLS 1000   // maximum number of columns in a matrix

void getMapDimensions(const char* filename, int* rows, int* cols) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: cannot open file %s\n", filename);
        return;
    }

    char line[1000];
    *rows = 0;
    *cols = 0;
    while (fgets(line, sizeof(line), fp) != NULL) {
        // count the number of rows
        (*rows)++;

        // count the number of columns
        int col_count = 0;
        char* token = strtok(line, ",");
        while (token != NULL) {
            col_count++;
            token = strtok(NULL, ",");
        }
        if (col_count > *cols) {
            *cols = col_count;
        }
    }

    fclose(fp);
}

void tilemapCreateInferno(Game* game, Vector2 scaleV)
{
	float scale = 1.;
	int cols = 10, rows = 10;

	getMapDimensions("./assets/inferno.csv", &cols, &rows);

	int tilemap[rows][cols];

	loadCSV("./assets/inferno.csv", rows, cols, tilemap);
	game->mapSize.x = cols * 64;
	game->mapSize.y = rows * 64;
	for(int y = 0; y < rows; y++)
	{
		for(int x = 0; x < cols; x++)
		{
			int entityId = registryCreateEntity(&game->registry);
			entityAddTag(entityId, &game->registry, TAG_TILE);

			float xf = (y - x) * -32 + cols * 30;
			float yf = (x + y) * 32 + rows;
			// SDL_Rect srcRect = (SDL_Rect){0, 0, 64, 64};
			int tileNumber = tilemap[y][x];
			SDL_Rect srcRect = (SDL_Rect){ (tileNumber % 3) * 64, (tileNumber / 3) * 64, 64, 64};
			SDL_Rect destRect = (SDL_Rect){ xf, yf, 64, 64 };
	
			SpriteComponent spriteComponent = spriteComponentCreate(TEXTURE_INFERNO_TILE_MAP, srcRect.w, srcRect.h, 0, srcRect.x, srcRect.y, false);
			entityAddComponent(entityId, &game->registry, &spriteComponent, COMPONENT_SPRITE);

			Vector2 position = {destRect.x * scale, destRect.y * scale};

			TransformComponent transformComponent = {position, scaleV, 0};
			entityAddComponent(entityId, &game->registry, &transformComponent, COMPONENT_TRANSFORM);
		}
	}
}