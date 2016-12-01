#version 330
precision lowp float;

uniform vec3 color;
out vec4 outColor;

void main() {
  if (color == vec3(0.0))
    discard;
  outColor = vec4(color, 1.0);
}