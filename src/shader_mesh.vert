#version 330

in vec3 position;
in vec3 normal;
in vec2 texcoord;
in vec4 color;

out vData {
  vec3 normal;
  vec2 texcoord;
  vec4 color;
} vertex;

void main() {
  gl_Position = vec4(position, 1.0);
  vertex.normal = normal;
  vertex.texcoord = texcoord;
  vertex.color = color;
}