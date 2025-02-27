// Copyright (c) 2020, RTE (https://www.rte-france.com)
// See AUTHORS.txt
// This Source Code Form is subject to the terms of the Mozilla Public License, version 2.0.
// If a copy of the Mozilla Public License, version 2.0 was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.
// SPDX-License-Identifier: MPL-2.0
// This file is part of LightSim2grid, LightSim2grid implements a c++ backend targeting the Grid2Op platform.

#ifndef DATAGENERIC_H
#define DATAGENERIC_H

#include <algorithm>  // for std::find

#include "Utils.h"

#include "Eigen/Core"
#include "Eigen/Dense"
#include "Eigen/SparseCore"
#include "Eigen/SparseLU"

#include "BaseConstants.h"

// iterator type
template<class DataType>
class DataConstIterator
{
    protected:
        typedef typename DataType::DataInfo DataInfo;

        const DataType * const _p_data_;
        int my_id;

    public:
        DataInfo my_info;

        // functions
        DataConstIterator(const DataType * const data_, int id):
            _p_data_(data_),
            my_id(id),
            my_info(*data_, id)
            {};

        const DataInfo& operator*() const { return my_info; }
        bool operator==(const DataConstIterator<DataType> & other) const { return (my_id == other.my_id) && (_p_data_ == other._p_data_); }
        bool operator!=(const DataConstIterator<DataType> & other) const { return !(*this == other); }
        DataConstIterator<DataType> & operator++()
        {
            ++my_id;
            my_info = DataInfo(*_p_data_, my_id);
            return *this;
        }
        DataConstIterator<DataType> & operator--()
        {
            --my_id;
            my_info = DataInfo(*_p_data_, my_id);
            return *this;
        }
        int size() const { return _p_data_->nb(); }
};
// end iterator type

// TODO make this class iterable ! with operator begin, end and an iterator
/**
Base class for every object that can be manipulated
**/
class DataGeneric : public BaseConstants
{
    public:

        virtual void fillYbus(std::vector<Eigen::Triplet<cplx_type> > & res,
                              bool ac,
                              const std::vector<int> & id_grid_to_solver,
                              real_type sn_mva) {};
        virtual void fillYbus(Eigen::SparseMatrix<cplx_type> & res, bool ac, const std::vector<int> & id_grid_to_solver) {};
        virtual void fillSbus(CplxVect & Sbus, bool ac, const std::vector<int> & id_grid_to_solver){};
        virtual void fillpv(std::vector<int>& bus_pv,
                            std::vector<bool> & has_bus_been_added,
                            Eigen::VectorXi & slack_bus_id_solver,
                            const std::vector<int> & id_grid_to_solver) const {};
        
        virtual void get_q(std::vector<real_type>& q_by_bus) {};

        void set_p_slack(const RealVect& node_mismatch, const std::vector<int> & id_grid_to_solver) {};
    
        static const int _deactivated_bus_id;

        /**"define" the destructor for compliance with clang (otherwise lots of warnings)**/
        virtual ~DataGeneric() {};
        
    protected:
        /**
        activation / deactivation of elements
        **/
        void _reactivate(int el_id, std::vector<bool> & status, bool & need_reset);
        void _deactivate(int el_id, std::vector<bool> & status, bool & need_reset);
        void _change_bus(int el_id, int new_bus_me_id, Eigen::VectorXi & el_bus_ids, bool & need_reset, int nb_bus);
        int _get_bus(int el_id, const std::vector<bool> & status_, const Eigen::VectorXi & bus_id_);

        /**
        compute the amps from the p, the q and the v (v should NOT be pair unit)
        **/
        void _get_amps(RealVect & a, const RealVect & p, const RealVect & q, const RealVect & v);

        /**
        convert v from pu to v in kv (and assign it to the right element...)
        **/
        void v_kv_from_vpu(const Eigen::Ref<const RealVect> & Va,
                           const Eigen::Ref<const RealVect> & Vm,
                           const std::vector<bool> & status,
                           int nb_element,
                           const Eigen::VectorXi & bus_me_id,
                           const std::vector<int> & id_grid_to_solver,
                           const RealVect & bus_vn_kv,
                           RealVect & v);


        /**
        compute va in degree from va in rad.
        **/
        void v_deg_from_va(const Eigen::Ref<const RealVect> & Va,
                           const Eigen::Ref<const RealVect> & Vm,
                           const std::vector<bool> & status,
                           int nb_element,
                           const Eigen::VectorXi & bus_me_id,
                           const std::vector<int> & id_grid_to_solver,
                           const RealVect & bus_vn_kv,
                           RealVect & v);

        /**
        check the size of the elements
        **/
        template<class T, class intType>
        void check_size(const T & container, intType size, const std::string & container_name)
        {
            if(container.size() != size) throw std::runtime_error(container_name + " do not have the proper size");
        }

        /**
        check if an element is in a vector or an Eigen Vector, do not use for other types of containers (might not be efficient at all)
        **/
        template<class T>  // a std::vector, or an Eigen::Vector                                                 
        bool is_in_vect(int val, const T & cont) const {return std::find(cont.begin(), cont.end(), val) != cont.end();}
};

#endif // DATAGENERIC_H

