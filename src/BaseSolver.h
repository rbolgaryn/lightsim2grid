// Copyright (c) 2020, RTE (https://www.rte-france.com)
// See AUTHORS.txt
// This Source Code Form is subject to the terms of the Mozilla Public License, version 2.0.
// If a copy of the Mozilla Public License, version 2.0 was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.
// SPDX-License-Identifier: MPL-2.0
// This file is part of LightSim2grid, LightSim2grid implements a c++ backend targeting the Grid2Op platform.

#ifndef BASESOLVER_H
#define BASESOLVER_H

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
#include "BaseConstants.h"

enum class ErrorType {NoError, SingularMatrix, TooManyIterations, InifiniteValue, SolverAnalyze, SolverFactor, SolverReFactor, SolverSolve, NotInitError, LicenseError};
/**
This class represents a solver to compute powerflow.

It can be derived for different usecase, for example for DC powerflow, AC powerflow using Newton Raphson method etc.
**/
class BaseSolver : public BaseConstants
{
    public:
        BaseSolver():BaseConstants(),n_(-1),err_(ErrorType::NotInitError),timer_Fx_(0.),timer_solve_(0.),timer_check_(0.),timer_total_nr_(0.){};

        virtual ~BaseSolver(){}

        Eigen::Ref<const RealVect> get_Va() const{
            return Va_;
        }
        Eigen::Ref<const RealVect> get_Vm() const{
            return Vm_;
        }
        Eigen::Ref<const CplxVect> get_V() const{
            return V_;
        }
        ErrorType get_error() const {
            return err_;
        }
        int get_nb_iter() const {
            return nr_iter_;
        }

        bool converged() const{
            return err_ == ErrorType::NoError;
        }

        std::tuple<double, double, double, double> get_timers() const
        {
            // TODO change the order of the timers here!
            auto res = std::tuple<double, double, double, double>(
              timer_Fx_, timer_solve_, timer_check_, timer_total_nr_);
            return res;
        }

        virtual
        bool compute_pf(const Eigen::SparseMatrix<cplx_type> & Ybus,
                        CplxVect & V,  // store the results of the powerflow and the Vinit !
                        const CplxVect & Sbus,
                        const Eigen::VectorXi & slack_ids,
                        const RealVect & slack_weights,
                        const Eigen::VectorXi & pv,
                        const Eigen::VectorXi & pq,
                        int max_iter,
                        real_type tol
                        ) = 0 ;

        virtual
        void reset();
        
    protected:
        virtual void reset_timer(){
            timer_Fx_ = 0.;
            timer_solve_ = 0.;
            timer_check_ = 0.;
            timer_total_nr_ = 0.;
        }

        bool is_linear_solver_valid(){
            // bool res = true;
            // if((err_ == ErrorType::NotInitError) || (err_ == ErrorType::LicenseError)) res = false;  // cannot use a non intialize solver
            // return res;
            return (err_ != ErrorType::LicenseError);
        }
        RealVect _evaluate_Fx(const Eigen::SparseMatrix<cplx_type> &  Ybus,
                              const CplxVect & V,
                              const CplxVect & Sbus,
                              Eigen::Index slack_id,  // id of the slack bus
                              real_type slack_absorbed,
                              const RealVect & slack_weights,
                              const Eigen::VectorXi & pv,
                              const Eigen::VectorXi & pq);

        RealVect _evaluate_Fx(const Eigen::SparseMatrix<cplx_type> &  Ybus,
                              const CplxVect & V,
                              const CplxVect & Sbus,
                              const Eigen::VectorXi & pv,
                              const Eigen::VectorXi & pq);

        bool _check_for_convergence(const RealVect & F,
                                    real_type tol);

        void one_iter_all_at_once(CplxVect & tmp_Sbus,
                                  const Eigen::SparseMatrix<cplx_type> & Ybus,
                                  const Eigen::VectorXi & pv,
                                  const Eigen::VectorXi & pq
                                  );
        void one_iter(CplxVect & tmp_Sbus,
                      const Eigen::SparseMatrix<cplx_type> & Ybus,
                      const Eigen::VectorXi & pv,
                      const Eigen::VectorXi & pq
                      );

        int extract_slack_bus_id(const Eigen::VectorXi & pv,
                                 const Eigen::VectorXi & pq,
                                 unsigned int nb_bus);

        /**
        When there are multiple slacks, add the other "slack buses" in the PV buses indexes
        (behaves as if only the first element is used for the slack !!!)
        **/
        Eigen::VectorXi retrieve_pv_with_slack(const Eigen::VectorXi & slack_ids, 
                                               const Eigen::VectorXi & pv) const {
            Eigen::VectorXi my_pv = pv;
            if(slack_ids.size() > 1){
                const auto nb_slack_added = slack_ids.size() - 1;
                my_pv = Eigen::VectorXi(pv.size() + nb_slack_added);
                for(auto i = 0; i < nb_slack_added; ++i){
                    my_pv(i) = slack_ids[i+1];
                }
                for(auto i = 0; i < pv.size(); ++i){
                    my_pv(i + nb_slack_added) = pv[i];
                }
            }
            return my_pv;
        }

        /**
        When there are multiple slacks, add the other "slack buses" in the PV buses indexes
        **/
        Eigen::VectorXi add_slack_to_pv(const Eigen::VectorXi & slack_ids, 
                                        const Eigen::VectorXi & pv) const {
            Eigen::VectorXi my_pv = Eigen::VectorXi(slack_ids.size() + pv.size());
            my_pv << slack_ids, pv;
            return my_pv;
        }
        
    protected:
        // solver initialization
        int n_;

        // solution of the problem
        RealVect Vm_;  // voltage magnitude
        RealVect Va_;  // voltage angle
        CplxVect V_;  // complex voltage

        int nr_iter_;  // number of iteration performs by the solver (may vary depending on the solver)
        ErrorType err_; //error message:
        // -1 : the solver has not been initialized (call initialize in this case)
        // 0 everything ok
        // 1: i can't factorize the matrix (klu_factor)
        // 2: i can't refactorize the matrix (klu_refactor)
        // 3: i can't solve the system (klu_solve)
        // 4: end of possible iterations (divergence because nr_iter_ >= max_iter)

        // timers
         double timer_Fx_;
         double timer_solve_;
         double timer_check_;
         double timer_total_nr_;

    private:
        // no copy allowed
        BaseSolver( const BaseSolver & ) ;
        BaseSolver & operator=( const BaseSolver & ) ;

};

#endif // BASESOLVER_H
