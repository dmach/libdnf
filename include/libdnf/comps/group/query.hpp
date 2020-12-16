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


#ifndef LIBDNF_COMPS_GROUP_QUERY_HPP
#define LIBDNF_COMPS_GROUP_QUERY_HPP

#include "libdnf/utils/weak_ptr.hpp"
#include "libdnf/common/sack/query.hpp"
//#include "libdnf/comps/comps.hpp"
////#include "libdnf/comps/group/sack.hpp"
#include "libdnf/comps/group/group.hpp"
////#include "group.hpp"

#include <memory>


namespace libdnf::comps {


class GroupQuery;
using GroupQueryWeakPtr = WeakPtr<GroupQuery, false>;

class GroupSack;
using GroupSackWeakPtr = WeakPtr<GroupSack, false>;


class GroupQuery : public libdnf::sack::Query<Group> {
public:
    explicit GroupQuery(GroupSack * sack);
    explicit GroupQuery(const Set<Group> & src_set, GroupSack * sack);
    explicit GroupQuery(Set<Group> && src_set, GroupSack * sack);
    ~GroupQuery();

    GroupQuery & ifilter_id(sack::QueryCmp cmp, const std::string & pattern);
    GroupQuery & ifilter_id(sack::QueryCmp cmp, const std::vector<std::string> & patterns);
    GroupQuery & ifilter_uservisible(bool value);
    GroupQuery & ifilter_default(bool value);
    GroupQuery & ifilter_installed(bool value);

    /// Create WeakPtr to GroupQuery
    GroupQueryWeakPtr get_weak_ptr();


private:
    struct F {
        static std::string id(const Group & obj) { return obj.get_id(); }
        static bool is_uservisible(const Group & obj) { return obj.get_uservisible(); }
        static bool is_default(const Group & obj) { return obj.get_default(); }
        static bool is_installed(const Group & obj) { return obj.get_installed(); }
    };

    GroupSackWeakPtr sack;
    class Impl;
    std::unique_ptr<Impl> p_impl;
};


inline GroupQuery & GroupQuery::ifilter_id(sack::QueryCmp cmp, const std::string & pattern) {
    ifilter(F::id, cmp, pattern);
    return *this;
}


inline GroupQuery & GroupQuery::ifilter_id(sack::QueryCmp cmp, const std::vector<std::string> & patterns) {
    ifilter(F::id, cmp, patterns);
    return *this;
}


inline GroupQuery & GroupQuery::ifilter_default(bool value) {
    ifilter(F::is_default, sack::QueryCmp::EQ, value);
    return *this;
}


inline GroupQuery & GroupQuery::ifilter_uservisible(bool value) {
    ifilter(F::is_uservisible, sack::QueryCmp::EQ, value);
    return *this;
}


inline GroupQuery & GroupQuery::ifilter_installed(bool value) {
    ifilter(F::is_installed, sack::QueryCmp::EQ, value);
    return *this;
}


}  // namespace libdnf::comps


#endif  // LIBDNF_COMPS_GROUP_QUERY_HPP
