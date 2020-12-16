/*
Copyright (C) 2020 Red Hat, Inc.

This file is part of libdnf: https://github.com/rpm-software-management/libdnf/

Libdnf is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Libdnf is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with libdnf.  If not, see <https://www.gnu.org/licenses/>.
*/


#ifndef LIBDNF_COMPS_COMPS_HPP
#define LIBDNF_COMPS_COMPS_HPP


#include <set>
#include <string>

//#include "group/group.hpp"
#include "group/sack.hpp"

extern "C" {
#include <solv/pool.h>
#include <solv/repo.h>
}


namespace libdnf {
class Base;
}


namespace libdnf::comps {


//class GroupSack;


class Comps {
public:
    explicit Comps(libdnf::Base & base);
    ~Comps();

    // TODO(dmach): load to a new Comps object and merge when it's fully loaded for better transactional behavior
    void load_from_file(const std::string & path, Repo * repo);
    void load_installed();
    GroupSack & get_group_sack() { return group_sack; }
    Pool * get_pool() { return pool; }

private:
    libdnf::Base & base;
    GroupSack group_sack{*this};
    Pool * pool;
    std::set<Id> processed_solvable_ids;
};


}  // namespace libdnf::comps


#endif  // LIBDNF_COMPS_COMPS_HPP
