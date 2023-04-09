#include "../../include/benchmarks/spmv.h"
#include <gtest/gtest.h>

using namespace SPMV;

TEST(ParallelFor, SpmvTest) {
  auto dense = SPMV::GenDenseMatrix<int64_t>(1<<10, 1<<10);

  auto sparse = DenseToSparse(dense);
  auto x = GenVector<int64_t>(dense.Dimensions.Columns);

  std::vector<int64_t> y_ref(dense.Dimensions.Rows);
  MultiplyMatrix(dense, x, y_ref);

  std::vector<int64_t> y(dense.Dimensions.Rows);
  MultiplyMatrix(sparse, x, y);
  for (size_t i = 0; i != dense.Dimensions.Rows; ++i) {
    EXPECT_EQ(y[i], y_ref[i]);
  }
}
