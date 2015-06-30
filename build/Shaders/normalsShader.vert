#version 330 core

in vec4 position;
in vec3 normal;

layout(std140) uniform GlobalMatrices
{
  mat4 cameraToClipMatrix;	// Perspective Matrix
  mat4 worldToCameraMatrix;	// CameraMatrix
  mat4 localToWorldMatrix;	// LocalMatrix
  
  uint debugCounter;	// Counter
};

uniform vec4 colorSet[2];
uniform vec2 heightBounds;
smooth out vec4 smoothColor;
out float fogFactor;

vec3 getLightDirection(){
  const uint periodInMilis = uint(5000);
  float timePassed = (debugCounter % periodInMilis)/ float(periodInMilis);
  float lightAngle = 3.14f * 2.0f * timePassed;
  float tiltAngle = 3.14f/3.0f;
  
  vec3 lightDirection = vec3(0, cos(tiltAngle), sin(tiltAngle));
  
  vec3 lightDirectionFinal;
  lightDirectionFinal.x = lightDirection.x * cos(lightAngle) - lightDirection.z * sin(lightAngle);
  lightDirectionFinal.y = lightDirection.y;
  lightDirectionFinal.z = lightDirection.x * sin(lightAngle) + lightDirection.z * cos(lightAngle);
  
  return lightDirectionFinal;
}

const float fogStart = 400.0;
const float fogEnd = 1200.0;

void main() {
  
  vec3 lightDirection = getLightDirection();

  // Light Calculations
  mat4 localToCamera = worldToCameraMatrix * localToWorldMatrix;
  //mat3 localToCameraNormal = mat3(worldToCameraMatrix) * mat3(localToWorldMatrix);
  mat3 localToCameraNormal = mat3(worldToCameraMatrix);
  localToCameraNormal *= mat3(localToWorldMatrix);
  
  //vec4 tempNormal = localToCamera * vec4(normal, 0);
  
  // Incidence Angle
  lightDirection = localToCameraNormal * lightDirection;
  
  mat3 localToCameraNormal2 = mat3(1.0);
  
  vec3 normCamSpace = normalize(localToCameraNormal * normal);
  //vec3 normCamSpace = normalize(tempMat * normal);
  //vec3 normCamSpace = vec3(1.0, 0, 0);
  
  float cosAngIncidence = dot(normCamSpace, lightDirection);
  cosAngIncidence = clamp(cosAngIncidence, 0, 1);
  
  
  float ambientLight = 0.2f;
  vec4 tempColor = vec4(1.0, 1.0, 1.0, 1.0);
  tempColor = vec4(normal, 1.0);
  
  // Color Interpolation
  
  float heightDelta = heightBounds.y - heightBounds.x;
  float heightPerc = (position.y - heightBounds.x)/heightDelta;
  heightPerc = clamp(heightPerc, 0, 1.0);
  
  //tempColor = mix(vec4(0,0.67,0,1.0), vec4(0,0,0.67,1.0), heightPerc);
  //oryg 
  tempColor = mix(colorSet[0], colorSet[1], heightPerc);
  //tempColor = mix(vec4(0,0.67,0,1.0), vec4(0.8,0.8,0.8,1.0), heightPerc);
  
  //smoothColor = tempColor;
  
  if(colorSet[1].z >= 0.8f)smoothColor = tempColor * cosAngIncidence + tempColor * ambientLight; 
  else smoothColor = tempColor;
  
  gl_Position =  cameraToClipMatrix * localToCamera * position;

  float distanceFromCamera = length(gl_Position);
  
  fogFactor = (distanceFromCamera - fogStart) / (fogEnd - fogStart);
  // fogFactor = clamp(fogFactor, 0.0, 1.0);
}
