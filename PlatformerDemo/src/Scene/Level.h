#pragma once

#include <vector>
#include <string>
#include <memory>
#include "Scene/Quad.h"

enum TileType
{
	TerrainDefault = 0,
	Coin = 1,
	Flag = 2
};

struct Tile
{
	int x, y;
	std::shared_ptr<Quad> quad;
	bool solid = true, visible = true;
	Tile(int x, int y);
};

struct Enemy
{
	int spawnPosX, spawnPosY;
	float x, y, wanderingRadius;
	bool alive = true;
	float dir = 0.f;
	std::shared_ptr<Quad> quad;
	Enemy(int spawnX, int spawnY, float wanderingRadius);
};

struct Player
{
	float x = 0, y = 0, yVelocity = 0;
	Quad quad = Quad(glm::vec3(0.0f));;
	bool grounded = true;
	float gravityStrength = 0.0f;
	Player() {}
};

struct Level
{
	int levelWidth, levelHeight;
	std::vector<Tile> tiles;
	std::vector<Enemy> enemies;
	Player player;
	int spawnX, spawnY, goalX, goalY;

	Level(std::string levelTexture);

	void runLogic(double deltaTime);
};