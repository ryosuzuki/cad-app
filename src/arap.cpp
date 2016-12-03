

#include "arap.h"


void ARAP::init(Eigen::MatrixXf &V, Eigen::MatrixXi &F, Eigen::VectorXi &b) {

  data.size = V.cols();
  data.b = b;

  data.F_ext = Eigen::MatrixXf::Zero(3, V.cols());

  Eigen::SparseMatrixS refMap, refMapDim;

  const Eigen::MatrixXf &refV = V;
  const Eigen::MatrixXi &refF = F;
  Eigen::SparseMatrixS L;

  cotmatrix(V, F, L);

  Eigen::SparseMatrixS CSM;

  covariance_scatter_matrix(refV, refF, ARAP_ENERGY_TYPE_ELEMENTS, CSM);

  Eigen::SparseMatrixS G_sum;
  Eigen::MatrixXi GG;
  Eigen::MatrixXi GF(3 F.cols());
  for (int f = 0; f < 3; f++) {
    Eigen::MatrixXi GFF;
    slice(G, F.row(f), GFF);
    GF.row(f) = GFF;
  }
  data.G = GG;

  Eigen::SparseMatrixXf G_sum_dim;

  main_quad_with_fixed_precompute(Q, b, data.solver_data);

}

void computeAdjacency(Eigen::MatrixXf &V, Eigen::MatrixXi &F, Eigen::MatrixXi &A) {

}

void computeWeightMatrix() {
  vector<Triplet<float> > weight_list;
  weight_list.reserve(3 * 7 * P_Num); // each vertex may have about 7 vertices connected

  for (int i=0; i<V.cols(); i++) {
    for (decltype(adj_list[i].size()) j = 0; j != adj_list[i].size(); ++j) {
      int id_j = adj_list[i][j];

      vector<int> share_vertex;
      find_share_vertex(i, id_j, share_vertex);

      float wij = 0;

      float p1 = V
      float p2
      float p3
      float p4
P_data[3 * i], &P_data[3 * id_j],
                                            &P_data[3 * share_vertex[0]], &P_data[3 * share_vertex[1]

      if (share_vertex.size() == 2)
        wij =

        float e4 = sqrt((p1[0]-p4[0])*(p1[0]-p4[0])+(p1[1]-p4[1])*(p1[1]-p4[1])+(p1[2]-p4[2])*(p1[2]-p4[2]));
        float e5 = sqrt((p4[0]-p2[0])*(p4[0]-p2[0])+(p4[1]-p2[1])*(p4[1]-p2[1])+(p4[2]-p2[2])*(p4[2]-p2[2]));
        beta_cos = fabs((e4*e4+e5*e5-e1*e1)/(2*e4*e5));



   float e1 = sqrt((p1[0]-p2[0])*(p1[0]-p2[0])+(p1[1]-p2[1])*(p1[1]-p2[1])+(p1[2]-p2[2])*(p1[2]-p2[2]));
    float e2 = sqrt((p1[0]-p3[0])*(p1[0]-p3[0])+(p1[1]-p3[1])*(p1[1]-p3[1])+(p1[2]-p3[2])*(p1[2]-p3[2]));
    float e3 = sqrt((p3[0]-p2[0])*(p3[0]-p2[0])+(p3[1]-p2[1])*(p3[1]-p2[1])+(p3[2]-p2[2])*(p3[2]-p2[2]));
    float alpha_cos = fabs((e3*e3+e2*e2-e1*e1)/(2*e3*e2));
    float beta_cos = 0;
    if (p4 != nullptr) {
    }



      compute_wij(&]);
      else wij = compute_wij(&P_data[3 * i], &P_data[3 * id_j], &P_data[3 * share_vertex[0]]);

      weight_list.push_back(Triplet<float>(i, id_j, wij));
      weight_list.push_back(Triplet<float>(i + P_Num, id_j + P_Num, wij));
      weight_list.push_back(Triplet<float>(i + 2 * P_Num, id_j + 2 * P_Num, wij));
    }
  }

  Weight.resize(3 * P_Num, 3 * P_Num);
  Weight.setFromTriplets(weight_list.begin(), weight_list.end());
}


void computeLaplacianMatrix(Eigen::MatrixXf &V, Eigen::MatrixXi &F, Eigen::SparseMatrix<float> &L) {

  SparseMatrix<float> weight = Weight;

  vector<Triplet<float> > weight_sum;
  weight_sum.reserve(3 * P_Num);

  for (int i = 0; i < V.cols(); i++) {

    for (int j = 0; j < A[i].size(); j++) {
      int id_j = adjList[]
    }

  }


  for (int i = 0; i != P_Num; ++i) {
    float wi = 0;

    for (decltype(adj_list[i].size()) j = 0; j != adj_list[i].size(); ++j)
    {
      int id_j = adj_list[i][j];

      if (is_hard_ctrs(i) == -1)
        wi += Weight.coeffRef(i, id_j);
      else
      {
        weight.coeffRef(i, id_j) = 0;
        weight.coeffRef(i + P_Num, id_j + P_Num) = 0;
        weight.coeffRef(i + 2 * P_Num, id_j + 2 * P_Num) = 0;

        wi = 1;
      }
    }

    if (is_soft_ctrs(i) != -1 && at == HARD_SOFT)
      wi += lambda / 2;

    weight_sum.push_back(Triplet<float>(i, i, wi));
    weight_sum.push_back(Triplet<float>(i + P_Num, i + P_Num, wi));
    weight_sum.push_back(Triplet<float>(i + 2 * P_Num, i + 2 * P_Num, wi));
  }

  SparseMatrix<float> Weight_sum(3 * P_Num, 3 * P_Num);
  Weight_sum.setFromTriplets(weight_sum.begin(), weight_sum.end());

  L =  Weight_sum - weight;

  chol.analyzePattern(L);
  chol.factorize(L);



  L.resize(V.cols(), V.cols());
  Eigen::MatrixXi edges;
  L.reserve(10 * V.cols());
  edges.resize(3, 2);
  edges << 1, 2, 2, 0, 0, 1;

  Eigen::Matrix<Scalar, Dynamic, Dynamic> C;
  cotmatrix_entries(V, F, C);

  vector<Triplet<Scalar> > IJV;
  IJV.reserve(F.rows()*edges.rows() * 4);

  for (int i = 0; i < F.cols(); i++) {
    for (int e = 0; e < edges.rows(); e++) {
      int source = F(edges(e, 0), i);
      int dest = F(edges(e, 1), i);
      IJV.push_back(Triplet<Scalar>(source, dest, C(i, e)));
      IJV.push_back(Triplet<Scalar>(dest, source, C(i, e)));
      IJV.push_back(Triplet<Scalar>(source, source, -C(i, e)));
      IJV.push_back(Triplet<Scalar>(dest, dest, -C(i, e)));
    }
  }
  L.setFromTriplets(IJV.begin(), IJV.end());

}



