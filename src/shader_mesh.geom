#version 330

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 proj, model, view;
uniform vec3 light_position;
uniform vec3 camera_local;

in vData {
  vec3 normal;
  vec2 texcoord;
} vertices[];

out fData {
  vec3 to_eye;
  vec3 to_light;
  vec3 normal;
  vec2 texcoord;
} frag;

void main() {
  vec3 face_normal = normalize(cross(
    gl_in[1].gl_Position.xyz-gl_in[0].gl_Position.xyz,
    gl_in[2].gl_Position.xyz-gl_in[0].gl_Position.xyz));

  if (dot(gl_in[0].gl_Position.xyz - camera_local, face_normal) > 0.0)
    return;

  for (int i=0; i<3; ++i) {
    vec4 pos_camera = view * (model * gl_in[i].gl_Position);
    vec3 vn = vertices[i].normal;

    gl_Position = proj * pos_camera;
    frag.to_light = (view * vec4(light_position, 1.0)).xyz - pos_camera.xyz;
    frag.to_eye = -pos_camera.xyz;
    frag.normal = (model * (view * vec4(vn, 0.0))).xyz;
    frag.texcoord = vertices[i].texcoord;

    EmitVertex();
  }
  EndPrimitive();
}