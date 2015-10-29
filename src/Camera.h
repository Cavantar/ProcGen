#pragma once

#include "Includes.h"
#include "InputManager.h"

class CameraData {
public:
  CameraData(const Vec3f& cameraPosition, const Vec3f& lookVec) :
    cameraPosition(cameraPosition), lookVec(lookVec) {}
  const Vec3f& cameraPosition;
  const Vec3f& lookVec;
};

class Camera{
public:
  Mat4* update(const InputManager& inputManager, long unsigned int& lastDelta);
  Vec3f getPosition() const { return position; }
  CameraData getCameraData() const { return CameraData(position, lookVec); }

  virtual void setTweakBar(TwBar* bar) {}
protected:
  Mat4 viewMatrix;

  // For Chunk Rendering
  Vec3f lookVec;

  Vec3f position = Vec3f(0, 200.0f, 200.0f);
  Vec3f rotation;

  bool autoWalk = false;

  virtual void handleInput(const InputManager& inputManager, long unsigned int& lastDelta) = 0;
};

class FreeLookCamera : public Camera {
public:
  void setTweakBar(TwBar* bar);

private:
  void handleInput(const InputManager& inputManager, long unsigned int& lastDelta);
};

typedef std::shared_ptr<Camera> CameraPtr;
