#include "Window.h"

void
Window::initializeWindow(const Vec2i dimensions, const std::string caption, const Vec2i position)
{
  GLenum GlewInitResult;

  glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG | GLUT_CORE_PROFILE);
  glutInitContextProfile(GLUT_CORE_PROFILE);

  glutSetOption(
    GLUT_ACTION_ON_WINDOW_CLOSE,
    GLUT_ACTION_GLUTMAINLOOP_RETURNS
		);

  glutInitWindowSize(dimensions.x, dimensions.y);
  glutInitWindowPosition(position.x, position.y);

  //glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB);

  windowHandle = glutCreateWindow(caption.c_str());
  this->caption = caption;

  glutSetWindowData((void *) this);

  if(windowHandle < 1) {
    fprintf(
      stderr,
      "ERROR: Could not create a new rendering window.\n"
	    );
    exit(EXIT_FAILURE);
  }

  glutReshapeFunc(resizeFunction);
  glutDisplayFunc(renderFunction);

  glutKeyboardFunc(keyboardFunction);
  glutKeyboardUpFunc(keyboardUpFunction);

  glutMouseFunc(mouseFunction);
  glutPassiveMotionFunc(mouseMove);
  glutMotionFunc(mouseMove);

  glewExperimental = GL_TRUE;
  GlewInitResult = glewInit();

  if(GLEW_OK != GlewInitResult) {
    fprintf(
      stderr,
      "ERROR: %s\n",
      glewGetErrorString(GlewInitResult)
	    );
    exit(EXIT_FAILURE);
  }


  std::cout << "Using glew " << glewGetString(GLEW_VERSION) << std::endl;
  std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
  std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "GLSL:" << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl << std::endl;;

  if(windowHandle == 1)  glClearColor(78.0f / 255.0f, 149.0f / 255.0f, 199.0f / 255.0f, 0.0f);
  else glClearColor(256.0f, 256.0f, 256.0f, 0);
}

void
Window::resizeFunction(int Width, int Height) {
  glViewport(0, 0, Width, Height);
}

void
Window::renderFunction() {
  ((Window*)glutGetWindowData())->myRenderFunction();
  ((Window*)glutGetWindowData())->inputManager.clear();
  ((Window*)glutGetWindowData())->fpsUpdate();

  glutSwapBuffers();
  glutPostRedisplay();
}

void
Window::fpsUpdate()
{
  static int fps = 0;

  static int time = 0;
  static int lastTime = glutGet(GLUT_ELAPSED_TIME);

  while(glutGet(GLUT_ELAPSED_TIME) - lastTime < 1);

  lastDelta = glutGet(GLUT_ELAPSED_TIME) - lastTime;
  time += lastDelta;
  lastTime = glutGet(GLUT_ELAPSED_TIME);
  if(time > 1000) {
    time = time % 1000;
    std::stringstream temp;
    temp << caption << ": " << fps;
    glutSetWindowTitle(temp.str().c_str());
    fps = 0;
  }
  fps++;
}

void
Window::keyboardFunction(unsigned char key, int x, int y)
{
  if(key == 27) glutLeaveMainLoop();
  if(!TwEventKeyboardGLUT(key,x,y))
  {
    int modifiers = glutGetModifiers();
    bool isShift = (modifiers & GLUT_ACTIVE_SHIFT) > 0;
    bool isCtrl = (modifiers & GLUT_ACTIVE_CTRL) > 0;
    bool isAlt = (modifiers & GLUT_ACTIVE_ALT) > 0;

    ((Window*)glutGetWindowData())->inputManager.handleKeyPress(key,
								isShift, isCtrl, isAlt);
  }
}

void
Window::keyboardUpFunction(unsigned char key, int x, int y)
{
  ((Window*)glutGetWindowData())->inputManager.handleKeyRelease(key);
}

void
Window::mouseFunction(int button, int state, int x, int y)
{
  if(!TwEventMouseButtonGLUT(button, state, x, y)){
    if(state == GLUT_DOWN)
      ((Window*)glutGetWindowData())->inputManager.handleButtonPress(button);
    else ((Window*)glutGetWindowData())->inputManager.handleButtonRelease(button);
  }
}

void
Window::mouseMove(int x, int y)
{
  if(!TwEventMouseMotionGLUT(x,y))
    ((Window*)glutGetWindowData())->inputManager.handleMouseMove(Vec2i(x, y));
}
