#version 330 core
smooth in vec4 smoothColor;

in float fogFactor;
flat in int isExported;

out vec4 fragColor;
const vec4 fogColor = vec4(0.5, 0.5, 0.5, 0.5);
const vec4 nonExportColor = vec4(1.0, 0, 0, 0.5);

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
  if(isExported == 0) fragColor = nonExportColor;
  // fragColor = mix(vec4(1.0, 1.0, 1.0, 0), smoothColor, min(fogFactor, 0.0));
  //fragColor = mix(vec4(1.0, 1.0, 1.0, 0), smoothColor, max(fogFactor, 1.0));
}
