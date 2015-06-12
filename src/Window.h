#pragma once 

#include "Includes.h"
#include "InputManager.h"

class Window {
public:
  unsigned int windowHandle;
  InputManager inputManager;
  
  long unsigned int lastDelta;
  string caption;
  
  void initializeWindow(const glm::ivec2 dimensions, const string caption = "", const glm::ivec2 position = glm::ivec2());
  void start() { glutMainLoop(); }
private:
  void fpsUpdate();
  static void resizeFunction(int Width, int Height);
  static void renderFunction();
  
  static void keyboardFunction(unsigned char key, int x, int y);
  static void keyboardUpFunction(unsigned char key, int x, int y);
  
  static void mouseFunction(int button, int state, int x, int y);
  
  static void mouseMove(int x, int y);
  
  virtual void myRenderFunction() = 0;
};
