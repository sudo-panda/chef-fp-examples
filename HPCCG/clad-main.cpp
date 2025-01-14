
//@HEADER
// ************************************************************************
//
//               HPCCG: Simple Conjugate Gradient Benchmark Code
//                 Copyright (2006) Sandia Corporation
//
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1 of the
// License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
// Questions? Contact Michael A. Heroux (maherou@sandia.gov)
//
// ************************************************************************
//@HEADER

// Changelog
//
// Version 0.3
// - Added timing of setup time for sparse MV
// - Corrected percentages reported for sparse MV with overhead
//
/////////////////////////////////////////////////////////////////////////

// Main routine of a program that reads a sparse matrix, right side
// vector, solution vector and initial guess from a file  in HPC
// format.  This program then calls the HPCCG conjugate gradient
// solver to solve the problem, and then prints results.

// Calling sequence:

// test_HPCCG linear_system_file

// Routines called:

// read_HPC_row - Reads in linear system

// mytimer - Timing routine (compile with -DWALL to get wall clock
//           times

// HPCCG - CG Solver

// compute_residual - Compares HPCCG solution to known solution.

#include <iostream>
using std::cerr;
using std::cout;
using std::endl;
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cassert>
#include <string>
#include <cmath>
#include <iomanip>

#include "clad/Differentiator/Differentiator.h"
#include "../PrintModel/ErrorFunc.h"

#include "generate_matrix.hpp"
#include "read_HPC_row.hpp"
#include "HPC_Sparse_Matrix.hpp"
#include "dump_matlab_matrix.hpp"
#include "ddot.hpp"

#include "HPCCG-clad.hpp"

#undef DEBUG

int main(int argc, char *argv[])
{
  double *x, *b, *xexact;
  double norm, d;
  int ierr = 0;
  int i, j;
  int ione = 1;
  double times[7];
  double t6 = 0.0;
  int nx, ny, nz;

  if (argc != 2 && argc != 4)
  {
    cerr << "Usage:" << endl
         << "Mode 1: " << argv[0] << " nx ny nz" << endl
         << "     where nx, ny and nz are the local sub-block dimensions, or" << endl
         << "Mode 2: " << argv[0] << " HPC_data_file " << endl
         << "     where HPC_data_file is a globally accessible file containing matrix data." << endl;
    exit(1);
  }

  if (argc == 4)
  {
    nx = atoi(argv[1]);
    ny = atoi(argv[2]);
    nz = atoi(argv[3]);
    generate_matrix(nx, ny, nz, clad::A, &x, &b, &xexact);
  }
  else
  {
    clad::read_HPC_row(argv[1], clad::A, &x, &b, &xexact);
  }

  bool dump_matrix = false;
  if (dump_matrix)
    dump_matlab_matrix(clad::A);

  int nrow = clad::A.local_nrow;
  int ncol = clad::A.local_ncol;

  // cout << "Float Result: " << executefunction<float>(nrow, ncol, x, b, xexact) << std::endl;
  // cout << "Double Result: " << executefunction<double>(nrow, ncol, x, b, xexact);

  // executeGradient(nrow, ncol, x, b, xexact);

  double *x_diff = new double[nrow]();
  clad::array_ref<double> d_x(x_diff, nrow);

  double *b_diff = new double[nrow]();
  clad::array_ref<double> d_b(b_diff, nrow);

  double *xexact_diff = new double[nrow]();
  clad::array_ref<double> d_xexact(xexact_diff, nrow);

  double *r = new double[nrow]();
  double *r_diff = new double[nrow]();
  clad::array_ref<double> d_r(r_diff, nrow);

  double *p = new double[ncol]();
  double *p_diff = new double[ncol]();
  clad::array_ref<double> d_p(p_diff, ncol);

  double *Ap = new double[nrow]();
  double *Ap_diff = new double[nrow]();
  clad::array_ref<double> d_Ap(Ap_diff, nrow);

  double _final_error = 0;

  int niters = 0;
  double normr = 0.0;
  int max_iter = 100;
  double tolerance = 0.0; // Set tolerance to zero to make all runs do max_iter iterations

  int d_niters = 0, d_max_iter = 0;
  double d_tolerance = 0, d_normr = 0;

  clad::HPCCG_grad(b, x, max_iter, tolerance, niters, normr, r, p, Ap, xexact, d_b, d_x, &d_max_iter, &d_tolerance, &d_niters, &d_normr, d_r, d_p, d_Ap, d_xexact, _final_error);

  cout << "\nFinal error in HPCCG =" << _final_error << endl;

  clad::printErrorReport();

  delete[] b_diff;
  delete[] x_diff;
  delete[] xexact_diff;
  delete[] p;
  delete[] p_diff;
  delete[] Ap;
  delete[] Ap_diff;
  delete[] r;
  delete[] r_diff;

  delete[] x;
  delete[] xexact;
  delete[] b;

  return 0;
}
