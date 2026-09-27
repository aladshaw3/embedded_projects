/**
  This example is meant to just test the Eigen
  usage to verify expected behaviors.
*/

#ifdef GTEST
#include <gtest/gtest.h>

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/OrderingMethods>
#include <Eigen/Sparse>
#include <Eigen/SparseLU>
#include <Eigen/SparseQR>
#include <complex>
#include <iostream>
#include <unsupported/Eigen/NonLinearOptimization>  // NOLINT [build/include_order]
#include <vector>

// Importing the namespaces
using Eigen::Matrix3f;  ///< *3f = Set at compile time, of type float
using Eigen::Vector3f;  ///< *3d = Set at compile time, of type float

TEST(EigenIntegrationTest, BasicUsageTest) {
  Matrix3f M = Matrix3f::Random();
  Vector3f V(1, 2, 3);

  EXPECT_EQ(V.size(), 3);
  EXPECT_EQ(M.rows(), 3);
  EXPECT_EQ(M.cols(), 3);
}

/**
  This example shows how to use the Full Templated
  Matrix object. This is for more advanced control
  over how the matrix is formed and how it is defined
  at compile time.
*/

using Eigen::ColMajor;
using Eigen::Dynamic;
using Eigen::Matrix;

TEST(EigenIntegrationTest, ComplexVariableTest) {
  /**
    Below is the Syntax for the full templated Matrix
    -------------------------------------------------

    Matrix<typename Scalar,
        int RowsAtCompileTime,
        int ColsAtCompileTime,
        int Options = 0,
        int MaxRowsAtCompileTime = RowsAtCompileTime,
        int MaxColsAtCompileTime = ColsAtCompileTime>

    The 'Dynamic' Keyword can be used to replace `RowsAtCompileTime`
    and `ColsAtCompileTime`, so that this info can by dynamically
    allocated

    The `Options` is an int flag to determine how the data is stored
    in memory. Choices are: (i) `ColMajor` [default] and (ii) `RowMajor`.
  */
  // NOTE: Dense Matrix MAX size is 100x100
  Matrix<float, Dynamic, Dynamic, ColMajor, 100, 100> m;
  m.resize(10, 10);

  EXPECT_EQ(m.rows(), 10);
  EXPECT_EQ(m.cols(), 10);

  // NOTE: ONLY the first 3 items are REQUIRED
  //    (i.e., provide the type, rows, and cols)
  Matrix<int, 3, 4> m2;

  EXPECT_EQ(m2.rows(), 3);
  EXPECT_EQ(m2.cols(), 4);

  // Can create matrices of complex values
  Matrix<std::complex<double>, 5, 5> m3;

  EXPECT_EQ(m3.rows(), 5);
  EXPECT_EQ(m3.cols(), 5);

  // Use the `real` function to get the real portion of
  //  the complex number
  EXPECT_EQ(real(m3(0, 0)), 0);

  // Use the `imag` function to get the imaginary portion of
  //  the complex number
  EXPECT_EQ(imag(m3(0, 0)), 0);

  // Fill in m3
  for (unsigned int i = 0; i < m3.rows(); i++) {
    m3(i, i) = std::complex<double>(10.0 * static_cast<double>(i), 1.0);
  }

  // Vectors can be created as matrices
  Matrix<std::complex<double>, 5, 1> v3, res;

  // Fill in v3
  for (unsigned int i = 0; i < m3.rows(); i++) {
    v3(i, 0) = std::complex<double>(1, 0);
  }

  // Matrix multiplication
  res = m3 * v3;

  EXPECT_EQ(real(res(0, 0)), 0);

  // Dot product
  std::complex<double> d;
  d = res.dot(v3);

  EXPECT_EQ(real(d), 100);

  std::complex<double> norm;
  norm = res.norm();

  EXPECT_LT(fabs(real(norm) - 54.8179), 1e-3);
}

/**
  This example shows how to use the SparseMatrix object.
  For most practical problems, we will likely want to
  use SparseMatrix for the sake of memory efficiency.
  However, the Dense Matrix class is more operationally
  efficient (if the matrices are relatively small).

  Vectors will generally be dense and can be used in
  conjunction with SparseMatrix objects.
*/

// Defining T as Eigen::Triplet<double> for convienience
typedef Eigen::Triplet<double> T;

// Inserts coefficients into b
void insertCoefficient(int id, int i, int j, double w, std::vector<T>& coeffs,
                       Eigen::VectorXd& b,  // cppcheck-suppress constParameterReference
                       const Eigen::VectorXd& boundary) {
  int n = static_cast<int>(boundary.size());
  int id1 = i + j * n;

  if (i == -1 || i == n)
    b(id) -= w * boundary(j);  // constrained coefficient
  else if (j == -1 || j == n)
    b(id) -= w * boundary(i);  // constrained coefficient
  else
    coeffs.push_back(T(id, id1, w));  // unknown coefficient
}

// Builds the b vector
void buildProblem(std::vector<T>& coefficients, Eigen::VectorXd& b, int n) {
  b.setZero();
  Eigen::ArrayXd boundary = Eigen::ArrayXd::LinSpaced(n, 0, 3.14159).sin().pow(2);
  for (int j = 0; j < n; ++j) {
    for (int i = 0; i < n; ++i) {
      int id = i + j * n;
      insertCoefficient(id, i - 1, j, -1, coefficients, b, boundary);
      insertCoefficient(id, i + 1, j, -1, coefficients, b, boundary);
      insertCoefficient(id, i, j - 1, -1, coefficients, b, boundary);
      insertCoefficient(id, i, j + 1, -1, coefficients, b, boundary);
      insertCoefficient(id, i, j, 4, coefficients, b, boundary);
    }
  }
}

TEST(EigenIntegrationTest, SparseMatrixTest) {
  // ------------ Test 1: Setting up and Solving Large Sparse System ----------------------------------
  int n = 300;    // size of the image
  int m = n * n;  // number of unknowns (=number of pixels)

  // Assembly: Collect RHS of Ax = b
  std::vector<T> coefficients;  // list of non-zeros coefficients
  Eigen::VectorXd b(m);         // the right hand side-vector resulting from the constraints
  buildProblem(coefficients, b, n);

  // Formulate the Sparse version of A
  //  NOTE: This matrix is far too large to create a dense form of
  Eigen::SparseMatrix<double> A(m, m);

  // This forms a Tridiagonal Matrix
  A.setFromTriplets(coefficients.begin(), coefficients.end());

  // Solving: Using Cholesky factorization on a SparseMatrix
  Eigen::SimplicialCholesky<Eigen::SparseMatrix<double>> chol(A);  // performs a Cholesky factorization of A
  Eigen::VectorXd x = chol.solve(b);  // use the factorization to solve for the given right hand side

  // ------------------- Test 2: Basic Matrix Creation ----------------------------------

  // SparseMatrix templated class has 3 args (last 2 are optional)
  /**
        SparseMatrix< type, Options, StorageIndex >

            type: float, double, complex, etc.
            Options:  ColMajor [default], RowMajor
            StorageIndex: int [default], short, etc.
  */

  // Declare a sparse matrix
  Eigen::SparseMatrix<double> B;

  // Size is intially zero
  EXPECT_EQ(B.rows(), 0);
  EXPECT_EQ(B.cols(), 0);

  // Specify size (without allocation)
  B.conservativeResize(10, 10);
  EXPECT_EQ(B.rows(), 10);
  EXPECT_EQ(B.cols(), 10);

  // There are 2 Ways to insert data into a SparseMatrix

  //  (1) =============== Using 'insert' ============================
  //        This method is generally less efficient, but easy to use.
  B.insert(0, 0) = 1;
  B.insert(1, 1) = 1;

  // Use the .`coeff` function to grab value from matrix
  //    (if value does not exist, it returns 0)
  EXPECT_EQ(B.coeff(0, 0), 1);

  //        This can be improved by pre-allocating memory space
  //        for a known number of non-zeros.
  unsigned int cols = 10;
  unsigned int nonzeros = 3;
  //        Here, we are reserving 3 non-zeros per colomn
  B.reserve(Eigen::VectorXi::Constant(cols, nonzeros));

  //        After reserving space, iterate through and insert
  //        remaining elements. (NOTE: `insert` is only valid
  //        if the element does NOT already exist).
  for (unsigned int i = 2; i < cols; i++) {
    B.insert(i, i) = 1;
  }

  //      Lastly, you can re-compress the matrix after formation
  //      (During a typical insertion, more than what is needed
  //      usually allocated to make the insertion faster).
  B.makeCompressed();
  EXPECT_EQ(B.rows(), 10);
  EXPECT_EQ(B.cols(), 10);

  //  (2) =============== Using 'setFromTriplets' ============================
  //        This method is usually more complicated, but more efficient
  Eigen::SparseMatrix<double> C;

  // Specify size (without allocation)
  C.conservativeResize(10, 10);

  // Recall from earlier, we defined `T` as `Eigen::Triplet<double>`
  //  We can use a list/vector of triplets to specify values.
  //  A Triplet is constructed with 3 args
  //        T( row_index, col_index, value )
  //  We create a list of these, then use `setFromTriplets` to fill
  //    the SparseMatrix...
  std::vector<T> list;
  list.resize(C.rows());
  for (unsigned int i = 0; i < C.rows(); i++) {
    list[i] = T(i, i, 2);  // Fill diagonal with 2s
  }
  C.setFromTriplets(list.begin(), list.end());

  EXPECT_EQ(C.coeff(0, 0), 2);

  // ------------------- Test 3: Iterating Through SparseMatrix ----------------------------------
  for (unsigned int k = 0; k < C.outerSize(); ++k) {
    for (Eigen::SparseMatrix<double>::InnerIterator it(C, k); it; ++it) {
      std::cout << "C(" << it.row() << "," << it.col() << ") = " << it.value() << std::endl;

      // NOTE: Inner Index...
      std::cout << "Inner Index: " << it.index() << std::endl << std::endl;
    }
  }

  // Other Notes:
  /**
      Addition of a SparseMatrix by a DenseMatrix results in a DenseMatrix
      Addition of a SparseMatrix by a SparseMatrix results in a SparseMatrix
      Multiplication of a SparseMatrix by a DenseMatrix results in a DenseMatrix
      Multiplication of a SparseMatrix by a SparseMatrix results in a SparseMatrix
      The 'prune' function removes values in SparseMatrix that are 'close to zero'
  */
}

/**
  This example shows how to use the Basic Linear Algebra
  solvers available in Eigen for both Dense and Sparse
  Matrix objects.

  The Basic Linear Algebra solvers cover:
    (a) Direct solvers (LU, QR, etc)
    (b) Eigen Value solvers
    (c) Iterative solvers (simple)

  Advanced Solvers will include matrix-free methods.
*/

// Helper function to fill in the b vector for BCs of 3D Laplacian
void fillNatural3DLaplacian_BCs(unsigned int m, Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>& b) {
  b.resize(m * m * m, 1);
  b(0, 0) = -1;
  b(m * m - 1, 0) = -1;
  b(m * m * m - 1, 0) = -1;
}

// Helper function to fill in a 3D sparse Laplacian
void fillNatural3DLaplacian(unsigned int m, Eigen::SparseMatrix<double>& S) {
  // Setup the matrix size
  unsigned int N = m * m * m;
  S.conservativeResize(N, N);

  // Create space for 7 non-zeros per row/col
  S.reserve(Eigen::VectorXi::Constant(N, 7));

  // Create piviots for marking when things have changed in a row/col
  unsigned int r = m;
  unsigned int r2 = m * m;
  unsigned int r3 = r2;
  unsigned int r4 = 0;

  // Loop along each row
  for (unsigned int i = 0; i < N; i++) {
    // If statements for tridiagonal portion
    S.insert(i, i) = 6;
    if (i == 0) {
      S.insert(i, i + 1) = -1;
    } else if (i == N - 1) {
      S.insert(i, i - 1) = -1;
    } else if (i == r - 1) {
      S.insert(i, i - 1) = -1;
    } else if (i == r) {
      S.insert(i, i + 1) = -1;
      r = r + m;
    } else {
      S.insert(i, i - 1) = -1;
      S.insert(i, i + 1) = -1;
    }

    // If statements for 2nd diagonal bands
    if (i > m - 1) {
      if (i <= r3 - 1) {
        S.insert(i, i - m) = -1;
      } else if (i > r3 - 1) {  // cppcheck-suppress knownConditionTrueFalse
        r4 = r4 + 1;
        if (r4 == m - 1) {
          r3 = r2;
          r4 = 0;
        }
      }
    }
    if (i <= N - m - 1 && i <= r2 - m - 1) {
      S.insert(i, i + m) = -1;
    }
    if (i == r2 - 1) {
      r2 = r2 + (m * m);
    }

    // If statements for 3rd diagonal bands
    if (i > (m * m) - 1) {
      S.insert(i, i - (m * m)) = -1;
    }
    if (i <= N - (m * m) - 1) {
      S.insert(i, i + (m * m)) = -1;
    }
  }

  // Lastly, compress the representation
  S.makeCompressed();
}

TEST(EigenIntegrationTest, BasicLinearAlgebraTest) {
  // ------------------- Test 1: Dense Matrix Solvers ----------------------
  //    How to solve Ax = b using LU and QR decomposition

  // Declare your matrices (any way you want)
  Eigen::Matrix<float, 3, 3> A;
  Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> b;
  b.resize(3, 1);
  Eigen::Matrix<float, 3, 1> x1, x2, x3, x4;

  // Fill in the matrices
  A << 1, 2, 3, 4, 5, 6, 7, 8, 10;
  b << 3, 3, 4;

  // Call a solver
  //    (a) Column Pivioting QR - Slower, but more stable
  x1 = A.colPivHouseholderQr().solve(b);

  //    (b) Full Pivioting LU - Much slower, but more stable
  x2 = A.fullPivLu().solve(b);

  //    (c) Standard QR - faster, but less stable
  x3 = A.householderQr().solve(b);

  //    (b) Partial Pivioting LU - Much faster, but less stable
  x4 = A.partialPivLu().solve(b);

  // Validate solution
  EXPECT_LT((x1 - x2).norm(), 1e-3);
  EXPECT_LT((x2 - x3).norm(), 1e-3);
  EXPECT_LT((x3 - x4).norm(), 1e-3);

  // Several other solvers, but these are the most generic
  // Full list here: https://eigen.tuxfamily.org/dox/group__TopicLinearAlgebraDecompositions.html

  // ------------------- Test 2: Dense Matrix Eigenvalues ----------------------
  //    How to find the Eigenvalues/Eigenvectors of A
  //        NOTE: The Eigenvalues && Eigenvectors on return are of type std::complex< T >
  Eigen::Matrix<std::complex<float>, 3, 1> lam;
  Eigen::Matrix<std::complex<float>, 3, 3> lam_mat;

  // Declare an EigenSolver object of same type as the matrix object
  // we are solving the eigenvalues of.
  //
  //    Other Options include:
  //          SelfAdjointEigenSolver - For matrices that are Self-adjoint/Symmetric Only
  //                (Fast, but conditional on symmetry)
  //          EigenSolver - For matrices that are Square and Real
  //                (Average, but real matrices only)
  //          GeneralizedSelfAdjointEigenSolver - For matrices that are Square
  //                (Fast, but only for A*v = lam*B*v)
  //          ComplexEigenSolver - For matrices that are Square
  //                (Slow, but most generic)

  // Either of these work for this problem...
  Eigen::EigenSolver<Eigen::Matrix<float, 3, 3>> e_solver;
  // Eigen::ComplexEigenSolver< Eigen::Matrix<float, 3, 3> > e_solver;

  // Call the 'compute' method passing the matrix to solve 'A'
  //  and boolean determining whether or not to also compute
  //  the eigenvectors. Select 'true' to get the eigenvectors.
  e_solver.compute(A, true);

  // After the solve, you can retrive the eigenvalues and
  // eigenvectors with `.eigenvalues()` and `.eigenvectors()`
  lam = e_solver.eigenvalues();
  lam_mat = e_solver.eigenvectors();
  EXPECT_LT(fabs(static_cast<double>(lam.norm()) - 16.7332), 1e-3);

  // ------------------- Test 3: Sparse Matrix Solvers ----------------------
  //    How to solve As*xs = bs using Sparse versions of LU and QR decomposition
  Eigen::SparseMatrix<double> As;
  int m = 4;  // 3D size var
  fillNatural3DLaplacian(m, As);

  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> bs, xs;
  xs.resize(m * m * m, 1);
  fillNatural3DLaplacian_BCs(m, bs);

  //    (a) Solve with LU
  //        Object MUST contain the type of matrix we are solving, as well
  //        as the OrderMethod to use. The OrderMethods are as follows:
  //            COLAMDOrdering<int>  - Column Major [Default]
  //            AMDOrdering<int>     - Requires Symmetry
  //            NaturalOrdering<int> - Ordering of the matrix
  Eigen::SparseLU<Eigen::SparseMatrix<double>> lu_solver;
  // Analyze the pattern - DO ONLY ONCE if sparsity pattern doesn't change
  //      for this step the numerical values of A are not used
  lu_solver.analyzePattern(As);

  // This is the actual 'solve' step, the 'solve' grabs the solution (if available)
  lu_solver.factorize(As);
  EXPECT_EQ(lu_solver.info(), Eigen::ComputationInfo::Success);

  xs = lu_solver.solve(bs);
  EXPECT_EQ(lu_solver.info(), Eigen::ComputationInfo::Success);

  double norm = (As * xs - bs).norm();
  EXPECT_LT(norm, 1e-6);

  //    (b) Solve with QR
  //        Object MUST contain the type of matrix we are solving, as well
  //        as the OrderMethod to use. The OrderMethods are as follows:
  //            COLAMDOrdering<int>  - Column Major [Default]
  //            AMDOrdering<int>     - Requires Symmetry
  //            NaturalOrdering<int> - Ordering of the matrix
  Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> qr_solver;
  // Analyze the pattern - DO ONLY ONCE if sparsity pattern doesn't change
  //      for this step the numerical values of A are not used
  qr_solver.analyzePattern(As);

  // This is the actual 'solve' step, the 'solve' grabs the solution (if available)
  qr_solver.factorize(As);
  EXPECT_EQ(qr_solver.info(), Eigen::ComputationInfo::Success);

  xs = qr_solver.solve(bs);
  EXPECT_EQ(qr_solver.info(), Eigen::ComputationInfo::Success);

  norm = (As * xs - bs).norm();
  EXPECT_LT(norm, 1e-6);

  // ------------------- Test 4: Sparse Matrix Solvers ----------------------
  //    How to solve As*xs = bs using PCG, BiCGSTAB, and LSCG

  //      (a) Using PCG
  //            ConjugateGradient< Matrix type, TrigPart, Precon>
  //                TrigPart = Lower [default], Lower|Upper, Upper
  //                Precon = DiagonalPreconditioner [default]
  Eigen::ConjugateGradient<Eigen::SparseMatrix<double>, Eigen::Lower | Eigen::Upper> cg;

  // As before, you call a 'compute()' function before calling solve
  cg.compute(As);
  xs = cg.solve(bs);
  EXPECT_LT(cg.error(), 1e-6);

  //      (b) Using BiCGSTAB
  //            BiCGSTAB< Matrix type, Precon>
  //                Precon = DiagonalPreconditioner [default]
  Eigen::BiCGSTAB<Eigen::SparseMatrix<double>> bicg;

  // As before, you call a 'compute()' function before calling solve
  bicg.compute(As);
  xs = bicg.solve(bs);  // cppcheck-suppress redundantAssignment
  EXPECT_LT(bicg.error(), 1e-6);

  //      (c) Using LSCG
  //            LeastSquaresConjugateGradient< Matrix type, Precon>
  //                Precon = DiagonalPreconditioner [default]
  Eigen::LeastSquaresConjugateGradient<Eigen::SparseMatrix<double>> lscg;

  // As before, you call a 'compute()' function before calling solve
  lscg.compute(As);
  xs = lscg.solve(bs);  // cppcheck-suppress redundantAssignment
  EXPECT_LT(lscg.error(), 1e-6);
}

/**
  This example shows how to use the Matrix-Free linear solvers,
  including the 'Unofficially' supported methods like GMRES.
*/

/**
  Iterative solvers such as ConjugateGradient and BiCGSTAB can be used in a matrix
  free context. To this end, user must provide a wrapper class inheriting EigenBase<>
  and implementing the following methods:

    - Index rows() and Index cols(): returns number of rows and columns respectively
    - operator* with your type and an Eigen dense column vector
        (its actual implementation goes in a specialization of the
        internal::generic_product_impl class)

  Eigen::internal::traits<> must also be specialized for the wrapper type.

  Here is a complete example wrapping an Eigen::SparseMatrix:
*/

class MatrixReplacement;
using Eigen::SparseMatrix;

namespace Eigen {
namespace internal {
// MatrixReplacement looks-like a SparseMatrix, so let's inherits its traits:
//      NOTE: This is specifically for a Matrix like object of type 'double'
//            That typdef is specific to this wrapper
template <>
struct traits<MatrixReplacement> : public Eigen::internal::traits<Eigen::SparseMatrix<double>> {};
}  // namespace internal
}  // namespace Eigen

// Example of a matrix-free wrapper from a user type to Eigen's compatible type
// For the sake of simplicity, this example simply wrap a Eigen::SparseMatrix.
class MatrixReplacement : public Eigen::EigenBase<MatrixReplacement> {
 public:
  // Required typedefs, constants, and method:
  typedef double Scalar;
  typedef double RealScalar;
  typedef int StorageIndex;
  enum { ColsAtCompileTime = Eigen::Dynamic, MaxColsAtCompileTime = Eigen::Dynamic, IsRowMajor = false };

  // Required method to override
  Index rows() const { return mp_mat->rows(); }
  Index cols() const { return mp_mat->cols(); }

  // Required operator to override
  //    NOTE: This is not the actual implementation section
  template <typename Rhs>
  Eigen::Product<MatrixReplacement, Rhs, Eigen::AliasFreeProduct> operator*(const Eigen::MatrixBase<Rhs>& x) const {
    return Eigen::Product<MatrixReplacement, Rhs, Eigen::AliasFreeProduct>(*this, x.derived());
  }

  // Custom API:
  MatrixReplacement() : mp_mat(0) {}

  void attachMyMatrix(const SparseMatrix<double>& mat) { mp_mat = &mat; }
  const SparseMatrix<double> my_matrix() const { return *mp_mat; }

 private:
  const SparseMatrix<double>* mp_mat;
};

// Implementation of MatrixReplacement * Eigen::DenseVector though a specialization of internal::generic_product_impl:
namespace Eigen {
namespace internal {

// This is where the actual implementation of the Matrix Multiply action is done
template <typename Rhs>
struct generic_product_impl<MatrixReplacement, Rhs, SparseShape, DenseShape,
                            GemvProduct>  // GEMV stands for matrix-vector
    : generic_product_impl_base<MatrixReplacement, Rhs, generic_product_impl<MatrixReplacement, Rhs>> {
  typedef typename Product<MatrixReplacement, Rhs>::Scalar Scalar;

  template <typename Dest>
  static void scaleAndAddTo(Dest& dst, const MatrixReplacement& lhs, const Rhs& rhs, const Scalar& alpha) {
    // This method should implement "dst += alpha * lhs * rhs" inplace,
    // however, for iterative solvers, alpha is always equal to 1, so let's not bother about it.
    assert(alpha == Scalar(1) && "scaling is not implemented");
    EIGEN_ONLY_USED_FOR_DEBUG(alpha);

    // Here we could simply call dst.noalias() += lhs.my_matrix() * rhs,
    // but let's do something fancier (and less efficient):
    // ------------- Manual multiply-------------
    //      NOTE: This is very, very slow
    /**
    for(Index i=0; i<lhs.cols(); ++i)
    {
      dst += rhs(i) * lhs.my_matrix().col(i);
    }
    */
    // ------------- Quick multiply (because it is just a wrapper to SparseMatrix) -------------
    dst.noalias() += lhs.my_matrix() * rhs;
  }
};

}  // namespace internal
}  // namespace Eigen

TEST(EigenIntegrationTest, UnsupportedLinearMethods) {
  int n = 50;
  Eigen::SparseMatrix<double> S;
  fillNatural3DLaplacian(n, S);

  MatrixReplacement A;
  A.attachMyMatrix(S);

  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> b, x;
  x.resize(n * n * n, 1);
  fillNatural3DLaplacian_BCs(n, b);

  // Solve Ax = b using various iterative solver with matrix-free version:
  {
    Eigen::ConjugateGradient<MatrixReplacement, Eigen::Lower | Eigen::Upper, Eigen::IdentityPreconditioner> cg;
    cg.setTolerance(1e-6);
    cg.compute(A);
    x = cg.solve(b);  // cppcheck-suppress redundantAssignment
    EXPECT_LT(cg.error(), 1e-6);
  }

  {
    Eigen::ConjugateGradient<Eigen::SparseMatrix<double>, Eigen::Lower | Eigen::Upper, Eigen::IdentityPreconditioner>
        cg;
    cg.setTolerance(1e-6);
    cg.compute(S);
    x = cg.solve(b);  // cppcheck-suppress redundantAssignment
    EXPECT_LT(cg.error(), 1e-6);
  }

  {
    Eigen::BiCGSTAB<MatrixReplacement, Eigen::IdentityPreconditioner> bicg;
    bicg.setTolerance(1e-6);
    bicg.compute(A);
    x = bicg.solve(b);  // cppcheck-suppress redundantAssignment
    EXPECT_LT(bicg.error(), 1e-6);
  }

  {
    Eigen::BiCGSTAB<Eigen::SparseMatrix<double>, Eigen::IdentityPreconditioner> bicg;
    bicg.setTolerance(1e-6);
    bicg.compute(S);
    x = bicg.solve(b);  // cppcheck-suppress redundantAssignment
    EXPECT_LT(bicg.error(), 1e-6);
  }

  {
    Eigen::GMRES<MatrixReplacement, Eigen::IdentityPreconditioner> gmres;
    gmres.setTolerance(1e-6);
    gmres.setMaxIterations(100);
    gmres.set_restart(50);
    gmres.compute(A);
    x = gmres.solve(b);  // cppcheck-suppress redundantAssignment
    EXPECT_LT(gmres.error(), 1e-6);
  }

  {
    Eigen::GMRES<Eigen::SparseMatrix<double>, Eigen::IdentityPreconditioner> gmres;
    gmres.setTolerance(1e-6);
    gmres.setMaxIterations(100);
    gmres.set_restart(50);
    gmres.compute(S);
    x = gmres.solve(b);  // cppcheck-suppress redundantAssignment
    EXPECT_LT(gmres.error(), 1e-6);
  }

  {
    Eigen::DGMRES<MatrixReplacement, Eigen::IdentityPreconditioner> gmres;
    gmres.setTolerance(1e-6);
    gmres.setMaxIterations(1000);
    gmres.set_restart(50);
    gmres.compute(A);
    x = gmres.solve(b);  // cppcheck-suppress redundantAssignment
    EXPECT_LT(gmres.error(), 1e-6);
  }

  {
    Eigen::DGMRES<Eigen::SparseMatrix<double>, Eigen::IdentityPreconditioner> gmres;
    gmres.setTolerance(1e-6);
    gmres.setMaxIterations(1000);
    gmres.set_restart(50);
    gmres.compute(S);
    x = gmres.solve(b);  // cppcheck-suppress redundantAssignment
    EXPECT_LT(gmres.error(), 1e-6);
  }

  {
    Eigen::MINRES<MatrixReplacement, Eigen::Lower | Eigen::Upper, Eigen::IdentityPreconditioner> minres;
    minres.setTolerance(1e-6);
    minres.compute(A);
    x = minres.solve(b);  // cppcheck-suppress redundantAssignment
    EXPECT_LT(minres.error(), 1e-6);
  }

  {
    Eigen::MINRES<Eigen::SparseMatrix<double>, Eigen::Lower | Eigen::Upper, Eigen::IdentityPreconditioner> minres;
    minres.setTolerance(1e-6);
    minres.compute(S);
    x = minres.solve(b);  // cppcheck-suppress redundantAssignment
    EXPECT_LT(minres.error(), 1e-6);
  }
}

/**
  This example shows how to use the Non-linear solvers from the
  'unsupported' section of Eigen.

  NOTE: The NonLinearOptimization Lib REQUIRES the use of Dense Matrices
*/

// Functor (template of a function to be used by Non-Linear Solvers)
// --------------------------------------------------------------------------------
template <typename Scalar_, int NX = Eigen::Dynamic, int NY = Eigen::Dynamic>
struct Functor {
  typedef Scalar_ Scalar;
  enum { InputsAtCompileTime = NX, ValuesAtCompileTime = NY };
  typedef Eigen::Matrix<Scalar, InputsAtCompileTime, 1> InputType;
  typedef Eigen::Matrix<Scalar, ValuesAtCompileTime, 1> ValueType;
  typedef Eigen::Matrix<Scalar, ValuesAtCompileTime, InputsAtCompileTime> JacobianType;

  const int m_inputs, m_values;

  Functor() : m_inputs(InputsAtCompileTime), m_values(ValuesAtCompileTime) {}
  Functor(int inputs, int values) : m_inputs(inputs), m_values(values) {}

  int inputs() const { return m_inputs; }
  int values() const { return m_values; }

  // REQUIRED override
  virtual int operator()(const InputType& x, ValueType& fvec) const;

  // Only needed if you want to formulate own Jacobian
  int df(const InputType& x, JacobianType& fjac) const;
};
//================================================================================

// ============== Functor with hand-coded Jacobian =========================
struct hybrj_functor : Functor<double> {
  hybrj_functor(void) : Functor<double>(9, 9) {}

  // Evaluation Function (F(x) = ...)
  int operator()(const Eigen::VectorXd& x, Eigen::VectorXd& fvec) const {
    double temp, temp1, temp2;  // cppcheck-suppress variableScope
    const Eigen::VectorXd::Index n = x.size();
    assert(fvec.size() == n);
    for (Eigen::VectorXd::Index k = 0; k < n; k++) {
      temp = (3. - 2. * x[k]) * x[k];
      temp1 = 0.;
      if (k) temp1 = x[k - 1];
      temp2 = 0.;
      if (k != n - 1) temp2 = x[k + 1];
      fvec[k] = temp - temp1 - 2. * temp2 + 1.;
    }
    return 0;
  }

  // Jacobian Function (dF/dx = ...)
  int df(const Eigen::VectorXd& x, Eigen::MatrixXd& fjac) const {
    const Eigen::VectorXd::Index n = x.size();
    assert(fjac.rows() == n);
    assert(fjac.cols() == n);
    for (Eigen::VectorXd::Index k = 0; k < n; k++) {
      for (Eigen::VectorXd::Index j = 0; j < n; j++) fjac(k, j) = 0.;
      fjac(k, k) = 3. - 4. * x[k];
      if (k) fjac(k, k - 1) = -1.;
      if (k != n - 1) fjac(k, k + 1) = -2.;
    }
    return 0;
  }
};
// =============================================================================

// ================== Function to test the above solve ===========================
int testHybrj() {
  const int n = 9;
  int info;
  Eigen::VectorXd x(n);

  /* the following starting values provide a rough fit. */
  x.setConstant(n, -1.);

  // do the computation
  hybrj_functor functor;
  Eigen::HybridNonLinearSolver<hybrj_functor> solver(functor);
  info = solver.solve(x);
  EIGEN_UNUSED_VARIABLE(info);

  // check return value
  EXPECT_EQ(info, 1);

  // check norm
  EXPECT_LT(solver.fvec.blueNorm(), 1e-6);

  return info;
}
//==============================================================================

// ============== Functor WITHOUT hand-coded Jacobian =========================
struct hybrd_functor : Functor<double> {
  hybrd_functor(void) : Functor<double>(9, 9) {}
  int operator()(const Eigen::VectorXd& x, Eigen::VectorXd& fvec) const {
    double temp, temp1, temp2;  // cppcheck-suppress variableScope
    const Eigen::VectorXd::Index n = x.size();

    assert(fvec.size() == n);
    for (Eigen::VectorXd::Index k = 0; k < n; k++) {
      temp = (3. - 2. * x[k]) * x[k];
      temp1 = 0.;
      if (k) temp1 = x[k - 1];
      temp2 = 0.;
      if (k != n - 1) temp2 = x[k + 1];
      fvec[k] = temp - temp1 - 2. * temp2 + 1.;
    }
    return 0;
  }
};
//=============================================================================

// ================== Function to test the above solve ===========================
int testHybrd() {
  const int n = 9;
  int info;
  Eigen::VectorXd x;

  /* the following starting values provide a rough fit. */
  x.setConstant(n, -1.);

  // do the computation
  hybrd_functor functor;
  Eigen::HybridNonLinearSolver<hybrd_functor> solver(functor);
  solver.parameters.nb_of_subdiagonals = 1;
  solver.parameters.nb_of_superdiagonals = 1;
  solver.diag.setConstant(n, 1.);
  solver.useExternalScaling = true;
  info = solver.solveNumericalDiff(x);  // Call this instead of '.solve(x)'
  EIGEN_UNUSED_VARIABLE(info);

  // check return value
  EXPECT_EQ(info, 1);

  // check norm
  EXPECT_LT(solver.fvec.blueNorm(), 1e-6);

  return info;
}
//=============================================================================

TEST(EigenIntegrationTest, UnsupportedNonlinearMethods) {
  EXPECT_EQ(testHybrj(), 1);
  EXPECT_EQ(testHybrd(), 1);
}

#endif
