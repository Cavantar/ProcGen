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

struct ListColor
{
  vec4 color;
  float startValue;
};

const int numbOfColors = 16;
uniform ListColor colors[numbOfColors];
uniform int renderOptions;

smooth out vec4 smoothColor;
out float fogFactor;
flat out int isExported;

vec3 getLightDirection()
{
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

vec4
getColor(float greyValue)
{
  vec4 resultColor = vec4(greyValue, greyValue, greyValue, 1.0);

  ListColor bottomColor = colors[0];
  ListColor topColor = colors[0];

  for(int i = 1; i < numbOfColors; i++)
    {
      topColor = colors[i];

      if(greyValue >= bottomColor.startValue &&
	 greyValue <= topColor.startValue)
	{
	  float valueDelta = (topColor.startValue - bottomColor.startValue);
	  float tValue = (greyValue - bottomColor.startValue) / valueDelta;

	  resultColor = mix(bottomColor.color, topColor.color, tValue);
	  break;
	}

      bottomColor = topColor;
    }

  return resultColor;
}

const float fogStart = 400.0;
const float fogEnd = 1200.0;

vec3 extractCameraPos(mat4 a_modelView)
{
  mat3 rotMat = mat3(a_modelView);
  vec3 d = vec3(a_modelView[3]);

  vec3 retVec = -d * rotMat;
  return retVec;
}

vec2 getChunkPosition(vec2 worldPosition)
{
  vec2 chunkPosition = vec2(int(floor((worldPosition.x - 50.0f) / 100)) + 1,
			    -int(floor((worldPosition.y + 50) / 100)));
  return chunkPosition;
}

void main() {

  vec3 lightDirection = getLightDirection();

  // Light Calculations
  mat4 localToCamera = worldToCameraMatrix * localToWorldMatrix;

  // localToCamera = localToWorlMatrix;
  //mat3 localToCameraNormal = mat3(worldToCameraMatrix) * mat3(localToWorldMatrix);
  mat3 localToCameraNormal = mat3(worldToCameraMatrix);
  localToCameraNormal *= mat3(localToWorldMatrix);

  // Incidence Angle
  lightDirection = localToCameraNormal * lightDirection;

  mat3 localToCameraNormal2 = mat3(1.0);
  vec3 normCamSpace = normalize(localToCameraNormal * normal);

  float cosAngIncidence = dot(normCamSpace, lightDirection);
  cosAngIncidence = clamp(cosAngIncidence, 0, 1);

  float ambientLight = 0.2f;
  vec4 tempColor = vec4(1.0, 1.0, 1.0, 1.0);
  tempColor = vec4(normal, 1.0);

  // Color Interpolation

  // float heightDelta = heightBounds.y - heightBounds.x;
  // float heightPerc = (position.y - heightBounds.x)/heightDelta;
  // heightPerc = clamp(heightPerc, 0, 1.0);

  tempColor = getColor(position.y / 100.0f);

  vec2 cameraPosition = getChunkPosition(extractCameraPos(worldToCameraMatrix).xz);
  vec2 worldChunkPosition = getChunkPosition(position.xz + localToWorldMatrix[3].xz);

  vec2 cameraDelta = vec2(worldChunkPosition - cameraPosition);

  isExported = 1;
  // -1 because radius 1 only should cover one chunk
  int chunkRadius = (renderOptions >> 16) - 1;
  if((renderOptions & 2) > 0 &&  (abs(cameraDelta.x) > chunkRadius ||
				  abs(cameraDelta.y) > chunkRadius))
    {
      //isExported = 0;
      tempColor = vec4(1.0, 0, 0, 1.0);
    }

  // smoothColor = tempColor;
  if((renderOptions & 1) > 0) smoothColor = tempColor;
  else smoothColor = tempColor * cosAngIncidence + tempColor * ambientLight;

  gl_Position =  cameraToClipMatrix * localToCamera * position;

  float distanceFromCamera = length(gl_Position);
  fogFactor = (distanceFromCamera - fogStart) / (fogEnd - fogStart);


  // fogFactor = clamp(fogFactor, 0.0, 1.0);
}
