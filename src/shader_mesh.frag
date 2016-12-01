#version 330
precision lowp float;

uniform float show_uvs;
uniform vec3 base_color;
uniform vec3 specular_color;
uniform vec4 fixed_color;

in fData {
  vec3 to_eye;
  vec3 to_light;
  vec3 normal;
  vec2 texcoord;
  vec4 color;
} frag;

out vec4 outColor;

void main() {
  outColor = vec4(1.0, 1.0, 1.0, 1.0);
  return;

  if (fixed_color.a != 0.0) {
    outColor = fixed_color;
    return;
  }

  vec3 Kd = base_color;
  vec3 Ks = specular_color;
  vec3 Ka = Kd * 0.2;

  vec3 to_light = normalize(frag.to_light);
  vec3 to_eye = normalize(frag.to_eye);
  vec3 normal = normalize(frag.normal);
  vec3 refl = reflect(-to_light, normal);

  float diffuse_factor = max(0.0, dot(to_light, normal));
  float specular_factor = pow(max(dot(to_eye, refl), 0.0), 10.0);

  vec3 finalColor = (Ka + Kd*diffuse_factor + Ks*specular_factor) * (1-frag.color.a) +
    frag.color.rgb * frag.color.a;

  if (show_uvs == 1.0) {
    // add uv mapping with texcoord
  }

  outColor = vec4(finalColor, 1.0);
}
