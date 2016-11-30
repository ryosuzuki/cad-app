
#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <Eigen/Core>
#include <Eigen/Geometry>

class Loader {
public:

void loadObj(const std::string &filename, Eigen::MatrixXf &V, Eigen::MatrixXi &F);

void writeObj(const std::string &filename, const Eigen::MatrixXf &V, const Eigen::MatrixXi &F, const Eigen::MatrixXf &N, const Eigen::MatrixXf &Nf, const Eigen::MatrixXf &UV, const Eigen::MatrixXf &C);

};
