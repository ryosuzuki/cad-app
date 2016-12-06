
#include "loader.h"

void Loader::loadObj(const std::string &filename, Eigen::MatrixXf &V, Eigen::MatrixXi &F) {
  std::cout << "Loading OBJ file .. ";

  struct obj_vertex {
    uint32_t p = (uint32_t) -1;
    uint32_t n = (uint32_t) -1;
    uint32_t uv = (uint32_t) -1;

    inline obj_vertex() { }

    inline std::vector<std::string> &str_tokenize(const std::string &s, char delim, std::vector<std::string> &elems, bool include_empty = false) {
      std::stringstream ss(s);
      std::string item;
      while (std::getline(ss, item, delim)) {
        if (!item.empty() || include_empty){
          elems.push_back(item);
        }
      }
      return elems;
    }

    inline std::vector<std::string> str_tokenize(const std::string &s, char delim, bool include_empty) {
      std::vector<std::string> elems;
      str_tokenize(s, delim, elems, include_empty);
      return elems;
    }

    inline uint32_t str_to_uint32_t(const std::string &str) {
      char *end_ptr = nullptr;
      uint32_t result = (uint32_t) strtoul(str.c_str(), &end_ptr, 10);
      if (*end_ptr != '\0') {
        throw std::runtime_error("Could not parse unsigned integer \"" + str + "\"");
      }
      return result;
    }

    inline obj_vertex(const std::string &string) {
      std::vector<std::string> tokens = str_tokenize(string, '/', true);

      if (tokens.size() < 1 || tokens.size() > 3)
      throw std::runtime_error("Invalid vertex data: \"" + string + "\"");

      p = str_to_uint32_t(tokens[0]);
    }

    inline bool operator==(const obj_vertex &v) const {
      return v.p == p && v.n == n && v.uv == uv;
    }
  };

  struct obj_vertexHash : std::unary_function<obj_vertex, size_t> {
    std::size_t operator()(const obj_vertex &v) const {
      size_t hash = std::hash<uint32_t>()(v.p);
      hash = hash * 37 + std::hash<uint32_t>()(v.uv);
      hash = hash * 37 + std::hash<uint32_t>()(v.n);
      return hash;
    }
  };

  typedef std::unordered_map<obj_vertex, uint32_t, obj_vertexHash> VertexMap;

  std::ifstream is(filename);
  if (is.fail())
  throw std::runtime_error("Unable to open OBJ file \"" + filename + "\"!");
  std::cout << "Loading \"" << filename << "\" .. ";
  std::cout.flush();

  std::vector<Eigen::Vector3f>   positions;
  std::vector<Eigen::Vector2f>   texcoords;
  std::vector<Eigen::Vector3f>   normals;
  std::vector<uint32_t>   indices;
  std::vector<obj_vertex> vertices;
  VertexMap vertexMap;

  std::string line_str;
  while (std::getline(is, line_str)) {
    std::istringstream line(line_str);

    std::string prefix;
    line >> prefix;

    if (prefix == "v") {
      Eigen::Vector3f p;
      line >> p.x() >> p.y() >> p.z();
      positions.push_back(p);
    }
    else if (prefix == "vt") {
      Eigen::Vector2f tc;
      line >> tc.x() >> tc.y();
      texcoords.push_back(tc);
    }
    else if (prefix == "vn") {
      Eigen::Vector3f n;
      line >> n.x() >> n.y() >> n.z();
      normals.push_back(n);
    }
    else if (prefix == "f") {
      std::string v1, v2, v3, v4;
      line >> v1 >> v2 >> v3 >> v4;
      obj_vertex tri[6];
      int nVertices = 3;

      tri[0] = obj_vertex(v1);
      tri[1] = obj_vertex(v2);
      tri[2] = obj_vertex(v3);

      if (!v4.empty()) {
        tri[3] = obj_vertex(v4);
        tri[4] = tri[0];
        tri[5] = tri[2];
        nVertices = 6;
      }
      for (int i=0; i<nVertices; ++i) {
        const obj_vertex &v = tri[i];
        VertexMap::const_iterator it = vertexMap.find(v);
        if (it == vertexMap.end()) {
          vertexMap[v] = (uint32_t) vertices.size();
          indices.push_back((uint32_t) vertices.size());
          vertices.push_back(v);
        }
        else {
          indices.push_back(it->second);
        }
      }
    }
  }

  F.resize(3, indices.size()/3);
  memcpy(F.data(), indices.data(), sizeof(uint32_t)*indices.size());

  V.resize(3, vertices.size());
  for (uint32_t i=0; i<vertices.size(); ++i)
  V.col(i) = positions.at(vertices[i].p-1);

  std::cout << "done. (V=" << V.cols() << ", F=" << F.cols() << ")" << std::endl;
}

void Loader::writeObj(const std::string &filename, const Eigen::MatrixXf &V, const Eigen::MatrixXi &F, const Eigen::MatrixXf &N, const Eigen::MatrixXf &Nf, const Eigen::MatrixXf &UV, const Eigen::MatrixXf &C) {

  std::cout << "Writing \"" << filename << "\" (V=" << V.cols()
  << ", F=" << F.cols() << ") .. ";
  std::cout.flush();
  std::ofstream os(filename);
  if (os.fail())
  throw std::runtime_error("Unable to open OBJ file \"" + filename + "\"!");
  if (N.size() > 0 && Nf.size() > 0)
  throw std::runtime_error("Please specify either face or vertex normals but not both!");

  for (uint32_t i=0; i<V.cols(); ++i)
  os << "v " << V(0, i) << " " << V(1, i) << " " << V(2, i) << std::endl;

  for (uint32_t i=0; i<N.cols(); ++i)
  os << "vn " << N(0, i) << " " << N(1, i) << " " << N(2, i) << std::endl;

  for (uint32_t i=0; i<Nf.cols(); ++i)
  os << "vn " << Nf(0, i) << " " << Nf(1, i) << " " << Nf(2, i) << std::endl;

  for (uint32_t i=0; i<UV.cols(); ++i)
  os << "vt " << UV(0, i) << " " << UV(1, i) << std::endl;

  /* Check for irregular faces */
  std::map<uint32_t, std::pair<uint32_t, std::map<uint32_t, uint32_t>>> irregular;
  size_t nIrregular = 0;

  for (uint32_t f=0; f<F.cols(); ++f) {
    if (F.rows() == 4) {
      if (F(2, f) == F(3, f)) {
        nIrregular++;
        auto &value = irregular[F(2, f)];
        value.first = f;
        value.second[F(0, f)] = F(1, f);
        continue;
      }
    }
    os << "f ";
    for (uint32_t j=0; j<F.rows(); ++j) {
      uint32_t idx = F(j, f);
      idx += 1;
      os << idx;
      if (Nf.size() > 0)
      idx = f + 1;
      os << "//" << idx << " ";
    }
    os << std::endl;
  }

  for (auto item : irregular) {
    auto face = item.second;
    uint32_t v = face.second.begin()->first, first = v, i = 0;
    os << "f ";
    while (true) {
      uint32_t idx = v + 1;
      os << idx;
      if (Nf.size() > 0)
      idx = face.first + 1;
      os << "//" << idx << " ";

      v = face.second[v];
      if (v == first || ++i == face.second.size())
      break;
    }
    os << std::endl;
  }

  std::cout << "done." << std::endl;
}