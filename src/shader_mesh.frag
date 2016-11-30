#version 330
precision lowp float;

uniform float show_uvs;
uniform vec4 fixed_color;
uniform vec3 specular_color;
uniform vec3 base_color;
uniform vec3 edge_factor;
uniform vec3 interior_factor;

in fData {
  vec3 to_eye;
  vec3 to_light;
  vec3 normal;
  vec2 texcoord;
  vec4 color;
} frag;

out vec4 outColor;

void main() {
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
    const float inv_sqrt3 = 0.577350269189626;
    vec3 tx = vec3(
      frag.texcoord.x + inv_sqrt3 * frag.texcoord.y,
      frag.texcoord.x - inv_sqrt3 * frag.texcoord.y,
      2.0*inv_sqrt3 * frag.texcoord.y
    );

    bool a = abs(fract(tx.x + 0.5) - 0.5) < 0.1;
    bool b = abs(fract(tx.y + 0.5) - 0.5) < 0.1;
    bool c = abs(fract(tx.z + 0.5) - 0.5) < 0.1;
    if (a || b || c)
      finalColor *= edge_factor;
    else
      finalColor *= interior_factor;
  }

  outColor = vec4(finalColor, 1.0);
}