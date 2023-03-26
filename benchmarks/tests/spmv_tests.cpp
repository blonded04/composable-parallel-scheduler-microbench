#include "spmv.h"
#include <gtest/gtest.h>

using namespace SPMV;

TEST(ParallelFor, SpmvTestUnbalanced) {
  // todo: gen dense matrix
  DenseMatrix<uint64_t> dense(1 << 10, 1 << 10);
  for (size_t i = 0; i != dense.Dimensions.Rows; ++i) {
    for (size_t j = 0; j != dense.Dimensions.Columns; ++j) {
      dense.Data[i][j] = i + j;
    }
  }

  auto sparse = DenseToSparse(dense);
  auto x = GenVector<uint64_t>(dense.Dimensions.Columns);

  std::vector<uint64_t> y_ref(dense.Dimensions.Rows);
  MultiplyMatrix(dense, x, y_ref);

  std::vector<uint64_t> y(dense.Dimensions.Rows);
  MultiplyMatrix(sparse, x, y);
  for (size_t i = 0; i != dense.Dimensions.Rows; ++i) {
    EXPECT_EQ(y[i], y_ref[i]);
  }
}
