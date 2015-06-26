#pragma once

#include "Includes.h"
#include "InputManager.h"


class CameraData {
public:
  CameraData(const glm::vec3& cameraPosition, const glm::vec3& lookVec) :
    cameraPosition(cameraPosition), lookVec(lookVec) {}
  const glm::vec3& cameraPosition;
  const glm::vec3& lookVec;
};

class Camera{
public:
  glm::mat4* update(const InputManager& inputManager, long unsigned int& lastDelta);
  glm::vec3 getPosition() const { return position; }
  CameraData getCameraData() const { return CameraData(position, lookVec); }
  
  virtual void setTweakBar(TwBar* bar) {}
protected:
  glm::mat4 viewMatrix;
  
  // For Chunk Rendering
  glm::vec3 lookVec;
  
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
