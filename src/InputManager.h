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

  void handleMouseMove(const Vec2i& position);

  bool isKeyPressed(const int key) const { return keysPressed[key]; }
  bool isKeyDown(const int key) const { return keysDown[key]; }

  bool isButtonPressed(const int key) const { return buttonsPressed[key]; }
  bool isButtonDown(const int key) const { return buttonsDown[key]; }

  Vec2i getMouseDelta() const { return mousePos - prevMousePos; }
  const Vec2i& getMousePos() const { return mousePos; }

private:
  bool keysDown[256];
  bool keysPressed[256];
  bool keysReleased[256];

  bool buttonsDown[3];
  bool buttonsPressed[3];
  bool buttonsReleased[3];

  Vec2i mousePos;
  Vec2i prevMousePos;

  void clearAll();


};
