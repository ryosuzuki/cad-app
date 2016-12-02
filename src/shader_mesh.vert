#version 330

in vec3 position;
in vec3 normal;
in vec2 texcoord;

out vData {
  vec3 normal;
  vec2 texcoord;
} vertex;

void main() {
  gl_Position = vec4(position, 1.0);
  vertex.normal = normal;
  vertex.texcoord = texcoord;
}