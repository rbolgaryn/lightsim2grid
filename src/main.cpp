// Copyright (c) 2020, RTE (https://www.rte-france.com)
// See AUTHORS.txt
// This Source Code Form is subject to the terms of the Mozilla Public License, version 2.0.
// If a copy of the Mozilla Public License, version 2.0 was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.
// SPDX-License-Identifier: MPL-2.0
// This file is part of LightSim2grid, LightSim2grid implements a c++ backend targeting the Grid2Op platform.

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include "ChooseSolver.h"
#include "DataConverter.h"
#include "GridModel.h"
#include "Computers.h"
#include "SecurityAnalysis.h"

namespace py = pybind11;

PYBIND11_MODULE(lightsim2grid_cpp, m)
{

    // solvers
    py::enum_<SolverType>(m, "SolverType", "This enum controls the solver you want to use.")
        .value("GaussSeidel", SolverType::GaussSeidel, "denotes the :class:`lightsim2grid.solver.GaussSeidelSolver`")
        .value("GaussSeidelSynch", SolverType::GaussSeidelSynch, "denotes the :class:`lightsim2grid.solver.GaussSeidelSynchSolver`")
        .value("SparseLU", SolverType::SparseLU, "denotes the :class:`lightsim2grid.solver.SparseLUSolver`")
        .value("SparseLUSingleSlack", SolverType::SparseLUSingleSlack, "denotes the :class:`lightsim2grid.solver.SparseLUSolverSingleSlack`")
        .value("DC", SolverType::DC, "denotes the :class:`lightsim2grid.solver.DCSolver`")
        .value("KLU", SolverType::KLU, "denotes the :class:`lightsim2grid.solver.KLUSolver`")
        .value("KLUSingleSlack", SolverType::KLUSingleSlack, "denotes the :class:`lightsim2grid.solver.KLUSolverSingleSlack`")
        .value("KLUDC", SolverType::KLUDC, "denotes the :class:`lightsim2grid.solver.KLUDCSolver`")
        .value("NICSLU", SolverType::NICSLU, "denotes the :class:`lightsim2grid.solver.NICSLUSolver`")
        .value("NICSLUSingleSlack", SolverType::NICSLUSingleSlack, "denotes the :class:`lightsim2grid.solver.NICSLUSolverSingleSlack`")
        .value("NICSLUDC", SolverType::NICSLUDC, "denotes the :class:`lightsim2grid.solver.NICSLUDCSolver`")
        .export_values();

    #ifdef KLU_SOLVER_AVAILABLE
    py::class_<KLUSolver>(m, "KLUSolver", R"mydelimiter(
        This classes implements the Newton Raphson algorithm, allowing for distributed slack and using the faster KLU solver available in the SuiteSparse library
        for the linear algebra (can be unavailable if you build lightsim2grid from source). It is usually faster than the :class:`lightsim2grid.solver.SparseLUSolver`.

        See :ref:`available-powerflow-solvers` for more information on how to use it.

        .. note::

            It is refered to by the :attr:`lightsim2grid.solver.SolverType.KLU` enum.
)mydelimiter")
        .def(py::init<>())
        .def("get_J", &KLUSolver::get_J_python)  // (get the jacobian matrix, sparse csc matrix)
        .def("get_Va", &KLUSolver::get_Va)  // get the voltage angle vector (vector of double)
        .def("get_Vm", &KLUSolver::get_Vm)  // get the voltage magnitude vector (vector of double)
        .def("get_error", &KLUSolver::get_error)  // get the error message, see the definition of "err_" for more information
        .def("get_nb_iter", &KLUSolver::get_nb_iter)  // return the number of iteration performed at the last optimization
        .def("reset", &KLUSolver::reset)  // reset the solver to its original state
        .def("converged", &KLUSolver::converged)  // whether the solver has converged
        .def("compute_pf", &KLUSolver::compute_pf, py::call_guard<py::gil_scoped_release>())  // perform the newton raphson optimization
        .def("get_timers", &KLUSolver::get_timers)  // returns the timers corresponding to times the solver spent in different part
        .def("solve", &KLUSolver::compute_pf, py::call_guard<py::gil_scoped_release>() );  // perform the newton raphson optimization
    
    py::class_<KLUSolverSingleSlack>(m, "KLUSolverSingleSlack", R"mydelimiter(
        This classes implements the Newton Raphson algorithm,the faster KLU solver available in the SuiteSparse library
        for the linear algebra. It does not support the distributed slack, but can be slightly faster than the :class:`lightsim2grid.solver.KLUSolver` .
)mydelimiter")
        .def(py::init<>())
        .def("get_J", &KLUSolverSingleSlack::get_J_python)  // (get the jacobian matrix, sparse csc matrix)
        .def("get_Va", &KLUSolverSingleSlack::get_Va)  // get the voltage angle vector (vector of double)
        .def("get_Vm", &KLUSolverSingleSlack::get_Vm)  // get the voltage magnitude vector (vector of double)
        .def("get_error", &KLUSolverSingleSlack::get_error)  // get the error message, see the definition of "err_" for more information
        .def("get_nb_iter", &KLUSolverSingleSlack::get_nb_iter)  // return the number of iteration performed at the last optimization
        .def("reset", &KLUSolverSingleSlack::reset)  // reset the solver to its original state
        .def("converged", &KLUSolverSingleSlack::converged)  // whether the solver has converged
        .def("compute_pf", &KLUSolverSingleSlack::compute_pf, py::call_guard<py::gil_scoped_release>())  // perform the newton raphson optimization
        .def("get_timers", &KLUSolverSingleSlack::get_timers)  // returns the timers corresponding to times the solver spent in different part
        .def("solve", &KLUSolverSingleSlack::compute_pf, py::call_guard<py::gil_scoped_release>() );  // perform the newton raphson optimization
    
    py::class_<KLUDCSolver>(m, "KLUDCSolver", R"mydelimiter(
       Alternative implementation of the DC solver, it uses the faster KLU solver available in the SuiteSparse library to solve for the DC voltage given the DC admitance matrix and
       the power injected at each nodes (can be unavailable if you build lightsim2grid from source).
)mydelimiter")
        .def(py::init<>())
        .def("get_Va", &KLUDCSolver::get_Va)  // get the voltage angle vector (vector of double)
        .def("get_Vm", &KLUDCSolver::get_Vm)  // get the voltage magnitude vector (vector of double)
        .def("get_error", &KLUDCSolver::get_error)  // get the error message, see the definition of "err_" for more information
        .def("get_nb_iter", &KLUDCSolver::get_nb_iter)  // return the number of iteration performed at the last optimization
        .def("reset", &KLUDCSolver::reset)  // reset the solver to its original state
        .def("converged", &KLUDCSolver::converged)  // whether the solver has converged
        .def("compute_pf", &KLUDCSolver::compute_pf, py::call_guard<py::gil_scoped_release>())  // perform the newton raphson optimization
        .def("get_timers", &KLUDCSolver::get_timers)  // returns the timers corresponding to times the solver spent in different part
        .def("solve", &KLUDCSolver::compute_pf, py::call_guard<py::gil_scoped_release>() );  // perform the newton raphson optimization
    #endif  // KLU_SOLVER_AVAILABLE

    #ifdef NICSLU_SOLVER_AVAILABLE
    py::class_<NICSLUSolver>(m, "NICSLUSolver", R"mydelimiter(
        This classes implements the Newton Raphson algorithm, allowing for distributed slack and using the faster NICSLU solver available in the NICSLU library
        for the linear algebra. It is usually faster than the :class:`lightsim2grid.solver.SparseLUSolver`. (requires a build from source)
)mydelimiter")
        .def(py::init<>())
        .def("get_J", &NICSLUSolver::get_J_python)  // (get the jacobian matrix, sparse csc matrix)
        .def("get_Va", &NICSLUSolver::get_Va)  // get the voltage angle vector (vector of double)
        .def("get_Vm", &NICSLUSolver::get_Vm)  // get the voltage magnitude vector (vector of double)
        .def("get_error", &NICSLUSolver::get_error)  // get the error message, see the definition of "err_" for more information
        .def("get_nb_iter", &NICSLUSolver::get_nb_iter)  // return the number of iteration performed at the last optimization
        .def("reset", &NICSLUSolver::reset)  // reset the solver to its original state
        .def("converged", &NICSLUSolver::converged)  // whether the solver has converged
        .def("compute_pf", &NICSLUSolver::compute_pf, py::call_guard<py::gil_scoped_release>())  // perform the newton raphson optimization
        .def("get_timers", &NICSLUSolver::get_timers)  // returns the timers corresponding to times the solver spent in different part
        .def("solve", &NICSLUSolver::compute_pf, py::call_guard<py::gil_scoped_release>() );  // perform the newton raphson optimization
    
    py::class_<NICSLUSolverSingleSlack>(m, "NICSLUSolverSingleSlack", R"mydelimiter(
        This classes implements the Newton Raphson algorithm, the faster NICSLU solver available in the NICSLU library
        for the linear algebra. It does not support the distributed slack, but can be slightly faster than the :class:`lightsim2grid.solver.NICSLUSolver` .
)mydelimiter")
        .def(py::init<>())
        .def("get_J", &NICSLUSolverSingleSlack::get_J_python)  // (get the jacobian matrix, sparse csc matrix)
        .def("get_Va", &NICSLUSolverSingleSlack::get_Va)  // get the voltage angle vector (vector of double)
        .def("get_Vm", &NICSLUSolverSingleSlack::get_Vm)  // get the voltage magnitude vector (vector of double)
        .def("get_error", &NICSLUSolverSingleSlack::get_error)  // get the error message, see the definition of "err_" for more information
        .def("get_nb_iter", &NICSLUSolverSingleSlack::get_nb_iter)  // return the number of iteration performed at the last optimization
        .def("reset", &NICSLUSolverSingleSlack::reset)  // reset the solver to its original state
        .def("converged", &NICSLUSolverSingleSlack::converged)  // whether the solver has converged
        .def("compute_pf", &NICSLUSolverSingleSlack::compute_pf, py::call_guard<py::gil_scoped_release>())  // perform the newton raphson optimization
        .def("get_timers", &NICSLUSolverSingleSlack::get_timers)  // returns the timers corresponding to times the solver spent in different part
        .def("solve", &NICSLUSolverSingleSlack::compute_pf, py::call_guard<py::gil_scoped_release>() );  // perform the newton raphson optimization
    
    py::class_<NICSLUDCSolver>(m, "NICSLUDCSolver", R"mydelimiter(
       Alternative implementation of the DC solver, it uses the faster NICSLU solver available in the NICSLU library to solve for the DC voltage given the DC admitance matrix and
       the power injected at each nodes (requires a build from source).
)mydelimiter")
        .def(py::init<>())
        .def("get_Va", &NICSLUDCSolver::get_Va)  // get the voltage angle vector (vector of double)
        .def("get_Vm", &NICSLUDCSolver::get_Vm)  // get the voltage magnitude vector (vector of double)
        .def("get_error", &NICSLUDCSolver::get_error)  // get the error message, see the definition of "err_" for more information
        .def("get_nb_iter", &NICSLUDCSolver::get_nb_iter)  // return the number of iteration performed at the last optimization
        .def("reset", &NICSLUDCSolver::reset)  // reset the solver to its original state
        .def("converged", &NICSLUDCSolver::converged)  // whether the solver has converged
        .def("compute_pf", &NICSLUDCSolver::compute_pf, py::call_guard<py::gil_scoped_release>())  // perform the newton raphson optimization
        .def("get_timers", &NICSLUDCSolver::get_timers)  // returns the timers corresponding to times the solver spent in different part
        .def("solve", &NICSLUDCSolver::compute_pf, py::call_guard<py::gil_scoped_release>() );  // perform the newton raphson optimization
    #endif  // NICSLU_SOLVER_AVAILABLE

    py::class_<SparseLUSolver>(m, "SparseLUSolver", R"mydelimiter(
        This classes implements the Newton Raphson algorithm, allowing for distributed slack and using the default Eigen sparse solver available in Eigen
        for the linear algebra. 
)mydelimiter")
        .def(py::init<>())
        .def("get_J", &SparseLUSolver::get_J_python)  // (get the jacobian matrix, sparse csc matrix)
        .def("get_Va", &SparseLUSolver::get_Va)  // get the voltage angle vector (vector of double)
        .def("get_Vm", &SparseLUSolver::get_Vm)  // get the voltage magnitude vector (vector of double)
        .def("get_error", &SparseLUSolver::get_error)  // get the error message, see the definition of "err_" for more information
        .def("get_nb_iter", &SparseLUSolver::get_nb_iter)  // return the number of iteration performed at the last optimization
        .def("reset", &SparseLUSolver::reset)  // reset the solver to its original state
        .def("converged", &SparseLUSolver::converged)  // whether the solver has converged
        .def("compute_pf", &SparseLUSolver::compute_pf, py::call_guard<py::gil_scoped_release>())  // perform the newton raphson optimization
        .def("get_timers", &SparseLUSolver::get_timers)  // returns the timers corresponding to times the solver spent in different part
        .def("solve", &SparseLUSolver::compute_pf, py::call_guard<py::gil_scoped_release>() );  // perform the newton raphson optimization
    
    py::class_<SparseLUSolverSingleSlack>(m, "SparseLUSolverSingleSlack", R"mydelimiter(
        This classes implements the Newton Raphson algorithm, using the default Eigen sparse solver available in Eigen
        for the linear algebra. It does not support the distributed slack, but can be slightly faster than the :class:`lightsim2grid.solver.SparseLUSolver` .
)mydelimiter")
        .def(py::init<>())
        .def("get_J", &SparseLUSolver::get_J_python)  // (get the jacobian matrix, sparse csc matrix)
        .def("get_Va", &SparseLUSolver::get_Va)  // get the voltage angle vector (vector of double)
        .def("get_Vm", &SparseLUSolver::get_Vm)  // get the voltage magnitude vector (vector of double)
        .def("get_error", &SparseLUSolver::get_error)  // get the error message, see the definition of "err_" for more information
        .def("get_nb_iter", &SparseLUSolver::get_nb_iter)  // return the number of iteration performed at the last optimization
        .def("reset", &SparseLUSolver::reset)  // reset the solver to its original state
        .def("converged", &SparseLUSolver::converged)  // whether the solver has converged
        .def("compute_pf", &SparseLUSolver::compute_pf, py::call_guard<py::gil_scoped_release>())  // perform the newton raphson optimization
        .def("get_timers", &SparseLUSolver::get_timers)  // returns the timers corresponding to times the solver spent in different part
        .def("solve", &SparseLUSolver::compute_pf, py::call_guard<py::gil_scoped_release>() );  // perform the newton raphson optimization

    py::class_<DCSolver>(m, "DCSolver", R"mydelimiter(
       Default implementation of the DC solver, it uses the default Eigen sparse lu decomposition to solve for the DC voltage given the DC admitance matrix and
       the power injected at each nodes.
)mydelimiter")
        .def(py::init<>())
        .def("get_Va", &DCSolver::get_Va)  // get the voltage angle vector (vector of double)
        .def("get_Vm", &DCSolver::get_Vm)  // get the voltage magnitude vector (vector of double)
        .def("get_error", &DCSolver::get_error)  // get the error message, see the definition of "err_" for more information
        .def("get_nb_iter", &DCSolver::get_nb_iter)  // return the number of iteration performed at the last optimization
        .def("reset", &DCSolver::reset)  // reset the solver to its original state
        .def("converged", &DCSolver::converged)  // whether the solver has converged
        .def("compute_pf", &DCSolver::compute_pf, py::call_guard<py::gil_scoped_release>())  // compute the powerflow
        .def("get_timers", &DCSolver::get_timers)  // returns the timers corresponding to times the solver spent in different part
        .def("solve", &DCSolver::compute_pf, py::call_guard<py::gil_scoped_release>() );  // perform the newton raphson optimization

    py::class_<GaussSeidelSolver>(m, "GaussSeidelSolver", R"mydelimiter(
       Default implementation of the "Gauss Seidel" powerflow solver. We do not recommend to use it as the Newton Raphson based solvers
       are usually much faster.
)mydelimiter")
        .def(py::init<>())
        .def("get_Va", &GaussSeidelSolver::get_Va)  // get the voltage angle vector (vector of double)
        .def("get_Vm", &GaussSeidelSolver::get_Vm)  // get the voltage magnitude vector (vector of double)
        .def("get_error", &GaussSeidelSolver::get_error)  // get the error message, see the definition of "err_" for more information
        .def("get_nb_iter", &GaussSeidelSolver::get_nb_iter)  // return the number of iteration performed at the last optimization
        .def("reset", &GaussSeidelSolver::reset)  // reset the solver to its original state
        .def("converged", &GaussSeidelSolver::converged)  // whether the solver has converged
        .def("compute_pf", &GaussSeidelSolver::compute_pf, py::call_guard<py::gil_scoped_release>())  // compute the powerflow
        .def("get_timers", &GaussSeidelSolver::get_timers)  // returns the timers corresponding to times the solver spent in different part
        .def("solve", &GaussSeidelSolver::compute_pf, py::call_guard<py::gil_scoped_release>() );  // perform the newton raphson optimization

    py::class_<GaussSeidelSynchSolver>(m, "GaussSeidelSynchSolver", R"mydelimiter(
       Variant implementation of the "Gauss Seidel" powerflow solver, where every buses are updated at once (can be significantly faster than the 
       :class:`lightsim2grid.solver.GaussSeidelSolver` for larger grid). We still do not recommend to use it as the Newton Raphson based solvers
       are usually much faster.
)mydelimiter")
        .def(py::init<>())
        .def("get_Va", &GaussSeidelSynchSolver::get_Va)  // get the voltage angle vector (vector of double)
        .def("get_Vm", &GaussSeidelSynchSolver::get_Vm)  // get the voltage magnitude vector (vector of double)
        .def("get_error", &GaussSeidelSynchSolver::get_error)  // get the error message, see the definition of "err_" for more information
        .def("get_nb_iter", &GaussSeidelSynchSolver::get_nb_iter)  // return the number of iteration performed at the last optimization
        .def("reset", &GaussSeidelSynchSolver::reset)  // reset the solver to its original state
        .def("converged", &GaussSeidelSynchSolver::converged)  // whether the solver has converged
        .def("compute_pf", &GaussSeidelSynchSolver::compute_pf, py::call_guard<py::gil_scoped_release>())  // compute the powerflow
        .def("get_timers", &GaussSeidelSynchSolver::get_timers)  // returns the timers corresponding to times the solver spent in different part
        .def("solve", &GaussSeidelSynchSolver::compute_pf, py::call_guard<py::gil_scoped_release>() );  // perform the newton raphson optimization

    // Only "const" method are exported
    // it is so that i cannot modify the internal solver of a gridmodel python side
    py::class_<ChooseSolver>(m, "AnySolver")
        .def(py::init<>())
        .def("get_type", &ChooseSolver::get_type)
        // .def("change_solver", &ChooseSolver::change_solver)
        // .def("reset", &ChooseSolver::reset)
        // .def("compute_pf", &ChooseSolver::compute_pf, py::call_guard<py::gil_scoped_release>())  // compute the powerflow
        // .def("solve", &ChooseSolver::compute_pf, py::call_guard<py::gil_scoped_release>() )
        .def("get_Va", &ChooseSolver::get_Va)  
        .def("get_Vm", &ChooseSolver::get_Vm) 
        .def("get_V", &ChooseSolver::get_V) 
        .def("get_J", &ChooseSolver::get_J_python) 
        .def("get_error", &ChooseSolver::get_error) 
        .def("get_nb_iter", &ChooseSolver::get_nb_iter) 
        .def("converged", &ChooseSolver::converged) 
        .def("get_computation_time", &ChooseSolver::get_computation_time);

    // iterator for generators
    py::class_<DataGen>(m, "DataGen")
        .def("__len__", [](const DataGen & data) { return data.nb(); })
        .def("__getitem__", [](const DataGen & data, int k){return data[k]; } )
        .def("__iter__", [](const DataGen & data) {
       return py::make_iterator(data.begin(), data.end());
    }, py::keep_alive<0, 1>()); /* Keep vector alive while iterator is used */

    py::class_<DataGen::GenInfo>(m, "GenInfo")
        .def_readonly("id", &DataGen::GenInfo::id)
        .def_readonly("connected", &DataGen::GenInfo::connected)
        .def_readonly("bus_id", &DataGen::GenInfo::bus_id)
        .def_readonly("is_slack", &DataGen::GenInfo::is_slack)
        .def_readonly("slack_weight", &DataGen::GenInfo::slack_weight)
        .def_readonly("target_p_mw", &DataGen::GenInfo::target_p_mw)
        .def_readonly("target_vm_pu", &DataGen::GenInfo::target_vm_pu)
        .def_readonly("min_q_mvar", &DataGen::GenInfo::min_q_mvar)
        .def_readonly("max_q_mvar", &DataGen::GenInfo::max_q_mvar)
        .def_readonly("has_res", &DataGen::GenInfo::has_res)
        .def_readonly("res_p_mw", &DataGen::GenInfo::res_p_mw)
        .def_readonly("res_q_mvar", &DataGen::GenInfo::res_q_mvar)
        .def_readonly("res_theta_deg", &DataGen::GenInfo::res_theta_deg)
        .def_readonly("res_v_kv", &DataGen::GenInfo::res_v_kv);

    // iterator for sgens
    py::class_<DataSGen>(m, "DataSGen")
        .def("__len__", [](const DataSGen & data) { return data.nb(); })
        .def("__getitem__", [](const DataSGen & data, int k){return data[k]; } )
        .def("__iter__", [](const DataSGen & data) {
       return py::make_iterator(data.begin(), data.end());
    }, py::keep_alive<0, 1>()); /* Keep vector alive while iterator is used */

    py::class_<DataSGen::SGenInfo>(m, "SGenInfo")
        .def_readonly("id", &DataSGen::SGenInfo::id)
        .def_readonly("connected", &DataSGen::SGenInfo::connected)
        .def_readonly("bus_id", &DataSGen::SGenInfo::bus_id)
        .def_readonly("min_q_mvar", &DataSGen::SGenInfo::min_q_mvar)
        .def_readonly("max_q_mvar", &DataSGen::SGenInfo::max_q_mvar)
        .def_readonly("min_p_mw", &DataSGen::SGenInfo::min_p_mw)
        .def_readonly("max_p_mw", &DataSGen::SGenInfo::max_p_mw)
        .def_readonly("target_p_mw", &DataSGen::SGenInfo::target_p_mw)
        .def_readonly("target_q_mvar", &DataSGen::SGenInfo::target_q_mvar)
        .def_readonly("has_res", &DataSGen::SGenInfo::has_res)
        .def_readonly("res_p_mw", &DataSGen::SGenInfo::res_p_mw)
        .def_readonly("res_q_mvar", &DataSGen::SGenInfo::res_q_mvar)
        .def_readonly("res_theta_deg", &DataSGen::SGenInfo::res_theta_deg)
        .def_readonly("res_v_kv", &DataSGen::SGenInfo::res_v_kv);

    // iterator for loads
    py::class_<DataLoad>(m, "DataLoad")
        .def("__len__", [](const DataLoad & data) { return data.nb(); })
        .def("__getitem__", [](const DataLoad & data, int k){return data[k]; } )
        .def("__iter__", [](const DataLoad & data) {
       return py::make_iterator(data.begin(), data.end());
    }, py::keep_alive<0, 1>()); /* Keep vector alive while iterator is used */

    py::class_<DataLoad::LoadInfo>(m, "LoadInfo")
        .def_readonly("id", &DataLoad::LoadInfo::id)
        .def_readonly("connected", &DataLoad::LoadInfo::connected)
        .def_readonly("bus_id", &DataLoad::LoadInfo::bus_id)
        .def_readonly("target_p_mw", &DataLoad::LoadInfo::target_p_mw)
        .def_readonly("target_q_mvar", &DataLoad::LoadInfo::target_q_mvar)
        .def_readonly("has_res", &DataLoad::LoadInfo::has_res)
        .def_readonly("res_p_mw", &DataLoad::LoadInfo::res_p_mw)
        .def_readonly("res_q_mvar", &DataLoad::LoadInfo::res_q_mvar)
        .def_readonly("res_theta_deg", &DataLoad::LoadInfo::res_theta_deg)
        .def_readonly("res_v_kv", &DataLoad::LoadInfo::res_v_kv);

    // iterator for shunts
    py::class_<DataShunt>(m, "DataShunt")
        .def("__len__", [](const DataShunt & data) { return data.nb(); })
        .def("__getitem__", [](const DataShunt & data, int k){return data[k]; } )
        .def("__iter__", [](const DataShunt & data) {
       return py::make_iterator(data.begin(), data.end());
    }, py::keep_alive<0, 1>()); /* Keep vector alive while iterator is used */

    py::class_<DataShunt::ShuntInfo>(m, "ShuntInfo")
        .def_readonly("id", &DataShunt::ShuntInfo::id)
        .def_readonly("connected", &DataShunt::ShuntInfo::connected)
        .def_readonly("bus_id", &DataShunt::ShuntInfo::bus_id)
        .def_readonly("target_p_mw", &DataShunt::ShuntInfo::target_p_mw)
        .def_readonly("target_q_mvar", &DataShunt::ShuntInfo::target_q_mvar)
        .def_readonly("has_res", &DataShunt::ShuntInfo::has_res)
        .def_readonly("res_p_mw", &DataShunt::ShuntInfo::res_p_mw)
        .def_readonly("res_q_mvar", &DataShunt::ShuntInfo::res_q_mvar)
        .def_readonly("res_theta_deg", &DataShunt::ShuntInfo::res_theta_deg)
        .def_readonly("res_v_kv", &DataShunt::ShuntInfo::res_v_kv);

    // iterator for trafos
    py::class_<DataTrafo>(m, "DataTrafo")
        .def("__len__", [](const DataTrafo & data) { return data.nb(); })
        .def("__getitem__", [](const DataTrafo & data, int k){return data[k]; } )
        .def("__iter__", [](const DataTrafo & data) {
       return py::make_iterator(data.begin(), data.end());
    }, py::keep_alive<0, 1>()); /* Keep vector alive while iterator is used */

    py::class_<DataTrafo::TrafoInfo>(m, "TrafoInfo")
        .def_readonly("id", &DataTrafo::TrafoInfo::id)
        .def_readonly("connected", &DataTrafo::TrafoInfo::connected)
        .def_readonly("bus_hv_id", &DataTrafo::TrafoInfo::bus_hv_id)
        .def_readonly("bus_lv_id", &DataTrafo::TrafoInfo::bus_lv_id)
        .def_readonly("r_pu", &DataTrafo::TrafoInfo::r_pu)
        .def_readonly("x_pu", &DataTrafo::TrafoInfo::x_pu)
        .def_readonly("h_pu", &DataTrafo::TrafoInfo::h_pu)
        .def_readonly("is_tap_hv_side", &DataTrafo::TrafoInfo::is_tap_hv_side)
        .def_readonly("ratio", &DataTrafo::TrafoInfo::ratio)
        .def_readonly("shift_rad", &DataTrafo::TrafoInfo::shift_rad)
        .def_readonly("has_res", &DataTrafo::TrafoInfo::has_res)
        .def_readonly("res_p_hv_mw", &DataTrafo::TrafoInfo::res_p_hv_mw)
        .def_readonly("res_q_hv_mvar", &DataTrafo::TrafoInfo::res_q_hv_mvar)
        .def_readonly("res_v_hv_kv", &DataTrafo::TrafoInfo::res_v_hv_kv)
        .def_readonly("res_a_hv_ka", &DataTrafo::TrafoInfo::res_a_hv_ka)
        .def_readonly("res_p_lv_mw", &DataTrafo::TrafoInfo::res_p_lv_mw)
        .def_readonly("res_q_lv_mvar", &DataTrafo::TrafoInfo::res_q_lv_mvar)
        .def_readonly("res_v_lv_kv", &DataTrafo::TrafoInfo::res_v_lv_kv)
        .def_readonly("res_a_lv_ka", &DataTrafo::TrafoInfo::res_a_lv_ka)
        .def_readonly("res_theta_hv_deg", &DataTrafo::TrafoInfo::res_theta_hv_deg)
        .def_readonly("res_theta_lv_deg", &DataTrafo::TrafoInfo::res_theta_lv_deg);

    // iterator for trafos
    py::class_<DataLine>(m, "DataLine")
        .def("__len__", [](const DataLine & data) { return data.nb(); })
        .def("__getitem__", [](const DataLine & data, int k){return data[k]; } )
        .def("__iter__", [](const DataLine & data) {
       return py::make_iterator(data.begin(), data.end());
    }, py::keep_alive<0, 1>()); /* Keep vector alive while iterator is used */

    py::class_<DataLine::LineInfo>(m, "LineInfo")
        .def_readonly("id", &DataLine::LineInfo::id)
        .def_readonly("connected", &DataLine::LineInfo::connected)
        .def_readonly("bus_or_id", &DataLine::LineInfo::bus_or_id)
        .def_readonly("bus_ex_id", &DataLine::LineInfo::bus_ex_id)
        .def_readonly("r_pu", &DataLine::LineInfo::r_pu)
        .def_readonly("x_pu", &DataLine::LineInfo::x_pu)
        .def_readonly("h_pu", &DataLine::LineInfo::h_pu)
        .def_readonly("has_res", &DataLine::LineInfo::has_res)
        .def_readonly("res_p_or_mw", &DataLine::LineInfo::res_p_or_mw)
        .def_readonly("res_q_or_mvar", &DataLine::LineInfo::res_q_or_mvar)
        .def_readonly("res_v_or_kv", &DataLine::LineInfo::res_v_or_kv)
        .def_readonly("res_a_or_ka", &DataLine::LineInfo::res_a_or_ka)
        .def_readonly("res_p_ex_mw", &DataLine::LineInfo::res_p_ex_mw)
        .def_readonly("res_q_ex_mvar", &DataLine::LineInfo::res_q_ex_mvar)
        .def_readonly("res_v_ex_kv", &DataLine::LineInfo::res_v_ex_kv)
        .def_readonly("res_a_ex_ka", &DataLine::LineInfo::res_a_ex_ka)
        .def_readonly("res_theta_or_deg", &DataLine::LineInfo::res_theta_or_deg)
        .def_readonly("res_theta_ex_deg", &DataLine::LineInfo::res_theta_ex_deg);

    // converters
    py::class_<PandaPowerConverter>(m, "PandaPowerConverter")
        .def(py::init<>())
        .def("set_f_hz", &PandaPowerConverter::set_f_hz)
        .def("set_sn_mva", &PandaPowerConverter::set_sn_mva)
        .def("get_line_param", &PandaPowerConverter::get_line_param)
        .def("get_trafo_param", &PandaPowerConverter::get_trafo_param);

    py::class_<GridModel>(m, "GridModel")
        .def(py::init<>())
        .def("copy", &GridModel::copy)

        // pickle
        .def(py::pickle(
                        [](const GridModel &gm) { // __getstate__
                            // Return a tuple that fully encodes the state of the object
                            return py::make_tuple(gm.get_state());
                        },
                        [](py::tuple py_state) { // __setstate__
                            if (py_state.size() != 1){
                                std::cout << "GridModel.__setstate__ : state size " << py_state.size() << std::endl;
                                throw std::runtime_error("Invalid state size when loading GridModel.__setstate__");
                                }
                            // Create a new C++ instance
                            GridModel gm = GridModel();
                            // TODO check the size of the input tuple!

                            // now set the status
                            GridModel::StateRes state = py_state[0].cast<GridModel::StateRes>();
                            gm.set_state(state);
                            return gm;
        }))

        // general parameters
        // solver control
        .def("change_solver", &GridModel::change_solver, R"mydelimiter(
    This function allows to control which solver is used during the powerflow. See the section :ref:`available-powerflow-solvers` for 
    more information about them.

    Examples
    ---------

    .. code-block:: python
        
        from lightsim2grid.solver import SolverType
        # init the grid model
        from lightsim2grid.initGridModel import init
        pp_net = ...  # any pandapower grid
        lightsim_grid_model = init(pp_net)  # some warnings might be issued as well as some warnings

        # change the solver used for the powerflow
        lightsim_grid_model.change_solver(SolverType.SparseLUSolver)  # change the NR solver that uses Eigen sparse LU

)mydelimiter")
        .def("available_solvers", &GridModel::available_solvers)  // retrieve the solver available for your installation
        .def("get_computation_time", &GridModel::get_computation_time)  // get the computation time spent in the solver
        .def("get_solver_type", &GridModel::get_solver_type)  // get the type of solver used
        .def("get_solver", &GridModel::get_solver, py::return_value_policy::reference)  // get the solver (AnySolver type python side) used

        // init the grid
        .def("init_bus", &GridModel::init_bus)
        .def("set_init_vm_pu", &GridModel::set_init_vm_pu)  // TODO use python "property" for that
        .def("get_init_vm_pu", &GridModel::get_init_vm_pu)
        .def("set_sn_mva", &GridModel::set_sn_mva)   // TODO use python "property" for that
        .def("get_sn_mva", &GridModel::get_sn_mva)

        // init its elements
        .def("init_powerlines", &GridModel::init_powerlines)  // TODO code the possibility to add / remove a powerline after creation
        .def("init_shunt", &GridModel::init_shunt)  // same
        .def("init_trafo", &GridModel::init_trafo)  // same 
        .def("init_generators", &GridModel::init_generators)  // same
        .def("init_loads", &GridModel::init_loads)  // same
        .def("init_storages", &GridModel::init_storages)  // same
        .def("init_sgens", &GridModel::init_sgens)  // same
        .def("add_gen_slackbus", &GridModel::add_gen_slackbus) // same
        .def("remove_gen_slackbus", &GridModel::remove_gen_slackbus)  // same

        // inspect the grid
        .def("get_lines", &GridModel::get_lines, R"mydelimiter(
    This function allows to retrieve the powerlines (as a DataLine object, see :ref:`elements-modeled` for more information)

    Examples
    ---------

    .. code-block:: python
        
        from lightsim2grid.solver import SolverType
        # init the grid model
        from lightsim2grid.initGridModel import init
        pp_net = ...  # any pandapower grid
        lightsim_grid_model = init(pp_net)  # some warnings might be issued as well as some warnings

        # change the solver used for the powerflow
        print([el.x_pu for el in lightsim_grid_model.get_lines()]) # to retrieve the "x" for each

)mydelimiter")
        .def("get_trafos", &GridModel::get_trafos, R"mydelimiter(
    This function allows to retrieve the transformers (as a DataTrafo object, see :ref:`elements-modeled` for more information)
    
    Examples
    ---------

    .. code-block:: python
        
        from lightsim2grid.solver import SolverType
        # init the grid model
        from lightsim2grid.initGridModel import init
        pp_net = ...  # any pandapower grid
        lightsim_grid_model = init(pp_net)  # some warnings might be issued as well as some warnings

        # change the solver used for the powerflow
        print([el.x_pu for el in lightsim_grid_model.get_trafos()]) # to retrieve the "x" for each trafo
)mydelimiter")
        .def("get_generators", &GridModel::get_generators, R"mydelimiter(
    This function allows to retrieve the generators (as a DataGenerators object, see :ref:`elements-modeled` for more information)

    Examples
    ---------

    .. code-block:: python
        
        from lightsim2grid.solver import SolverType
        # init the grid model
        from lightsim2grid.initGridModel import init
        pp_net = ...  # any pandapower grid
        lightsim_grid_model = init(pp_net)  # some warnings might be issued as well as some warnings

        # change the solver used for the powerflow
        print([el.target_p_mw for el in lightsim_grid_model.get_generators()]) # to retrieve the active production setpoint for each generators
)mydelimiter")
        .def("get_static_generators", &GridModel::get_static_generators, R"mydelimiter(
    This function allows to retrieve the static generators (as a DataStaticGenerator object, see :ref:`elements-modeled` for more information)

    Examples
    ---------

    .. code-block:: python
        
        from lightsim2grid.solver import SolverType
        # init the grid model
        from lightsim2grid.initGridModel import init
        pp_net = ...  # any pandapower grid
        lightsim_grid_model = init(pp_net)  # some warnings might be issued as well as some warnings

        # change the solver used for the powerflow
        print([el.target_p_mw for el in lightsim_grid_model.get_static_generators()]) # to retrieve the active production setpoint for each static gen
)mydelimiter")
        .def("get_shunts", &GridModel::get_shunts, R"mydelimiter(
    This function allows to retrieve the shunts (as a DatShunt object, see :ref:`elements-modeled` for more information)
    
    Examples
    ---------

    .. code-block:: python
        
        from lightsim2grid.solver import SolverType
        # init the grid model
        from lightsim2grid.initGridModel import init
        pp_net = ...  # any pandapower grid
        lightsim_grid_model = init(pp_net)  # some warnings might be issued as well as some warnings

        # change the solver used for the powerflow
        print([el.target_q_mvar for el in lightsim_grid_model.get_shunts()]) # to retrieve the reactive consumption for each shunts
)mydelimiter")
        .def("get_storages", &GridModel::get_storages, R"mydelimiter(
    This function allows to retrieve the storage units (as a DataLoad object, see :ref:`elements-modeled` for more information)

    Examples
    --------

    .. code-block:: python
        
        from lightsim2grid.solver import SolverType
        # init the grid model
        from lightsim2grid.initGridModel import init
        pp_net = ...  # any pandapower grid
        lightsim_grid_model = init(pp_net)  # some warnings might be issued as well as some warnings

        # change the solver used for the powerflow
        print([el.target_p_mw for el in lightsim_grid_model.get_storages()]) # to retrieve the active consumption for each storage unit
)mydelimiter")
        .def("get_loads", &GridModel::get_loads, R"mydelimiter(
    This function allows to retrieve the loads (as a DataLoad object, see :ref:`elements-modeled` for more information)

    Examples
    --------

    .. code-block:: python
        
        from lightsim2grid.solver import SolverType
        # init the grid model
        from lightsim2grid.initGridModel import init
        pp_net = ...  # any pandapower grid
        lightsim_grid_model = init(pp_net)  # some warnings might be issued as well as some warnings

        # change the solver used for the powerflow
        print([el.target_p_mw for el in lightsim_grid_model.get_loads()]) # to retrieve the active consumption setpoint for each loads
)mydelimiter")

        // modify the grid
        .def("deactivate_bus", &GridModel::deactivate_bus, R"mydelimiter(
        INTERNAL

        .. warning:: /!\\\\ Internal, do not use unless you know what you are doing /!\\\\

)mydelimiter")
        .def("reactivate_bus", &GridModel::reactivate_bus, R"mydelimiter(
        INTERNAL

        .. warning:: /!\\\\ Internal, do not use unless you know what you are doing /!\\\\
        
)mydelimiter")
        .def("nb_bus", &GridModel::nb_bus, R"mydelimiter(
        Returns the total number of buses on the grid, some of which might be disconnected, some others connected
        
)mydelimiter")

        .def("deactivate_powerline", &GridModel::deactivate_powerline)
        .def("reactivate_powerline", &GridModel::reactivate_powerline)
        .def("change_bus_powerline_or", &GridModel::change_bus_powerline_or)
        .def("change_bus_powerline_ex", &GridModel::change_bus_powerline_ex)
        .def("get_bus_powerline_or", &GridModel::get_bus_powerline_or)
        .def("get_bus_powerline_ex", &GridModel::get_bus_powerline_ex)

        .def("deactivate_trafo", &GridModel::deactivate_trafo)
        .def("reactivate_trafo", &GridModel::reactivate_trafo)
        .def("change_bus_trafo_hv", &GridModel::change_bus_trafo_hv)
        .def("change_bus_trafo_lv", &GridModel::change_bus_trafo_lv)
        .def("get_bus_trafo_hv", &GridModel::get_bus_trafo_hv)
        .def("get_bus_trafo_lv", &GridModel::get_bus_trafo_lv)

        .def("deactivate_load", &GridModel::deactivate_load)
        .def("reactivate_load", &GridModel::reactivate_load)
        .def("change_bus_load", &GridModel::change_bus_load)
        .def("get_bus_load", &GridModel::get_bus_load)
        .def("change_p_load", &GridModel::change_p_load)
        .def("change_q_load", &GridModel::change_q_load)

        .def("deactivate_gen", &GridModel::deactivate_gen)
        .def("reactivate_gen", &GridModel::reactivate_gen)
        .def("change_bus_gen", &GridModel::change_bus_gen)
        .def("get_bus_gen", &GridModel::get_bus_gen)
        .def("change_p_gen", &GridModel::change_p_gen)
        .def("change_v_gen", &GridModel::change_v_gen)

        .def("deactivate_shunt", &GridModel::deactivate_shunt)
        .def("reactivate_shunt", &GridModel::reactivate_shunt)
        .def("change_bus_shunt", &GridModel::change_bus_shunt)
        .def("get_bus_shunt", &GridModel::get_bus_shunt)
        .def("change_p_shunt", &GridModel::change_p_shunt)
        .def("change_q_shunt", &GridModel::change_q_shunt)

        .def("deactivate_sgen", &GridModel::deactivate_sgen)
        .def("reactivate_sgen", &GridModel::reactivate_sgen)
        .def("change_bus_sgen", &GridModel::change_bus_sgen)
        .def("get_bus_sgen", &GridModel::get_bus_sgen)
        .def("change_p_sgen", &GridModel::change_p_sgen)
        .def("change_q_sgen", &GridModel::change_q_sgen)

        .def("deactivate_storage", &GridModel::deactivate_storage)
        .def("reactivate_storage", &GridModel::reactivate_storage)
        .def("change_bus_storage", &GridModel::change_bus_storage)
        .def("get_bus_storage", &GridModel::get_bus_storage)
        .def("change_p_storage", &GridModel::change_p_storage)
        .def("change_q_storage", &GridModel::change_q_storage)

        // get back the results
        .def("get_V", &GridModel::get_V)
        .def("get_Va", &GridModel::get_Va)
        .def("get_Vm", &GridModel::get_Vm)
        .def("get_J", &GridModel::get_J_python)
        .def("check_solution", &GridModel::check_solution)

        // TODO optimize that for speed, results are copied apparently
        .def("get_loads_res", &GridModel::get_loads_res)
        .def("get_loads_status", &GridModel::get_loads_status)
        .def("get_shunts_res", &GridModel::get_shunts_res)
        .def("get_shunts_status", &GridModel::get_shunts_status)
        .def("get_gen_res", &GridModel::get_gen_res)
        .def("get_gen_status", &GridModel::get_gen_status)
        .def("get_lineor_res", &GridModel::get_lineor_res)
        .def("get_lineex_res", &GridModel::get_lineex_res)
        .def("get_lines_status", &GridModel::get_lines_status)
        .def("get_trafohv_res", &GridModel::get_trafohv_res)
        .def("get_trafolv_res", &GridModel::get_trafolv_res)
        .def("get_trafo_status", &GridModel::get_trafo_status)
        .def("get_storages_res", &GridModel::get_storages_res)
        .def("get_storages_status", &GridModel::get_storages_status)
        .def("get_sgens_res", &GridModel::get_sgens_res)
        .def("get_sgens_status", &GridModel::get_sgens_status)

        .def("get_gen_theta", &GridModel::get_gen_theta)
        .def("get_load_theta", &GridModel::get_load_theta)
        .def("get_shunt_theta", &GridModel::get_shunt_theta)
        .def("get_storage_theta", &GridModel::get_storage_theta)
        .def("get_lineor_theta", &GridModel::get_lineor_theta)
        .def("get_lineex_theta", &GridModel::get_lineex_theta)
        .def("get_trafohv_theta", &GridModel::get_trafohv_theta)
        .def("get_trafolv_theta", &GridModel::get_trafolv_theta)

        // do something with the grid
        // .def("init_Ybus", &DataModel::init_Ybus) // temporary
        .def("get_Ybus", &GridModel::get_Ybus)
        .def("get_dcYbus", &GridModel::get_dcYbus)
        .def("get_Sbus", &GridModel::get_Sbus)
        .def("get_pv", &GridModel::get_pv)
        .def("get_pq", &GridModel::get_pq)

        .def("deactivate_result_computation", &GridModel::deactivate_result_computation)
        .def("reactivate_result_computation", &GridModel::reactivate_result_computation)
        .def("dc_pf", &GridModel::dc_pf)
        .def("ac_pf", &GridModel::ac_pf)
        .def("unset_topo_changed", &GridModel::unset_topo_changed)
        .def("tell_topo_changed", &GridModel::tell_topo_changed)
        .def("compute_newton", &GridModel::ac_pf)

         // apply action faster (optimized for grid2op representation)
         // it is not recommended to use it outside of grid2Op.
        .def("update_bus_status", &GridModel::update_bus_status)
        .def("update_gens_p", &GridModel::update_gens_p)
        .def("update_gens_v", &GridModel::update_gens_v)
        .def("update_loads_p", &GridModel::update_loads_p)
        .def("update_loads_q", &GridModel::update_loads_q)
        .def("update_topo", &GridModel::update_topo)
        .def("update_storages_p", &GridModel::update_storages_p)

        // auxiliary functions
        .def("set_n_sub", &GridModel::set_n_sub)
        .def("set_load_pos_topo_vect", &GridModel::set_load_pos_topo_vect)
        .def("set_gen_pos_topo_vect", &GridModel::set_gen_pos_topo_vect)
        .def("set_line_or_pos_topo_vect", &GridModel::set_line_or_pos_topo_vect)
        .def("set_line_ex_pos_topo_vect", &GridModel::set_line_ex_pos_topo_vect)
        .def("set_trafo_hv_pos_topo_vect", &GridModel::set_trafo_hv_pos_topo_vect)
        .def("set_trafo_lv_pos_topo_vect", &GridModel::set_trafo_lv_pos_topo_vect)
        .def("set_storage_pos_topo_vect", &GridModel::set_storage_pos_topo_vect)
        .def("set_load_to_subid", &GridModel::set_load_to_subid)
        .def("set_gen_to_subid", &GridModel::set_gen_to_subid)
        .def("set_line_or_to_subid", &GridModel::set_line_or_to_subid)
        .def("set_line_ex_to_subid", &GridModel::set_line_ex_to_subid)
        .def("set_trafo_hv_to_subid", &GridModel::set_trafo_hv_to_subid)
        .def("set_trafo_lv_to_subid", &GridModel::set_trafo_lv_to_subid)
        .def("set_storage_to_subid", &GridModel::set_storage_to_subid)
        ;

    py::class_<Computers>(m, "Computers")
        .def(py::init<const GridModel &>())

        // solver control
        .def("change_solver", &Computers::change_solver)
        .def("available_solvers", &Computers::available_solvers)
        .def("get_solver_type", &Computers::get_solver_type)

        // timers
        .def("total_time", &Computers::total_time)
        .def("solver_time", &Computers::solver_time)
        .def("preprocessing_time", &Computers::preprocessing_time)
        .def("amps_computation_time", &Computers::amps_computation_time)
        .def("nb_solved", &Computers::nb_solved)

        // status
        .def("get_status", &Computers::get_status)

        // computation control
        .def("deactivate_flow_computations", &Computers::deactivate_flow_computations)
        .def("activate_flow_computations", &Computers::activate_flow_computations)

        // perform the computations
        .def("compute_Vs", &Computers::compute_Vs, py::call_guard<py::gil_scoped_release>())
        .def("compute_flows", &Computers::compute_flows, py::call_guard<py::gil_scoped_release>()) // need to be done after compute_Vs

        // results (for now only flow (at each -line origin- or voltages -at each buses)
        .def("get_flows", &Computers::get_flows)  // need to be done after "compute_Vs"  and "compute_flows"
        .def("get_voltages", &Computers::get_voltages)  // need to be done after "compute_Vs" 
        .def("get_sbuses", &Computers::get_sbuses)  // need to be done after "compute_Vs" 
        ;

    py::class_<SecurityAnalysis>(m, "SecurityAnalysis")
        .def(py::init<const GridModel &>())
        // solver control
        .def("change_solver", &Computers::change_solver)
        .def("available_solvers", &Computers::available_solvers)
        .def("get_solver_type", &Computers::get_solver_type)

        // add some defaults
        .def("add_all_n1", &SecurityAnalysis::add_all_n1)
        .def("add_n1", &SecurityAnalysis::add_n1)
        .def("add_nk", &SecurityAnalysis::add_nk)
        .def("add_multiple_n1", &SecurityAnalysis::add_multiple_n1)

        // remove some defaults (TODO)
        .def("reset", &SecurityAnalysis::clear)
        .def("clear", &SecurityAnalysis::clear)
        .def("remove_n1", &SecurityAnalysis::remove_n1)
        .def("remove_nk", &SecurityAnalysis::remove_nk)
        .def("remove_multiple_n1", &SecurityAnalysis::remove_multiple_n1)
        
        // inspect the class
        .def("my_defaults", &SecurityAnalysis::my_defaults_vect)

        // perform the computation
        .def("compute", &SecurityAnalysis::compute, py::call_guard<py::gil_scoped_release>())

        // results (for now only flow (at each -line origin- or voltages -at each buses)
        .def("get_flows", &SecurityAnalysis::get_flows)  // need to be done after "compute" and "compute_flows"
        .def("get_voltages", &SecurityAnalysis::get_voltages) // need to be done after "compute"
        .def("compute_flows", &SecurityAnalysis::compute_flows, py::call_guard<py::gil_scoped_release>())  // need to be done after "compute"

        // timers
        .def("total_time", &SecurityAnalysis::total_time)
        .def("solver_time", &SecurityAnalysis::solver_time)
        .def("preprocessing_time", &SecurityAnalysis::preprocessing_time)
        .def("amps_computation_time", &SecurityAnalysis::amps_computation_time)
        .def("modif_Ybus_time", &SecurityAnalysis::modif_Ybus_time)
        .def("nb_solved", &SecurityAnalysis::nb_solved)
        ;
}
