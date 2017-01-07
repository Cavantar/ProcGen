#include "Camera.h"

// (min(floor(Map1 + 0.9), 1.0) * 0.1) + Map2

Mat4* Camera::update(const InputManager& inputManager, long unsigned int& lastDelta)
{
  viewMatrix = Mat4();
  handleInput(inputManager, lastDelta);

  // std::cout << position.x << " " << position.y << " " << position.z << std::endl;
  return &viewMatrix;
}

void FreeLookCamera::handleInput(const InputManager& inputManager, long unsigned int& lastDelta)
{
  static float rotationSpeed = 0.10f;
  static float movementSpeed = 0.05f * 5.00f;

  if(inputManager.isButtonDown(0))
  {
    Vec2i mouseDelta = inputManager.getMouseDelta();
    rotation.y += mouseDelta.x * rotationSpeed * lastDelta;
    rotation.x += mouseDelta.y * rotationSpeed * lastDelta;
  }

  if(inputManager.isKeyPressed(']')) movementSpeed *= 1.5f;
  if(inputManager.isKeyPressed('[')) movementSpeed *= 2 / 3.0f;
  if(inputManager.isKeyPressed('b')) autoWalk = !autoWalk;

  // Rotating around y axis and normalizinge
  lookVec = Vec3f(0, 0, 1.0f);
  lookVec = Vec3f::rotateAround(lookVec, -rotation.y, Vec3f(0, 1.0f, 0));
  Vec3f rightVec = Vec3f::cross(Vec3f(0, 1.0f, 0), lookVec);

  // Rotating around x axis
  lookVec = Vec3f::rotateAround(lookVec, -rotation.x, rightVec);
  Vec3f upVec = Vec3f::cross(lookVec, rightVec);

  // Movement
  if(inputManager.isKeyDown('w'))
  {
    position -= lookVec * lastDelta * movementSpeed ;
  }
  if(inputManager.isKeyDown('s'))
  {
    position += lookVec * lastDelta * movementSpeed;
  }
  if(inputManager.isKeyDown('d'))
  {
    position += rightVec * lastDelta * movementSpeed;
  }
  if(inputManager.isKeyDown('a'))
  {
    position -= rightVec * lastDelta * movementSpeed;
  }
  if(inputManager.isKeyDown('q'))
  {
    position -= upVec * lastDelta * movementSpeed;
  }
  if(inputManager.isKeyDown('e'))
  {
    position += upVec * lastDelta * movementSpeed;
  }

  if(autoWalk)
  {
    position -= Vec3f(0,0,1.0f) * lastDelta * movementSpeed;
  }

  Vec4f offset = Vec4f(-(Vec3f::dotProduct(rightVec, position)),
		       -(Vec3f::dotProduct(upVec, position)),
		       -(Vec3f::dotProduct(lookVec, position)), 1.0f);

  viewMatrix.m[0] = Vec4f(rightVec.x, rightVec.y, rightVec.z, offset.x);
  viewMatrix.m[1] = Vec4f(upVec.x, upVec.y, upVec.z, offset.y);
  viewMatrix.m[2] = Vec4f(lookVec.x, lookVec.y, lookVec.z, offset.z);
  viewMatrix.m[3] = Vec4f(0, 0, 0, 1.0f);

  viewMatrix = Mat4::transpose(viewMatrix);
}

void FreeLookCamera::setTweakBar(TwBar* bar)
{
  TwAddVarRW(bar, "AutoWalk", TW_TYPE_BOOLCPP, &autoWalk,
	     " label='AutoWalk' help='Toggle AutoWalk' group='TerrainGen'");
}
