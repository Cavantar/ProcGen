#pragma once 

#include "Includes.h"

class InputManager {
public:
	InputManager() { clearAll(); }
	
	void clear();
	
	void handleKeyPress(const int key);
	void handleKeyRelease(const int key);

	void handleButtonPress(const int key);
	void handleButtonRelease(const int key);

	void handleMouseMove(const glm::ivec2& position);

	bool isKeyPressed(const int key) const { return keysPressed[key]; }
	bool isKeyDown(const int key) const { return keysDown[key]; }

	bool isButtonPressed(const int key) const { return buttonsPressed[key]; }
	bool isButtonDown(const int key) const { return buttonsDown[key]; }

	glm::ivec2 getMouseDelta() const { return mousePos - prevMousePos; }
	const glm::ivec2& getMousePos() const { return mousePos; }

private:
	bool keysDown[256];
	bool keysPressed[256];
	bool keysReleased[256];

	bool buttonsDown[3];
	bool buttonsPressed[3];
	bool buttonsReleased[3];

	glm::ivec2 mousePos;
	glm::ivec2 prevMousePos;

	void clearAll();


};