#include "InputManager.h"
#include "Includes.h"

void InputManager::clear() {
  for(int i = 0 ; i < 256; i++) {
    keysPressed[i] = false;
    keysReleased[i] = false;
  }
  for(int i = 0 ; i < 3; i++) {
    buttonsPressed[i] = false;
    buttonsReleased[i] = false;
  }
  prevMousePos = mousePos;
}
void InputManager::handleKeyPress(const int key) {
  keysDown[key] = true;
  keysPressed[key] = true;
}
void InputManager::handleKeyRelease(const int key) {
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
void InputManager::handleButtonPress(const int key) {
  buttonsPressed[key] = true;
  buttonsDown[key] = true;
}
void InputManager::handleButtonRelease(const int key) {
  buttonsReleased[key] = true;
  buttonsDown[key] = false;
}

void InputManager::handleMouseMove(const Vec2i& position) {
  prevMousePos = mousePos;
  mousePos = position;
}

void InputManager::clearAll() {
  for(int i = 0 ; i < 256; i++) {
    keysDown[i] = false;
    keysPressed[i] = false;
    keysReleased[i] = false;
  }
  for(int i = 0 ; i < 3; i++) {
    buttonsDown[i] = false;
    buttonsPressed[i] = false;
    buttonsReleased[i] = false;
  }
}
