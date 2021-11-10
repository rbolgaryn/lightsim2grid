// Copyright (c) 2020, RTE (https://www.rte-france.com)
// See AUTHORS.txt
// This Source Code Form is subject to the terms of the Mozilla Public License, version 2.0.
// If a copy of the Mozilla Public License, version 2.0 was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.
// SPDX-License-Identifier: MPL-2.0
// This file is part of LightSim2grid, LightSim2grid implements a c++ backend targeting the Grid2Op platform.

#ifdef NICSLU_SOLVER_AVAILABLE
#ifndef NICSLUSOLVER_H
#define NICSLUSOLVER_H

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

// import klu package
#include "nicslu_cpp.inl"

#include "CustTimer.h"
#include "BaseNRSolver.h"
/**
class to handle the solver using newton-raphson method, using NICSLU algorithm and sparse matrices.

As long as the admittance matrix of the system does not change, you can reuse the same solver.
Reusing the same solver is possible, but "reset" method must be called.

Otherwise, unexpected behaviour might follow, including "segfault".

NB: the code of NICSLU is not included in this repository. This class is only compiled if the "setup.py"
can find a version of `https://github.com/chenxm1986/nicslu`. Be careful though, this code is under some
specific license.

**/

// TODO use the cpp API instead !
class NICSLUSolver: public BaseNRSolver
{
    public:
        NICSLUSolver():
            BaseNRSolver(),
            solver_(),
            nb_thread_(1),
            ai_(nullptr), 
            ap_(nullptr){}

        ~NICSLUSolver()
         {
            solver_.Free();
            if(ai_!= nullptr) delete [] ai_;
            if(ap_!= nullptr) delete [] ap_;
         }

        // prevent copy and assignment
        NICSLUSolver(const NICSLUSolver & other) = delete;
        NICSLUSolver & operator=( const NICSLUSolver & ) = delete;

        virtual void reset();

    protected:
        virtual
        void initialize();

        virtual
        void solve(RealVect & b, bool has_just_been_inialized);

    private:
        // solver initialization
        CNicsLU solver_;
        const unsigned int nb_thread_;
        unsigned int * ai_;
        unsigned int * ap_;

};

#endif // NICSLUSOLVER_H
#endif  // NICSLU_SOLVER_AVAILABLE
