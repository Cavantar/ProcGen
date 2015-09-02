#include "Camera.h"

glm::mat4* Camera::update(const InputManager& inputManager, long unsigned int& lastDelta) 
{
  viewMatrix = glm::mat4(1.0);
  handleInput(inputManager, lastDelta);

  // std::cout << position.x << " " << position.y << " " << position.z << std::endl;
  return &viewMatrix;
}

void FreeLookCamera::handleInput(const InputManager& inputManager, long unsigned int& lastDelta) 
{
  static float rotationSpeed = 0.10f;
  static float movementSpeed = 0.05f * 5.00f;
  
  if(inputManager.isButtonDown(0)) {
    glm::ivec2 mouseDelta = inputManager.getMouseDelta();
    if(mouseDelta.x != 0) {
      rotation.y += mouseDelta.x * rotationSpeed * lastDelta;
      //cout << rotation.y << endl;
    }	
    if(mouseDelta.y != 0) {
      rotation.x += mouseDelta.y * rotationSpeed * lastDelta;
    }
  }
  
  if(inputManager.isKeyPressed(']')) movementSpeed *= 1.5f;
  if(inputManager.isKeyPressed('[')) movementSpeed *= 2 / 3.0f;
  if(inputManager.isKeyPressed('b')) autoWalk = !autoWalk;
  
  // Rotating around y axis and normalizing
  lookVec = glm::vec3(0, 0, 1.0f);
  lookVec = glm::rotate(lookVec, -rotation.y, glm::vec3(0, 1.0f, 0));
  glm::vec3 rightVec = glm::cross(glm::vec3(0, 1.0f, 0), lookVec);
  
  // Rotating around x axis
  lookVec = glm::rotate(lookVec, -rotation.x, rightVec);
  glm::vec3 upVec = glm::cross(lookVec, rightVec);
  
  // Movement
  if(inputManager.isKeyDown('w'))
  {
    position -= lastDelta * movementSpeed * lookVec;
  }
  if(inputManager.isKeyDown('s'))
  {
    position += lastDelta * movementSpeed * lookVec;
  }
  if(inputManager.isKeyDown('d'))
  {
    position += lastDelta * movementSpeed * rightVec;
  }
  if(inputManager.isKeyDown('a'))
  {
    position -= lastDelta * movementSpeed * rightVec;
  }
  if(inputManager.isKeyDown('q'))
  {
    position -= lastDelta * movementSpeed * upVec;
  }
  if(inputManager.isKeyDown('e'))
  {
    position += lastDelta * movementSpeed * upVec;
  }

  
  if(autoWalk)
  {
    position -= lastDelta * movementSpeed * glm::vec3(0,0,1.0f);
  }
  
  glm::vec4 offset = glm::vec4(-(glm::dot(rightVec, position)),
			       -(glm::dot(upVec, position)),
			       -(glm::dot(lookVec, position)), 1.0f);
  
  viewMatrix[0] = glm::vec4(rightVec, offset.x);
  viewMatrix[1] = glm::vec4(upVec, offset.y);
  viewMatrix[2] = glm::vec4(lookVec, offset.z);
  
  viewMatrix = glm::transpose(viewMatrix);	
}

void FreeLookCamera::setTweakBar(TwBar* bar) 
{
  TwAddVarRW(bar, "AutoWalk", TW_TYPE_BOOLCPP, &autoWalk,
	     " label='AutoWalk' help='Toggle AutoWalk' group='Movement'");
}
