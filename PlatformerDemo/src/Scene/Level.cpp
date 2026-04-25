#include "Level.h"

#include <stb_image/stb_image.h>
#include <iostream>
#include "Input.h"

Level::Level(std::string levelTexture)
{
	int bytesPerChannel;
	unsigned char* levelData = stbi_load(levelTexture.c_str(), &levelWidth, &levelHeight, &bytesPerChannel, 3); // set to 4 if I wanna go alpha later

	tiles.reserve(levelWidth*levelHeight);
	enemies.reserve(levelWidth);
	for (int x = 0; x < levelWidth; x++)
	{
		for (int y = 0; y < levelHeight; y++)
		{
			unsigned char* pixel = levelData + (levelWidth * y + x) * bytesPerChannel;
			unsigned char r = pixel[0];
			unsigned char g = pixel[1];
			unsigned char b = pixel[2];
			//if alpha: unsigned char a = bytesPerChannel > 3 ? pixel[3] : 0xff;
			unsigned long hex = ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
			//if alpha: ((r & 0xff) << 24) + ((g & 0xff) << 16) + ((b & 0xff) << 8) +(a & 0xff);

			if (hex == 0x00) // black = tile
			{
				tiles.emplace_back(x, levelHeight-y);
			}
			else if (r==255 && g <= 80 && b==0) // red = enemy (g=wandering radius as a float (so divide by 10))
			{
				enemies.emplace_back(x, levelHeight-y, (float)g/10);
			}
			else if (hex == 0x00FF00) // green = player spawn
			{
				spawnX = x;
				spawnY = levelHeight - y;
			}
			else if (hex == 0x0000FF) // blue = level goal
			{
				goalX = x;
				goalY = levelHeight - y;
			}
			else if (hex == 0xFFFF00) // yellow = coin
			{
				//TODO: implement the system
			}
			else if (hex == 0xFF00FF) // pink = checkpoint
			{
				//TODO: implement the system
			}
			//std::cout << std::to_string(r) << "/" << std::to_string(g) << "/" << std::to_string(b) << std::endl;
		}
	}
	player.x = spawnX;
	player.y = spawnY;
	player.quad.SetPosition(glm::vec3(player.x, player.y, 0));
}

bool collides(float x1, float y1, float x2, float y2)
{
	return x1 < x2 + 1.f && x1 + 1.f > x2 && y1 < y2 + 1.f && y1 + 1.f > y2;
}

void Level::runLogic(double deltaTime)
{
	if (!ApplicationWindow::Get().IsFocused() || ApplicationWindow::Get().IsPaused()) return;

	float playerMoveX = 0.f, playerMoveY = player.yVelocity - MathUtil::Lerp(7.f, 15.f, MathUtil::EaseInOutExpo(player.gravityStrength)) * deltaTime;
	// Player mvt
	float mvtSpeed = player.grounded ? 5.f : 4.5f;
	if (Input::IsKeyPressed(GLFW_KEY_A)) playerMoveX -= mvtSpeed;
	if (Input::IsKeyPressed(GLFW_KEY_D)) playerMoveX += mvtSpeed;
	if (Input::IsKeyPressed(GLFW_KEY_SPACE) && player.grounded)
	{
		playerMoveY += 10.f;
		player.grounded = false;
		player.gravityStrength = 0.0f;
	}
	float playerNewX = player.x + playerMoveX * deltaTime;
	float playerNewY = player.y + playerMoveY * deltaTime;

	if (player.gravityStrength > 0.18f) player.grounded = false;
	player.gravityStrength += 3.f * deltaTime;
	player.gravityStrength = std::max(std::min(player.gravityStrength, 1.0f), 0.0f);

	bool dead = false;

	// Player collisions with terrain
	for (const Tile& tile : tiles)
	{
		if (!tile.solid) continue;
		//std::cout << std::to_string(playerNewX) << "//" << std::to_string(playerNewY) << "//" << std::to_string(tile.x) << "//" << std::to_string(tile.y) << std::endl;
		if (collides(playerNewX, player.y, tile.x, tile.y))
		{
			if (playerMoveX > 0.f)
			{
				playerNewX = tile.x - 1.f;
			}
			else if (playerMoveX < 0.f)
			{
				playerNewX = tile.x + 1.f;
			}
		}
		if (collides(playerNewX, playerNewY, tile.x, tile.y))
		{
			if (playerMoveY > 0.f)
			{
				playerNewY = tile.y - 1.f;
				playerMoveY = 0.0f;
			}
			else if (playerMoveY < 0.f)
			{
				player.grounded = true;
				playerNewY = tile.y + 1.f;
				playerMoveY = 0.0f;
			}
		}
	}
	if (playerNewX <= 0.0f) playerNewX = 0.0f;
	else if (playerNewX >= levelWidth-1) playerNewX = levelWidth-1;
	if (player.y < 0.f) dead = true;

	// Enemies mvt left/right + collisions with player
	for (Enemy& enemy : enemies)
	{
		if (!enemy.alive) continue;
		
		if (std::abs(enemy.spawnPosX - enemy.x) >= enemy.wanderingRadius) enemy.dir *= -1.f;
		float enemyMvtSpeed = std::abs(enemy.x - playerNewX) <= 5 ? 1.5f : 1.f;
		float enemyMoveX = enemyMvtSpeed * enemy.dir;
		enemy.x += enemyMoveX * deltaTime;
		enemy.quad.get()->SetPosition(glm::vec3(enemy.x, enemy.y, 0));

		if (collides(playerNewX, playerNewY, enemy.x, enemy.y-0.2f))
		{
			if (playerMoveY < 0.f && std::abs(playerNewX-enemy.x)<0.6f)
			{
				enemy.alive = false;
				player.grounded = false;
				player.gravityStrength = 0.0f;
			}
			else
			{
				dead = true;
			}
		}
	}

	//TODO: coins

	// Goal check
	if (collides(playerNewX, playerNewY, goalX, goalY))
	{
		playerNewY += 10.0f;
		std::cout << "Victoire !²" << std::endl;
	}
	
	if (dead)
	{
		playerNewX = spawnX;
		playerNewY = spawnY;
		playerMoveY = 0.f;
	}

	player.x = playerNewX;
	player.y = playerNewY;
	player.yVelocity = std::max(std::min(playerMoveY, 10.0f), -10.f);
	player.quad.SetPosition(glm::vec3(player.x, player.y, 0));
}

Tile::Tile(int x, int y)
	: x(x), y(y), quad(std::make_shared<Quad>(glm::vec3(x, y, 0)))
{
}
Enemy::Enemy(int spawnX, int spawnY, float wanderingRadius)
	: x(spawnX), y(spawnY), wanderingRadius(wanderingRadius), spawnPosX(spawnX), spawnPosY(spawnY), quad(std::make_shared<Quad>(glm::vec3(spawnX, spawnY, 0)))
{
	dir = Common::RandInt(0, 1) == 0 ? 1.f : -1.f;
}