// Copyright (c) 2020, RTE (https://www.rte-france.com)
// See AUTHORS.txt
// This Source Code Form is subject to the terms of the Mozilla Public License, version 2.0.
// If a copy of the Mozilla Public License, version 2.0 was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.
// SPDX-License-Identifier: MPL-2.0
// This file is part of LightSim2grid, LightSim2grid implements a c++ backend targeting the Grid2Op platform.

#include "DataGeneric.h"
#include <iostream>
#include <sstream>

const int DataGeneric::_deactivated_bus_id = -1;

// TODO all functions bellow are generic ! Make a base class for that
void DataGeneric::_get_amps(RealVect & a, const RealVect & p, const RealVect & q, const RealVect & v){
    const real_type _1_sqrt_3 = 1.0 / std::sqrt(3.);
    RealVect p2q2 = p.array() * p.array() + q.array() * q.array();
    p2q2 = p2q2.array().cwiseSqrt();

    // modification in case of disconnected powerlines
    // because i don't want to divide by 0. below
    RealVect v_tmp = v;
    for(auto & el: v_tmp){
        if(el == 0.) el = 1.0;
    }
    a = p2q2.array() * _1_sqrt_3 / v_tmp.array();
}
void DataGeneric::_reactivate(int el_id, std::vector<bool> & status, bool & need_reset){
    bool val = status.at(el_id);
    if(!val) need_reset = true;  // I need to recompute the grid, if a status has changed
    status.at(el_id) = true;  //TODO why it's needed to do that again
}
void DataGeneric::_deactivate(int el_id, std::vector<bool> & status, bool & need_reset){
    bool val = status.at(el_id);
    if(val) need_reset = true;  // I need to recompute the grid, if a status has changed
    status.at(el_id) = false;  //TODO why it's needed to do that again
}
void DataGeneric::_change_bus(int el_id, int new_bus_me_id, Eigen::VectorXi & el_bus_ids, bool & need_reset, int nb_bus){
    // bus id here "me_id" and NOT "solver_id"
    // throw error: object id does not exist
    if(el_id >= el_bus_ids.size())
    {
        // TODO DEBUG MODE: only check in debug mode
        std::ostringstream exc_;
        exc_ << "DataGeneric::_change_bus: Cannot change the bus of element with id ";
        exc_ << el_id;
        exc_ << " while the grid counts ";
        exc_ << el_bus_ids.size();
        exc_ << " such elements (id too high)";
        throw std::out_of_range(exc_.str());
    }
    if(el_id < 0)
    {
        // TODO DEBUG MODE: only check in debug mode
        std::ostringstream exc_;
        exc_ << "DataGeneric::_change_bus: Cannot change the bus of element with id ";
        exc_ << el_id;
        exc_ << " (id should be >= 0)";
        throw std::out_of_range(exc_.str());
    }

    // throw error: bus id does not exist
    if(new_bus_me_id >= nb_bus)
    {
        // TODO DEBUG MODE: only check in debug mode
        std::ostringstream exc_;
        exc_ << "DataGeneric::_change_bus: Cannot change an element to bus ";
        exc_ << new_bus_me_id;
        exc_ << " There are only ";
        exc_ << nb_bus;
        exc_ << " distinct buses on this grid.";
        throw std::out_of_range(exc_.str());
    }
    if(new_bus_me_id < 0)
    {
        // TODO DEBUG MODE: only check in debug mode
        std::ostringstream exc_;
        exc_ << "DataGeneric::_change_bus: new bus id should be >=0 and not ";
        exc_ << new_bus_me_id;
        throw std::out_of_range(exc_.str());
    }
    int & bus_me_id = el_bus_ids(el_id);
    if(bus_me_id != new_bus_me_id) need_reset = true;  // in this case i changed the bus, i need to recompute the jacobian and reset the solver
    bus_me_id = new_bus_me_id;
}

int DataGeneric::_get_bus(int el_id, const std::vector<bool> & status_, const Eigen::VectorXi & bus_id_)
{
    int res;
    bool val = status_.at(el_id);  // also check if the el_id is out of bound
    if(!val) res = _deactivated_bus_id;
    else{
        res = bus_id_(el_id);
    }
    return res;
}

void DataGeneric::v_kv_from_vpu(const Eigen::Ref<const RealVect> & Va,
                                const Eigen::Ref<const RealVect> & Vm,
                                const std::vector<bool> & status,
                                int nb_element,
                                const Eigen::VectorXi & bus_me_id,
                                const std::vector<int> & id_grid_to_solver,
                                const RealVect & bus_vn_kv,
                                RealVect & v){
    v = RealVect::Constant(nb_element, -1.0);
    for(int el_id = 0; el_id < nb_element; ++el_id){
        // if the element is disconnected, i leave it like that
        if(!status[el_id]) continue;
        int el_bus_me_id = bus_me_id(el_id);
        int bus_solver_id = id_grid_to_solver[el_bus_me_id];
        if(bus_solver_id == _deactivated_bus_id){
            // TODO DEBUG MODE: only check in debug mode
            std::ostringstream exc_;
            exc_ << "DataGeneric::v_kv_from_vpu: The element of id ";
            exc_ << bus_solver_id;
            exc_ << " is connected to a disconnected bus";
            throw std::runtime_error(exc_.str());
        }
        real_type bus_vn_kv_me = bus_vn_kv(el_bus_me_id);
        v(el_id) = Vm(bus_solver_id) * bus_vn_kv_me;
    }
}


void DataGeneric::v_deg_from_va(const Eigen::Ref<const RealVect> & Va,
                                const Eigen::Ref<const RealVect> & Vm,
                                const std::vector<bool> & status,
                                int nb_element,
                                const Eigen::VectorXi & bus_me_id,
                                const std::vector<int> & id_grid_to_solver,
                                const RealVect & bus_vn_kv,
                                RealVect & theta){
    theta = RealVect::Constant(nb_element, 0.0);
    for(int el_id = 0; el_id < nb_element; ++el_id){
        // if the element is disconnected, i leave it like that
        if(!status[el_id]) continue;
        int el_bus_me_id = bus_me_id(el_id);
        int bus_solver_id = id_grid_to_solver[el_bus_me_id];
        if(bus_solver_id == _deactivated_bus_id){
            // TODO DEBUG MODE: only check in debug mode
            std::ostringstream exc_;
            exc_ << "DataGeneric::v_deg_from_va: The element of id ";
            exc_ << bus_solver_id;
            exc_ << " is connected to a disconnected bus";
            throw std::runtime_error(exc_.str());
        }
        theta(el_id) = Va(bus_solver_id) * 180. / my_pi;
    }
}
