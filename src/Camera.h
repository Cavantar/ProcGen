#pragma once

#include "Includes.h"
#include "InputManager.h"


class Camera{
public:
  glm::mat4* update(const InputManager& inputManager, long unsigned int& lastDelta);
  glm::vec3 getPosition() const { return position; }
  
  virtual void setTweakBar(TwBar* bar) {}
protected:
  glm::mat4 viewMatrix;
  
  glm::vec3 position = glm::vec3(0, 300.0f, 200.0f);
  glm::vec3 rotation;
  
  bool autoWalk = false;
  
  virtual void handleInput(const InputManager& inputManager, long unsigned int& lastDelta) = 0;
};

class FreeLookCamera : public Camera {
public:
  void setTweakBar(TwBar* bar);
  
private:
  void handleInput(const InputManager& inputManager, long unsigned int& lastDelta);
};

typedef shared_ptr<Camera> CameraPtr;
