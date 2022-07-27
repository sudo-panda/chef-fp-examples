
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

#ifndef DUMP_MATLAB_MATRIX_H
#define DUMP_MATLAB_MATRIX_H
#include <cstdio>
#include "HPC_Sparse_Matrix.hpp"

int dump_matlab_matrix(HPC_Sparse_Matrix &A)
{
   int nrow = A.local_nrow;
  int start_row = 0; // Each processor gets a section of a chimney stack domain

  FILE *handle = 0;
  handle = fopen("mat0.dat", "w");

  for (int i = 0; i < nrow; i++)
  {
    double * cur_vals = A.ptr_to_vals_in_row[i];
    int * cur_inds = A.ptr_to_inds_in_row[i];
    int cur_nnz = A.nnz_in_row[i];
    for (int j = 0; j < cur_nnz; j++)
      fprintf(handle, " %d %d %22.16e\n", start_row + i + 1, cur_inds[j] + 1, cur_vals[j]);
  }

  fclose(handle);
  return (0);
}

#endif