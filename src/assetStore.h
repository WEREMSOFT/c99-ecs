#ifndef __ASSET_STORE_H__
#define __ASSET_STORE_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef enum
{
	TEXTURE_TANK_IMAGE,
	TEXTURE_TRUCK_IMAGE,
	TEXTURE_CHOPPER_IMAGE,
	TEXTURE_TILEMAP_IMAGE,
	TEXTURE_RADAR_IMAGE,
	TEXTURE_BULLET_IMAGE,
	TEXTURE_TREE_IMAGE,
	TEXTURE_COUNT,
	TEXTURE_NONE
} TextureIdEnum;

typedef enum 
{
	FONT_MAIN,
	FONT_MAIN_SMALL,
	FONT_COUNT
} FontIdEnum;

typedef struct
{
	SDL_Texture* textures[TEXTURE_COUNT];
	TTF_Font* fonts[(unsigned int)FONT_COUNT];
} AssetStore;

#include <SDL2/SDL_image.h>

AssetStore assetStoreClearAssets(AssetStore _this)
{
	for(int i = 0; i < TEXTURE_COUNT; i++)
	{
		SDL_DestroyTexture(_this.textures[i]);
	}

	for(int i = 0; i < FONT_COUNT; i++)
	{
		TTF_CloseFont(_this.fonts[i]);
	}
	return _this;
}

void assetStoreDestroy(AssetStore _this)
{
	_this = assetStoreClearAssets(_this);
}

AssetStore assetStoreAddTexture(AssetStore _this, SDL_Renderer* renderer, TextureIdEnum assetId, const char* filePath)
{
	SDL_Surface* surface = IMG_Load(filePath);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	_this.textures[(int)assetId] = texture;
	return _this;
}

SDL_Texture* assetStoreGetTexture(AssetStore _this, TextureIdEnum assetId)
{
	return _this.textures[(int)assetId];
}

AssetStore assetStoreAddFont(AssetStore _this, FontIdEnum assetId, char* filePath, int fontSize)
{
	_this.fonts[(int)assetId] = TTF_OpenFont(filePath, fontSize);
	return _this;
}

TTF_Font* assetStoreGetFont(AssetStore _this, const FontIdEnum assetId)
{
	return _this.fonts[(int)assetId];
}

#endif