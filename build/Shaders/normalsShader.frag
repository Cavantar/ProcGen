#version 330 core
smooth in vec4 smoothColor;

in float fogFactor;
out vec4 fragColor;

const vec4 fogColor = vec4(0.5, 0.5, 0.5, 0.5);
  
void main() {
  if(fogFactor < 0.5)
    {
      fragColor = smoothColor;
    }
  else
    {
      fragColor = vec4(1.0, 0, 0, 0);
    }

  fragColor = mix(smoothColor, fogColor, fogFactor);
}
