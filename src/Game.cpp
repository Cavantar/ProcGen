#include <jpb/Noise.h>
#include <jpb/Profiler.h>
#include "Game.h"
#include "TerrainGenerator.h"

void Game::setupAndStart()
{
  srand(((unsigned int)time(NULL)));

  // Window Things
  bool fullHD = true;
  mainWindowSize = fullHD ? glm::ivec2(1280, 720) : glm::ivec2(1000, 600);
  initializeWindow(mainWindowSize, "ProcGen", glm::ivec2(fullHD ? 1920 : 0, 0));

  HWND consoleWindow = GetConsoleWindow();
  MoveWindow(consoleWindow,
	     0, mainWindowSize.y + 30, 1000, (fullHD ? 1080 : 720) - (mainWindowSize.y + 30),
	     true);

  glutSetWindow(windowHandle);
  glutPopWindow();

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CW);

  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LEQUAL);
  glDepthRange(0.0f, 1.0f);

  glEnable(GL_TEXTURE_2D);

  // Initalizing AntTweakBar
  // -----------------
  TwWindowSize(mainWindowSize.x, mainWindowSize.y);
  TwInit(TW_OPENGL_CORE, NULL);

  terrainBar = TwNewBar("Terrain");
  TwDefine(" Terrain label='TerrainGen' position='16 16' size='400 500' valueswidth=200 fontsize=1");

  chunkMap.setTweakBar(terrainBar);
  camera->setTweakBar(terrainBar);
  // -----------------


  perspectiveMatrix = glm::perspective(45.0f, (float)mainWindowSize.x / mainWindowSize.y, 1.0f, 20000.0f);

  setGlobalMatrices(); // Order Here is Very Important
  loadShaders();

  GL_CHECK_ERRORS;

  setTextureStuff();

  // Process order may be an issue here.
  // Initializes internal tweak bar.


  Profiler::create();

  // Net Test

  // Vec2u dimensions(129, 129);
  // Vec2u internalDimensions = dimensions - Vec2u(2, 2);

  // std::vector<Vec4f> vertices;
  // vertices.resize(int32(dimensions.x * dimensions.y));

  // Vec2f dimensionsReal(100.0f, 100.0f);
  // Vec2f offset(0, dimensionsReal.y);
  // std::cout << vertices.size() << std::endl;
  // for(int32 y = 0; y < dimensions.y ; y++)
  // {
  //   for(int32 x = 0; x < dimensions.x ; x++)
  //   {
  //     int32 verticyIndex = (x + (y * dimensions.x));
  //     real32 scalar = 10.0f;

  //     real32 scaleX = (real32)1 / (dimensions.x - 2);
  //     real32 scaleY = (real32)1 / (dimensions.y - 2);

  //     real32 realScaleX = (real32)(x - 1) / (dimensions.x - 2);
  //     real32 realScaleY = (real32)(y - 1) / (dimensions.y - 2);

  //     real32 height = sin(realScaleX * M_PI * scalar + realScaleY * M_PI * scalar);

  //     Vec2f positionReal;
  //     positionReal.x = x * scaleX * dimensionsReal.x;
  //     positionReal.y = y * scaleY * dimensionsReal.y;

  //     // Vec2f positionReal ((scaleX * dimensionsReal.x) + (x * scaleX * dimensionsReal.x),
  //     //			  (scaleY * dimensionsReal.y) + (y * scaleY * dimensionsReal.y));

  //     positionReal -= offset;

  //     vertices[verticyIndex] = Vec4f(positionReal.x, height, positionReal.y, 1.0f);
  //   }
  // }

  // std::list<GenData> genDataList;
  // GenData genData = { NT_PERLIN, {0.2f, 5, 2.0f, 0.4f}, 2.0f };
  // genDataList.push_back(genData);
  // genDataList.push_back(genData);
  // genDataList.push_back(genData);

  // vector<Vec4f>& map = Noise::getMapFast(Vec2f(0, 0), 65, genDataList, "Map1");

  // testNet.prepareData(dimensions, vertices);
  // testNet.copyToGfx(normalsShader);

  // // Test 1

  // for(auto it = vertices.begin(); it != vertices.end(); it++)
  // {
  //   Vec4f& vertex = *it;
  //   vertex += Vec4f(-100.0f, 0, 0, 0);
  // }

  // testNet2.prepareDataWithBounds(internalDimensions, vertices);
  // testNet2.copyToGfx(normalsShader);

  // // Test 2

  // for(auto it = vertices.begin(); it != vertices.end(); it++)
  // {
  //   Vec4f& vertex = *it;
  //   vertex += Vec4f(0, 0, 100.0f, 0);
  // }

  // testNet3.prepareDataWithBounds(internalDimensions, vertices);
  // testNet3.copyToGfx(normalsShader);

  // setTexturedQuad();
  // GL_CHECK_ERRORS;

  textureModule.initialize(ssTextureShader, Vec2i(mainWindowSize.x, mainWindowSize.y));

  start();
}

void Game::myRenderFunction()
{
  debugCounter += lastDelta;
  Profiler::get()->startFrame();

  // NOTE(Jakub): Left Profiler code in net construction part
#if 0
  std::list<GenData> genDataList;
  GenData genData = { NT_PERLIN, {0.2f, 5, 2.0f, 0.4f}, 2.0f };
  genDataList.push_back(genData);
  genDataList.push_back(genData);
  genDataList.push_back(genData);

  Profiler::get()->start("NoiseAq");
  vector<Vec4f>& map = Noise::getMapFast(Vec2f(0, 0), 65, genDataList, "Map1");
  Profiler::get()->end("NoiseAq");

  Profiler::get()->start("NetConstruct");
  Net net(Vec2u(64, 64), map, normalsShader);
  Profiler::get()->end("NetConstruct");

  Profiler::get()->endFrame();
  Profiler::get()->showData();

  glutLeaveMainLoop();
#else

  chunkMap.process(normalsShader, glm::vec2(camera->getPosition().x, camera->getPosition().z));
  if(inputManager.isKeyPressed('t')) chunkMap.showDebugInfo();
  render();

  if(inputManager.isKeyPressed('q'))
  {
    chunkMap.cleanUp(normalsShader);
    glutLeaveMainLoop();
  }
  GL_CHECK_ERRORS;

#endif

  Profiler::get()->endFrame();

  if(inputManager.isKeyPressed('p'))
  {
    Profiler::get()->showData();
  }
}

void Game::render()
{

  static RENDER_TYPE renderType = RT_TRIANGLES;
  if(inputManager.isKeyPressed('1')) renderType = RT_POINTS;
  if(inputManager.isKeyPressed('2')) renderType = RT_LINES;
  if(inputManager.isKeyPressed('3')) renderType = RT_TRIANGLES;

  glClearColor(0.5f, 0.5f, 0.5f, 0);
  // glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  static glm::mat4 modelViewMatrix;
  modelViewMatrix = *camera->update(inputManager, lastDelta);

  glBindBuffer(GL_UNIFORM_BUFFER, globalMatricesUBO);
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(modelViewMatrix));
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 3, sizeof(unsigned int), &debugCounter);
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, sizeof(glm::mat4), glm::value_ptr(glm::mat4(1.0)));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  GL_CHECK_ERRORS;
  chunkMap.render(normalsShader, renderType, globalMatricesUBO, camera->getCameraData());
  GL_CHECK_ERRORS;

  textureModule.render(ssTextureShader);

  TwDraw();
  glutSwapBuffers();
}

void Game::loadShaders()
{

  shader.loadFromFile(GL_VERTEX_SHADER, "Shaders/shader.vert");
  shader.loadFromFile(GL_FRAGMENT_SHADER, "Shaders/shader.frag");
  shader.createAndLinkProgram();
  shader.use();
  shader.addAttribute("position");
  shader.addAttribute("color");
  shader.addUniform("MVP");
  shader.unUse();

  //GL_CHECK_ERRORS;

  normalsShader.loadFromFile(GL_VERTEX_SHADER, "Shaders/normalsShader.vert");
  normalsShader.loadFromFile(GL_FRAGMENT_SHADER, "Shaders/normalsShader.frag");
  normalsShader.createAndLinkProgram();
  normalsShader.use();
  normalsShader.addUniformBlock("GlobalMatrices");
  normalsShader.addAttribute("position");
  normalsShader.addAttribute("normal");
  normalsShader.addUniform("heightBounds");
  normalsShader.addUniform("colorSet");
  normalsShader.unUse();

  //Binding UniformBlockIndex With Uniform Binding Index
  normalsShader.bindUniformBlock("GlobalMatrices", globalMatricesUBI);

  textureShader.loadFromFile(GL_VERTEX_SHADER, "Shaders/textureShader.vert");
  textureShader.loadFromFile(GL_FRAGMENT_SHADER, "Shaders/textureShader.frag");
  textureShader.createAndLinkProgram();
  textureShader.use();
  textureShader.addUniformBlock("GlobalMatrices");
  textureShader.addAttribute("position");
  textureShader.addAttribute("texCoord");
  textureShader.addUniform("tex");
  textureShader.unUse();

  textureShader.bindUniformBlock("GlobalMatrices", globalMatricesUBI);

  ssTextureShader.loadFromFile(GL_VERTEX_SHADER, "Shaders/ssTextureShader.vert");
  ssTextureShader.loadFromFile(GL_FRAGMENT_SHADER, "Shaders/textureShader.frag");
  ssTextureShader.createAndLinkProgram();
  ssTextureShader.use();
  ssTextureShader.addAttribute("position");
  ssTextureShader.addAttribute("texCoord");
  ssTextureShader.addUniform("tex");
  ssTextureShader.unUse();

}

void Game::setGlobalMatrices()
{
  // Creating Buffers

  glGenBuffers(1, &globalMatricesUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, globalMatricesUBO);
  // 3 4x4 matrices + float
  glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 3 + sizeof(unsigned int), NULL, GL_STREAM_DRAW);

  // Setting Perspective Matrix Values (and temporarily local)
  glm::mat4 identity = glm::mat4(1);

  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(perspectiveMatrix));
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, sizeof(glm::mat4), glm::value_ptr(identity));

  // REMEMBER BINDING INDEX VALUE
  globalMatricesUBI = 0; // FUCKING HATE CONSTANTS LIKE THIS

  // Binding Buffer and Binding Index Value
  glBindBufferRange(GL_UNIFORM_BUFFER, globalMatricesUBI, globalMatricesUBO, 0, sizeof(glm::mat4) * 3);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Game::setTextureStuff()
{
  // IMPORTANT
  texBindingUnit = 1;

  textureShader.use();
  // Binding Texture Binding Unit To Script
  glUniform1i(textureShader("tex"), texBindingUnit);
  textureShader.unUse();

  ssTextureShader.use();
  glUniform1i(ssTextureShader("tex"), texBindingUnit);
  ssTextureShader.unUse();

  // Setting Sampler Values
  glGenSamplers(1, &texSampler);
  glSamplerParameteri(texSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glSamplerParameteri(texSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glSamplerParameteri(texSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glSamplerParameteri(texSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glSamplerParameteri(texSampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

  // Binding Sampler to Texture Binding Unit
  glBindSampler(texBindingUnit, texSampler);

}

void Game::setTexturedQuad()
{
  vector<glm::vec3> textureData;
  int textureWidth = 128;
  int textureArea = textureWidth*textureWidth;
  textureData.resize(textureArea);
  float greyValue;

  float stepSize = 1.0f / textureWidth;

  Vec2f point00 = Vec2f(-0.5f, 0.5f);
  Vec2f point10 = Vec2f(0.5f, 0.5f);
  Vec2f point01 = Vec2f(-0.5f, -0.5f);
  Vec2f point11 = Vec2f(0.5f, -0.5f);

  NoiseParams noiseParams = { 2, 2, 2.0f, 0.5f, 1}; // { 2, 2, 2.0f, 0.5f};

  for(int y = 0; y < textureWidth; y++) {
    Vec2f point0 = Vec2f::lerp(point00, point01, ((float)y + 0.5f) * stepSize);
    Vec2f point1 = Vec2f::lerp(point10, point11, ((float)y + 0.5f) * stepSize);
    for(int x = 0; x < textureWidth; x++) {
      Vec2f point = Vec2f::lerp(point0, point1, ((float)x + 0.5f) * stepSize);

      greyValue = Noise::sumWorley(point, noiseParams);
      //greyValue = Noise::sumValue(point, 3, 5);
      textureData[y * textureWidth + x] = glm::vec3(greyValue, greyValue, greyValue);
    }
  }

  //  texturedQuad.prepareData(textureData, textureWidth, 100);
  texturedQuad.prepareData(textureData, textureWidth, 0.6, ((real32)mainWindowSize.x / mainWindowSize.y), Vec2f(0.8f, 0.65f));
  texturedQuad.copyToGfx(textureShader);
}
