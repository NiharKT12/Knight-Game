#pragma once
#include <raylib.h>
class Button
{
public:
	void setPosition(Vector2 position);
	void draw(const char* text);
	bool checkCollision(Vector2 mousePos, bool mousePressed);
	void update();
private:
	Vector2 position;
	bool goingUp = true;
	float hoverOffset = 0;
};
