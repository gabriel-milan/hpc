/* A pure C/C++ version of a Gauss-Siedel Laplacian solver to test the
speed of a C program versus that of doing it with
Python/Numeric/Weave. */
#include <iostream>
#include <cmath>
#include <time.h>
#define SECS_PER_TICK (1.0 / CLOCKS_PER_SEC)
typedef double Real;
inline double seconds(void)
{
  return ((double)clock()) * SECS_PER_TICK;
}
inline Real BC(Real x, Real y)
{
  return (x * x - y * y);
}

struct Grid
{
  Real dx, dy;
  int nx, ny;
  Real **u;
  Grid(const int n_x = 10, const int n_y = 10);
  ~Grid();
  void setBCFunc(Real (*f)(const Real, const Real));
  /*Real computeError();*/
};
Grid ::Grid(const int n_x, const int n_y) : nx(n_x), ny(n_y)
{
  dx = 1.0 / Real(nx - 1);
  dy = 1.0 / Real(ny - 1);
  u = new Real *[nx];
  for (int i = 0; i < nx; ++i)
  {
    u[i] = new double[ny];
  }
  for (int i = 0; i < nx; ++i)
  {
    for (int j = 0; j < ny; ++j)
    {
      u[i][j] = 0.0;
    }
  }
}

Grid ::~Grid()
{
  for (int i = 0; i < nx; ++i)
  {
    delete[] u[i];
  }
  delete[] u;
}
void Grid ::setBCFunc(Real (*f)(const Real, const Real))
{
  Real xmin, ymin, xmax, ymax, x, y;
  xmin = 0.0;
  ymin = 0.0;
  xmax = 1.0;
  ymax = 1.0;
  /* Left and right sides. */
  for (int j = 0; j < ny; ++j)
  {
    y = j * dy;
    u[0][j] = f(xmin, y);
    u[nx - 1][j] = f(xmax, y);
  }
  /* Top and bottom sides. */
  for (int i = 0; i < nx; ++i)
  {
    x = i * dx;
    u[i][0] = f(x, ymin);
    u[i][ny - 1] = f(x, ymax);
  }
}

struct LaplaceSolver
{
  Grid *g;
  LaplaceSolver(Grid *g);
  ~LaplaceSolver();
  void initialize();
  Real timeStep(const Real dt = 0.0);
  Real solve(const int n_iter = 0, const Real eps = 1e-16);
  Real dx2;
  Real dy2;
  Real mul_cte;
};
LaplaceSolver ::LaplaceSolver(Grid *grid)
{
  g = grid;
  initialize();
}
LaplaceSolver::~LaplaceSolver()
{
}

void LaplaceSolver ::initialize()
{
  dx2 = g->dx * g->dx;
  dy2 = g->dy * g->dy;
  mul_cte = 0.5 / (dx2 + dy2);
}
Real LaplaceSolver ::timeStep(const Real dt)
{
  Real tmp;
  Real err = 0.0;
  int nx = g->nx;
  int ny = g->ny;
  Real **u = g->u;
  // Red-black ordering
  // Red sweep (part 1)
  for (int i = 1; i < nx - 1; i += 2)
  {
    for (int j = 1; j < ny - 1; j += 2)
    {
      tmp = u[i][j];
      u[i][j] = ((u[i - 1][j] + u[i + 1][j]) * dy2 +
                 (u[i][j - 1] + u[i][j + 1]) * dx2) *
                mul_cte;
      err += (u[i][j] - tmp) * (u[i][j] - tmp);
    }
  }
  // Red sweep (part 2)
  for (int i = 2; i < nx - 1; i += 2)
  {
    for (int j = 2; j < ny - 1; j += 2)
    {
      tmp = u[i][j];
      u[i][j] = ((u[i - 1][j] + u[i + 1][j]) * dy2 +
                 (u[i][j - 1] + u[i][j + 1]) * dx2) *
                mul_cte;
      err += (u[i][j] - tmp) * (u[i][j] - tmp);
    }
  }
  // Black sweep (part 1)
  for (int i = 1; i < nx - 1; i += 2)
  {
    for (int j = 2; j < ny - 1; j += 2)
    {
      tmp = u[i][j];
      u[i][j] = ((u[i - 1][j] + u[i + 1][j]) * dy2 +
                 (u[i][j - 1] + u[i][j + 1]) * dx2) *
                mul_cte;
      err += (u[i][j] - tmp) * (u[i][j] - tmp);
    }
  }
  // Black sweep (part 2)
  for (int i = 2; i < nx - 1; i += 2)
  {
    for (int j = 1; j < ny - 1; j += 2)
    {
      tmp = u[i][j];
      u[i][j] = ((u[i - 1][j] + u[i + 1][j]) * dy2 +
                 (u[i][j - 1] + u[i][j + 1]) * dx2) *
                mul_cte;
      err += (u[i][j] - tmp) * (u[i][j] - tmp);
    }
  }
  return sqrt(err);
}
Real LaplaceSolver ::solve(const int n_iter, const Real eps)
{
  Real err = timeStep();
  int count = 1;
  while (err > eps)
  {
    if (n_iter && (count >= n_iter))
    {
      return err;
    }
    err = timeStep();
    ++count;
  }
  return Real(count);
}

int main(int argc, char *argv[])
{
  if (argc != 4)
  {
    std::cout << "Usage: " << argv[0] << " <nx> <n_iter> <eps>" << std::endl;
    return 1;
  }
  int nx, n_iter;
  Real eps;
  Real t_start, t_end;
  Real result;
  nx = atoi(argv[1]);
  n_iter = atoi(argv[2]);
  eps = atof(argv[3]);
  Grid *g = new Grid(nx, nx);
  g->setBCFunc(BC);
  LaplaceSolver s = LaplaceSolver(g);
  t_start = seconds();
  result = s.solve(n_iter, eps);
  t_end = seconds();
  std::cout << result << "," << nx << "," << n_iter << "," << t_end - t_start << "," << 0 << std::endl;
  return 0;
}