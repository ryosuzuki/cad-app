

#include "opengl_state.h"
#include "mesh.h"

void OpenGLState::initBuffers() {

  // Mesh: Vertex Array Object & Buffer objects
  glGenVertexArrays(1, &vaoMesh);
  glBindVertexArray(vaoMesh);
  glGenBuffers(1, &vboVertices);
  glGenBuffers(1, &vboVertexNormals);
  glGenBuffers(1, &vboVertexAmbient);
  glGenBuffers(1, &vboVertexDiffuse);
  glGenBuffers(1, &vboVertexSpecular);
  glGenBuffers(1, &vboVertexUv);
  glGenBuffers(1, &vboFace);
  glGenTextures(1, &vboTextures);

  // Line overlay
  glGenVertexArrays(1, &vaoverlayLines);
  glBindVertexArray(vaoOverlayLines);
  glGenBuffers(1, &vboLinesFaces);
  glGenBuffers(1, &vboLinesVertices);
  glGenBuffers(1, &vboLinesVertexColors);

  // Point overlay
  glGenVertexArrays(1, &vaoOverlayPoints);
  glBindVertexArray(vaoOverlayPoints);
  glGenBuffers(1, &vboPointFaces);
  glGenBuffers(1, &vboPointVertices);
  glGenBuffers(1, &vboPointVertexColors);

}

void OpenGLState::freeBuffers() {
  glDeleteVertexArrays(1, &vaoMesh);
  glDeleteVertexArrays(1, &vaoOverlayLines);
  glDeleteVertexArrays(1, &vaoOverlayPoints);

  glDeleteBuffers(1, &vboVertex);
  glDeleteBuffers(1, &vboVertexNormals);
  glDeleteBuffers(1, &vboVertexAmbient);
  glDeleteBuffers(1, &vboVertexDiffuse);
  glDeleteBuffers(1, &vboVertexSpecular);
  glDeleteBuffers(1, &vboVertexUv);
  glDeleteBuffers(1, &vboFaces);
  glDeleteBuffers(1, &vboLinesFaces);
  glDeleteBuffers(1, &vboLinesVertices);
  glDeleteBuffers(1, &vboLinesVertexColors);
  glDeleteBuffers(1, &vboPointsFaces);
  glDeleteBuffers(1, &vboPointsVertices);
  glDeleteBuffers(1, &vboPointsVertexColors);

  glDeleteTextures(1, &vboTextures);
}


void OpenGLState::setData(const Mesh &mesh, ool invertNormals) {
  bool perCornerUv = (data.F_uv.rows() == data.F.rows());
  bool perCornerNormals = (data.FNormals.rows() == 3 * data.F.rows());

  dirty |= data.dirty;

  if (!data.face_based) {
    if (!perCornerUv) {
      // Vertex positions
      if (dirty & ViewerData::DIRTY_POSITION) {
        vVbo = (data.V.transpose()).cast<float>();
      }

      // Vertex normals
      if (dirty & ViewerData::DIRTY_NORMAL) {
        VNormals_vbo = (data.VNormals.transpose()).cast<float>();
        if (invertNormals) {
          VNormals_vbo = -VNormals_vbo;
        }
      }

      // Per-vertex material settings
      if (dirty & ViewerData::DIRTY_AMBIENT) {
        vAmbientVbo = (data.V_materialAmbient.transpose()).cast<float>();
      }
      if (dirty & ViewerData::DIRTY_DIFFUSE) {
        VDiffuse_vbo = (data.V_materialDiffuse.transpose()).cast<float>();
      }
      if (dirty & ViewerData::DIRTY_SPECULAR)
        V_specular_vbo = (data.V_material_specular.transpose()).cast<float>();

      // Face indices
      if (dirty & ViewerData::DIRTY_FACE)
        F_vbo = (data.F.transpose()).cast<unsigned>();

      // Texture coordinates
      if (dirty & ViewerData::DIRTY_UV)
        V_uv_vbo = (data.V_uv.transpose()).cast<float>();
    } else {
      // Per vertex properties with per corner UVs
      if (dirty & ViewerData::DIRTY_POSITION) {
        vVbo.resize(3,data.F.rows()*3);
        for (unsigned i=0; i<data.F.rows();++i) {
          for (unsigned j=0;j<3;++j) {
            vVbo.col(i*3+j) = data.V.row(data.F(i,j)).transpose().cast<float>();
          }
        }
      }

      if (dirty & ViewerData::DIRTY_AMBIENT) {
        vAmbientVbo.resize(3,data.F.rows()*3);
        for (unsigned i=0; i<data.F.rows();++i) {
          for (unsigned j=0;j<3;++j) {
            vAmbientVbo.col (i*3+j) = data.V_materialAmbient.row(data.F(i,j)).transpose().cast<float>();
          }
        }
      }

      if (dirty & ViewerData::DIRTY_DIFFUSE) {
        VDiffuse_vbo.resize(3,data.F.rows()*3);
        for (unsigned i=0; i<data.F.rows();++i) {
          for (unsigned j=0;j<3;++j) {
            VDiffuse_vbo.col (i*3+j) = data.V_materialDiffuse.row(data.F(i,j)).transpose().cast<float>();
          }
        }
      }

      if (dirty & ViewerData::DIRTY_SPECULAR) {
        V_specular_vbo.resize(3,data.F.rows()*3);
        for (unsigned i=0; i<data.F.rows();++i) {
          for (unsigned j=0;j<3;++j) {
            V_specular_vbo.col(i*3+j) = data.V_material_specular.row(data.F(i,j)).transpose().cast<float>();
          }
        }
      }

      if (dirty & ViewerData::DIRTY_NORMAL) {
        VNormals_vbo.resize(3,data.F.rows()*3);
        for (unsigned i=0; i<data.F.rows();++i) {
          for (unsigned j=0;j<3;++j) {
            VNormals_vbo.col (i*3+j) = data.VNormals.row(data.F(i,j)).transpose().cast<float>();
          }
        }

        if (invertNormals) {
          VNormals_vbo = -VNormals_vbo;
        }
      }

      if (dirty & ViewerData::DIRTY_FACE) {
        F_vbo.resize(3,data.F.rows());
        for (unsigned i=0; i<data.F.rows();++i) {
          F_vbo.col(i) << i*3+0, i*3+1, i*3+2;
        }
      }

      if (dirty & ViewerData::DIRTY_UV) {
        V_uv_vbo.resize(2,data.F.rows()*3);
        for (unsigned i=0; i<data.F.rows();++i) {
          for (unsigned j=0;j<3;++j) {
            V_uv_vbo.col(i*3+j) = data.V_uv.row(data.F(i,j)).transpose().cast<float>();
          }
        }
      }
    }
  } else {
    if (dirty & ViewerData::DIRTY_POSITION) {
      vVbo.resize(3,data.F.rows()*3);
      for (unsigned i=0; i<data.F.rows();++i) {
        for (unsigned j=0;j<3;++j) {
          vVbo.col(i*3+j) = data.V.row(data.F(i,j)).transpose().cast<float>();
        }
      }
    }

    if (dirty & ViewerData::DIRTY_AMBIENT)
    {
      vAmbientVbo.resize(3,data.F.rows()*3);
      for (unsigned i=0; i<data.F.rows();++i)
        for (unsigned j=0;j<3;++j)
          vAmbientVbo.col (i*3+j) = data.F_materialAmbient.row(i).transpose().cast<float>();
    }

    if (dirty & ViewerData::DIRTY_DIFFUSE)
    {
      VDiffuse_vbo.resize(3,data.F.rows()*3);
      for (unsigned i=0; i<data.F.rows();++i)
        for (unsigned j=0;j<3;++j)
          VDiffuse_vbo.col (i*3+j) = data.F_materialDiffuse.row(i).transpose().cast<float>();
    }

    if (dirty & ViewerData::DIRTY_SPECULAR)
    {
      V_specular_vbo.resize(3,data.F.rows()*3);
      for (unsigned i=0; i<data.F.rows();++i)
        for (unsigned j=0;j<3;++j)
          V_specular_vbo.col(i*3+j) = data.F_material_specular.row(i).transpose().cast<float>();
    }

    if (dirty & ViewerData::DIRTY_NORMAL)
    {
      VNormals_vbo.resize(3,data.F.rows()*3);
      for (unsigned i=0; i<data.F.rows();++i)
        for (unsigned j=0;j<3;++j)
          VNormals_vbo.col (i*3+j) =
             perCornerNormals ?
               data.FNormals.row(i*3+j).transpose().cast<float>() :
               data.FNormals.row(i).transpose().cast<float>();

      if (invertNormals)
        VNormals_vbo = -VNormals_vbo;
    }

    if (dirty & ViewerData::DIRTY_FACE)
    {
      F_vbo.resize(3,data.F.rows());
      for (unsigned i=0; i<data.F.rows();++i)
        F_vbo.col(i) << i*3+0, i*3+1, i*3+2;
    }

    if (dirty & ViewerData::DIRTY_UV)
    {
        V_uv_vbo.resize(2,data.F.rows()*3);
        for (unsigned i=0; i<data.F.rows();++i)
          for (unsigned j=0;j<3;++j)
            V_uv_vbo.col(i*3+j) = data.V_uv.row(perCornerUv ? data.F_uv(i,j) : data.F(i,j)).transpose().cast<float>();
    }
  }

  if (dirty & ViewerData::DIRTY_TEXTURE)
  {
    tex_u = data.texture_R.rows();
    tex_v = data.texture_R.cols();
    tex.resize(data.texture_R.size()*3);
    for (unsigned i=0;i<data.texture_R.size();++i)
    {
      tex(i*3+0) = data.texture_R(i);
      tex(i*3+1) = data.texture_G(i);
      tex(i*3+2) = data.texture_B(i);
    }
  }

  if (dirty & ViewerData::DIRTY_OVERLAY_LINES)
  {
    lines_vVbo.resize(3, data.lines.rows()*2);
    lines_V_colors_vbo.resize(3, data.lines.rows()*2);
    lines_F_vbo.resize(1, data.lines.rows()*2);
    for (unsigned i=0; i<data.lines.rows();++i)
    {
      lines_vVbo.col(2*i+0) = data.lines.block<1, 3>(i, 0).transpose().cast<float>();
      lines_vVbo.col(2*i+1) = data.lines.block<1, 3>(i, 3).transpose().cast<float>();
      lines_V_colors_vbo.col(2*i+0) = data.lines.block<1, 3>(i, 6).transpose().cast<float>();
      lines_V_colors_vbo.col(2*i+1) = data.lines.block<1, 3>(i, 6).transpose().cast<float>();
      lines_F_vbo(2*i+0) = 2*i+0;
      lines_F_vbo(2*i+1) = 2*i+1;
    }
  }

  if (dirty & ViewerData::DIRTY_OVERLAY_POINTS)
  {
    points_vVbo.resize(3, data.points.rows());
    points_V_colors_vbo.resize(3, data.points.rows());
    points_F_vbo.resize(1, data.points.rows());
    for (unsigned i=0; i<data.points.rows();++i)
    {
      points_vVbo.col(i) = data.points.block<1, 3>(i, 0).transpose().cast<float>();
      points_V_colors_vbo.col(i) = data.points.block<1, 3>(i, 3).transpose().cast<float>();
      points_F_vbo(i) = i;
    }
  }
}

void OpenGLState::bind_mesh() {
  glBindVertexArray(vaoMesh);
  shader_mesh.bind();
  shader_mesh.bindVertexAttribArray("position", vboVertex, vVbo, dirty & ViewerData::DIRTY_POSITION);
  shader_mesh.bindVertexAttribArray("normal", vboVertexNormals, VNormals_vbo, dirty & ViewerData::DIRTY_NORMAL);
  shader_mesh.bindVertexAttribArray("Ka", vboVertexAmbient, vAmbientVbo, dirty & ViewerData::DIRTY_AMBIENT);
  shader_mesh.bindVertexAttribArray("Kd", vboVertexDiffuse, VDiffuse_vbo, dirty & ViewerData::DIRTY_DIFFUSE);
  shader_mesh.bindVertexAttribArray("Ks", vboVertex_specular, V_specular_vbo, dirty & ViewerData::DIRTY_SPECULAR);
  shader_mesh.bindVertexAttribArray("texcoord", vboVertex_uv, V_uv_vbo, dirty & ViewerData::DIRTY_UV);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_F);
  if (dirty & ViewerData::DIRTY_FACE)
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned)*F_vbo.size(), F_vbo.data(), GL_DYNAMIC_DRAW);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, vbo_tex);
  if (dirty & ViewerData::DIRTY_TEXTURE)
  {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_u, tex_v, 0, GL_RGB, GL_UNSIGNED_BYTE, tex.data());
  }
  glUniform1i(shader_mesh.uniform("tex"), 0);
  dirty &= ~ViewerData::DIRTY_MESH;
}

void OpenGLState::bind_overlay_lines() {
  bool isDirty = dirty & ViewerData::DIRTY_OVERLAY_LINES;

  glBindVertexArray(vao_overlay_lines);
  shader_overlay_lines.bind();
  shader_overlay_lines.bindVertexAttribArray("position", vbo_lines_V, lines_vVbo, isDirty);
  shader_overlay_lines.bindVertexAttribArray("color", vbo_lines_V_colors, lines_V_colors_vbo, isDirty);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_lines_F);
  if (isDirty)
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned)*lines_F_vbo.size(), lines_F_vbo.data(), GL_DYNAMIC_DRAW);

  dirty &= ~ViewerData::DIRTY_OVERLAY_LINES;
}

void OpenGLState::bind_overlay_points() {
  bool isDirty = dirty & ViewerData::DIRTY_OVERLAY_POINTS;

  glBindVertexArray(vao_overlay_points);
  shader_overlay_points.bind();
  shader_overlay_points.bindVertexAttribArray("position", vbo_points_V, points_vVbo, isDirty);
  shader_overlay_points.bindVertexAttribArray("color", vbo_points_V_colors, points_V_colors_vbo, isDirty);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_points_F);
  if (isDirty)
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned)*points_F_vbo.size(), points_F_vbo.data(), GL_DYNAMIC_DRAW);

  dirty &= ~ViewerData::DIRTY_OVERLAY_POINTS;
}

void OpenGLState::draw_mesh(bool solid) {
  glPolygonMode(GL_FRONT_AND_BACK, solid ? GL_FILL : GL_LINE);

  /* Avoid Z-buffer fighting between filled triangles & wireframe lines */
  if (solid)
  {
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0);
  }
  glDrawElements(GL_TRIANGLES, 3*F_vbo.cols(), GL_UNSIGNED_INT, 0);

  glDisable(GL_POLYGON_OFFSET_FILL);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void OpenGLState::draw_overlay_lines() {
  glDrawElements(GL_LINES, lines_F_vbo.cols(), GL_UNSIGNED_INT, 0);
}

void OpenGLState::draw_overlay_points() {
  glDrawElements(GL_POINTS, points_F_vbo.cols(), GL_UNSIGNED_INT, 0);
}


void OpenGLState::init()
{
  std::string meshVertexShaderString =
  "#version 150\n"
  "uniform mat4 model;"
  "uniform mat4 view;"
  "uniform mat4 proj;"
  "in vec3 position;"
  "in vec3 normal;"
  "out vec3 position_eye;"
  "out vec3 normal_eye;"
  "in vec3 Ka;"
  "in vec3 Kd;"
  "in vec3 Ks;"
  "in vec2 texcoord;"
  "out vec2 texcoordi;"
  "out vec3 Kai;"
  "out vec3 Kdi;"
  "out vec3 Ksi;"

  "void main()"
  "{"
  "  position_eye = vec3 (view * model * vec4 (position, 1.0));"
  "  normal_eye = vec3 (view * model * vec4 (normal, 0.0));"
  "  normal_eye = normalize(normal_eye);"
  "  gl_Position = proj * vec4 (position_eye, 1.0);" //proj * view * model * vec4(position, 1.0);"
  "  Kai = Ka;"
  "  Kdi = Kd;"
  "  Ksi = Ks;"
  "  texcoordi = texcoord;"
  "}";

  std::string meshFragmentShaderString =
  "#version 150\n"
  "uniform mat4 model;"
  "uniform mat4 view;"
  "uniform mat4 proj;"
  "uniform vec4 fixed_color;"
  "in vec3 position_eye;"
  "in vec3 normal_eye;"
  "uniform vec3 light_position_world;"
  "vec3 Ls = vec3 (1, 1, 1);"
  "vec3 Ld = vec3 (1, 1, 1);"
  "vec3 La = vec3 (1, 1, 1);"
  "in vec3 Ksi;"
  "in vec3 Kdi;"
  "in vec3 Kai;"
  "in vec2 texcoordi;"
  "uniform sampler2D tex;"
  "uniform float specular_exponent;"
  "uniform float lighting_factor;"
  "uniform float texture_factor;"
  "out vec4 outColor;"
  "void main()"
  "{"
  "vec3 Ia = La * Kai;"    // ambient intensity

  "vec3 light_position_eye = vec3 (view * vec4 (light_position_world, 1.0));"
  "vec3 vector_to_light_eye = light_position_eye - position_eye;"
  "vec3 direction_to_light_eye = normalize (vector_to_light_eye);"
  "float dot_prod = dot (direction_to_light_eye, normal_eye);"
  "float clampedDot_prod = max (dot_prod, 0.0);"
  "vec3 Id = Ld * Kdi * clampedDot_prod;"    // Diffuse intensity

  "vec3 reflection_eye = reflect (-direction_to_light_eye, normal_eye);"
  "vec3 surface_to_viewer_eye = normalize (-position_eye);"
  "float dot_prod_specular = dot (reflection_eye, surface_to_viewer_eye);"
  "dot_prod_specular = float(abs(dot_prod)==dot_prod) * max (dot_prod_specular, 0.0);"
  "float specular_factor = pow (dot_prod_specular, specular_exponent);"
  "vec3 Is = Ls * Ksi * specular_factor;"    // specular intensity
  "vec4 color = vec4(lighting_factor * (Is + Id) + Ia, 1.0) + vec4((1.0-lighting_factor) * Kdi,1.0);"
  "outColor = mix(vec4(1,1,1,1), texture(tex, texcoordi), texture_factor) * color;"
  "if (fixed_color != vec4(0.0)) outColor = fixed_color;"
  "}";

  std::string overlayVertexShaderString =
  "#version 150\n"
  "uniform mat4 model;"
  "uniform mat4 view;"
  "uniform mat4 proj;"
  "in vec3 position;"
  "in vec3 color;"
  "out vec3 color_frag;"

  "void main()"
  "{"
  "  gl_Position = proj * view * model * vec4 (position, 1.0);"
  "  color_frag = color;"
  "}";

  std::string overlayFragmentShaderString =
  "#version 150\n"
  "in vec3 color_frag;"
  "out vec4 outColor;"
  "void main()"
  "{"
  "  outColor = vec4(color_frag, 1.0);"
  "}";

  std::string overlayPointFragmentShaderString =
  "#version 150\n"
  "in vec3 color_frag;"
  "out vec4 outColor;"
  "void main()"
  "{"
  "  if (length(gl_PointCoord - vec2(0.5)) > 0.5)"
  "    discard;"
  "  outColor = vec4(color_frag, 1.0);"
  "}";

  initBuffers();
  shader_mesh.init(mesh_vertex_shader_string,
      mesh_fragment_shader_string, "outColor");
  shader_overlay_lines.init(overlay_vertex_shader_string,
      overlay_fragment_shader_string, "outColor");
  shader_overlay_points.init(overlay_vertex_shader_string,
      overlay_point_fragment_shader_string, "outColor");
}

void OpenGLState::free() {
  shader_mesh.free();
  shader_overlay_lines.free();
  shader_overlay_points.free();
  free_buffers();
}

