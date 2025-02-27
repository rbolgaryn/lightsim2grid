// Copyright (c) 2020, RTE (https://www.rte-france.com)
// See AUTHORS.txt
// This Source Code Form is subject to the terms of the Mozilla Public License, version 2.0.
// If a copy of the Mozilla Public License, version 2.0 was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.
// SPDX-License-Identifier: MPL-2.0
// This file is part of LightSim2grid, LightSim2grid implements a c++ backend targeting the Grid2Op platform.

#ifndef SPARSELUSOLVER_H
#define SPARSELUSOLVER_H

#include <iostream>
#include <vector>
#include <stdio.h>
#include <cstdint> // for int32
#include <chrono>
#include <cmath>  // for PI

// eigen is necessary to easily pass data from numpy to c++ without any copy.
// and to optimize the matrix operations
#include "Utils.h"
#include "Eigen/Core"
#include "Eigen/Dense"
#include "Eigen/SparseCore"
#include "Eigen/SparseLU"

#include "CustTimer.h"
#include "BaseNRSolver.h"
/**
class to handle the solver using newton-raphson method, using a "SparseLU" algorithm from Eigein
and sparse matrices.

As long as the admittance matrix of the sytem does not change, you can reuse the same solver.
Reusing the same solver is possible, but "reset" method must be called.

Otherwise, unexpected behaviour might follow, including "segfault".

**/
class SparseLULinearSolver
{
    public:
        SparseLULinearSolver():solver_(){}
        
        // public api
        ErrorType initialize(const Eigen::SparseMatrix<real_type> & J);
        ErrorType solve(const Eigen::SparseMatrix<real_type> & J, RealVect & b, bool has_just_been_inialized);
        ErrorType reset(){ return ErrorType::NoError; }

    private:
        // solver initialization
        Eigen::SparseLU<Eigen::SparseMatrix<real_type>, Eigen::COLAMDOrdering<int> >  solver_;

        // no copy allowed
        SparseLULinearSolver( const SparseLULinearSolver & ) =delete ;
        SparseLULinearSolver & operator=( const SparseLULinearSolver & ) =delete ;
};

#endif // SPARSELUSOLVER_H
