
#include "Game.h"
#include "TerrainGenerator.h"
#include "Noise.h"

void Game::setupAndStart() {
  srand(((unsigned int)time(NULL)));
  
  // Window Things 
  
  bool fullHD = true;
  mainWindowSize = fullHD ? glm::ivec2(1280, 720) : glm::ivec2(1000, 600);
  initializeWindow(mainWindowSize, "ProcGen");
  
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
  
  // AntTweakBar
  setTweakBar();
  
  perspectiveMatrix = glm::perspective(45.0f, (float)mainWindowSize.x / mainWindowSize.y, 1.0f, 20000.0f);
  
  setGlobalMatrices(); // Order Here is Very Important
  loadShaders();
  setTextureStuff();
  //setTexturedQuad();
  
  start();
}
void Game::myRenderFunction() {
  debugCounter += lastDelta;
  
  //threadStuff();
  static bool temp = false;
  if(inputManager.isKeyPressed('t')) temp = !temp;
  if(!temp) chunkMap.process(normalsShader, glm::vec2(camera->getPosition().x, camera->getPosition().z));
  if(inputManager.isKeyPressed('t')) chunkMap.showDebugInfo();
  if(inputManager.isKeyPressed('q')) glutLeaveMainLoop();
  render();
  
  GL_CHECK_ERRORS
    }

void Game::render() {
  
  static RENDER_TYPE renderType = RT_TRIANGLES;
  if(inputManager.isKeyPressed('1')) renderType = RT_POINTS;
  if(inputManager.isKeyPressed('2')) renderType = RT_LINES;
  if(inputManager.isKeyPressed('3')) renderType = RT_TRIANGLES;
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  static glm::mat4 modelViewMatrix;
  modelViewMatrix = *camera->update(inputManager, lastDelta);
  
  glBindBuffer(GL_UNIFORM_BUFFER, globalMatricesUBO);
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(modelViewMatrix));
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 3, sizeof(unsigned int), &debugCounter);
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, sizeof(glm::mat4), glm::value_ptr(glm::mat4()));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  
  GL_CHECK_ERRORS;
  chunkMap.render(normalsShader, renderType, globalMatricesUBO);
  GL_CHECK_ERRORS;
  
  // Rendering Texture Quad
  
  /*glBindBuffer(GL_UNIFORM_BUFFER, globalMatricesUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, sizeof(glm::mat4), glm::value_ptr(glm::translate(glm::mat4(),glm::vec3(0, 200.0f,0))));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    textureShader.use();
    texturedQuad.render(renderType, texBindingUnit);
    textureShader.unUse();*/
  
  /*textureShader.use();
    glBindBuffer(GL_UNIFORM_BUFFER, globalMatricesUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, sizeof(glm::mat4), glm::value_ptr(glm::translate(glm::mat4(), glm::vec3(100.0f,0,0))));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    texturedQuad2.render(renderType, texBindingUnit);
    textureShader.unUse();*/
  
  
  TwDraw();
  glutSwapBuffers();
}

void Game::loadShaders() {
  
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
  
}
void Game::setGlobalMatrices() {
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
void Game::setTextureStuff() {
  
  // IMPORTANT
  texBindingUnit = 1;
  
  textureShader.use();
  
  // Binding Texture Binding Unit To Script
  glUniform1i(textureShader("tex"), texBindingUnit);
  
  // Setting Sampler Values
  glGenSamplers(1, &texSampler);
  glSamplerParameteri(texSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glSamplerParameteri(texSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glSamplerParameteri(texSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glSamplerParameteri(texSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glSamplerParameteri(texSampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
  
  // Binding Sampler to Texture Binding Unit
  glBindSampler(texBindingUnit, texSampler);
  
  textureShader.unUse();
  
}

void Game::setTexturedQuad() {
  vector<glm::vec3> textureData;
  int textureWidth = 64;
  int textureArea = textureWidth*textureWidth;
  textureData.resize(textureArea);
  float greyValue;
  
  float stepSize = 1.0f / textureWidth;
  
  //for(int i = 0; i < textureArea; i++) {
  //	//textureData[i] = glm::vec3((rand() % 2 == 0 ? 1.0f : 0), (rand() % 2 == 1 ? 1.0f : 0), (rand() % 2 == 1 ? 1.0f : 0));
  
  //	greyValue = Noise::value(glm::vec2((i % textureWidth) + 0.5f - textureWidth / 2.0f, int(i / textureWidth) + 0.5f) - textureWidth / 2.0f, 0.075f);
  //	textureData[i] = glm::vec3(greyValue, greyValue, greyValue);
  //}
  
  glm::vec2 point00 = glm::vec2(-0.5f, 0.5f);
  glm::vec2 point10 = glm::vec2(0.5f, 0.5f);
  glm::vec2 point01 = glm::vec2(-0.5f, -0.5f);
  glm::vec2 point11 = glm::vec2(0.5f, -0.5f);
  
  NoiseParams noiseParams = { 3, 5, 2.0f, 0.5f };
  
  for(int y = 0; y < textureWidth; y++) {
    glm::vec2 point0 = glm::lerp(point00, point01, ((float)y + 0.5f) * stepSize);
    glm::vec2 point1 = glm::lerp(point10, point11, ((float)y + 0.5f) * stepSize);
    for(int x = 0; x < textureWidth; x++) {
      glm::vec2 point = glm::lerp(point0, point1, ((float)x + 0.5f) * stepSize);
      
      greyValue = Noise::sumPerlin(point, noiseParams) * 0.5f + 0.5f;
      //greyValue = Noise::sumValue(point, 3, 5);
      textureData[y * textureWidth + x] = glm::vec3(greyValue, greyValue, greyValue);
    }
  }
  
  texturedQuad.prepareData(textureData, textureWidth, 100);
  texturedQuad.copyToGfx(textureShader);
  
  for(int y = 0; y < textureWidth; y++) {
    glm::vec2 point0 = glm::lerp(point00, point01, ((float)y + 0.5f) * stepSize);
    glm::vec2 point1 = glm::lerp(point10, point11, ((float)y + 0.5f) * stepSize);
    for(int x = 0; x < textureWidth; x++) {
      glm::vec2 point = glm::lerp(point0, point1, ((float)x + 0.5f) * stepSize);
      point.x += 1.0f;
      greyValue = Noise::sumPerlin(point, noiseParams) * 0.5f + 0.5f;
      //greyValue = Noise::sumValue(point, 3, 5);
      textureData[y * textureWidth + x] = glm::vec3(greyValue, greyValue, greyValue);
    }
  }
  texturedQuad2.prepareData(textureData, textureWidth, 100);
  texturedQuad2.copyToGfx(textureShader);
}

void Game::setTweakBar() {
  TwWindowSize(mainWindowSize.x, mainWindowSize.y);
  TwInit(TW_OPENGL_CORE, NULL);
  myBar = TwNewBar("Generation Menu");
  // Si³a Wiatru
  chunkMap.setTweakBar(myBar);
  camera->setTweakBar(myBar);
}
