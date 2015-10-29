#include "InputManager.h"
#include "Includes.h"

void
InputManager::clear() {
  for(int i = 0 ; i < 256; i++) {
    keysPressed[i].isPressed = false;
    keysReleased[i] = false;
  }
  for(int i = 0 ; i < 3; i++) {
    buttonsPressed[i] = false;
    buttonsReleased[i] = false;
  }
  prevMousePos = mousePos;
}

void
InputManager::handleKeyPress(const int key, bool isShift, bool isCtrl, bool isAlt)
{
  keysDown[key] = true;
  keysPressed[key].isPressed = true;
  keysPressed[key].isShift = isShift;
  keysPressed[key].isCtrl = isCtrl;
  keysPressed[key].isAlt = isAlt;
}

void
InputManager::handleKeyRelease(const int key) {
  keysDown[key] = false;
  keysReleased[key] = true;

  if(key >= 'A' && key <= 'Z') {
    keysDown[key + ('a' - 'A')] = false;
    keysReleased[key + ('a' - 'A')] = true;
  }
  if(key >= 'a' && key <= 'z') {
    keysDown[key - ('a' - 'A')] = false;
    keysReleased[key - ('a' - 'A')] = true;
  }

}

void
InputManager::handleButtonPress(const int key) {
  buttonsPressed[key] = true;
  buttonsDown[key] = true;
}

void
InputManager::handleButtonRelease(const int key) {
  buttonsReleased[key] = true;
  buttonsDown[key] = false;
}

void
InputManager::handleMouseMove(const Vec2i& position) {
  prevMousePos = mousePos;
  mousePos = position;
}

bool
InputManager::isKeyPressed(const int key, INPUT_MODIFIER inputModifier) const
{
  if(inputModifier == 0) return keysPressed[key].isPressed;

  else if(((inputModifier & IM_SHIFT) > 0) == keysPressed[key].isShift &&
	  ((inputModifier & IM_CTRL) > 0) == keysPressed[key].isCtrl &&
	  ((inputModifier & IM_ALT) > 0) == keysPressed[key].isAlt)
    return keysPressed[key].isPressed;

  else return false;
}

void
InputManager::clearAll() {
  for(int i = 0 ; i < 256; i++) {
    keysDown[i] = false;
    keysPressed[i].isPressed = false;
    keysReleased[i] = false;
  }
  for(int i = 0 ; i < 3; i++) {
    buttonsDown[i] = false;
    buttonsPressed[i] = false;
    buttonsReleased[i] = false;
  }
}
