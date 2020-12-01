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


#ifndef LIBDNF_COMPS_GROUP_PACKAGE_HPP
#define LIBDNF_COMPS_GROUP_PACKAGE_HPP

#include <string>


namespace libdnf::comps {


enum package_type {MANDATORY, DEFAULT, OPTIONAL, CONDITIONAL};

// TODO(dmach): isn't it more a package dependency rather than a package?


/// @replaces dnf:dnf/comps.py:class:Package
/// @replaces dnf:dnf/comps.py:class:CompsTransPkg
class Package {
public:
    // lukash: Why is there Package in comps?
    /// @replaces dnf:dnf/comps.py:attribute:Package.name
    std::string get_name() const;
    void set_name(const std::string & value) { name = value; }

    package_type get_type() const;
    void set_type(package_type & value) { type = value; }

    std::string get_condition() const;
    void set_condition(const std::string & value) { condition = value; }

private:
    std::string name;
    package_type type;
    std::string condition;
};


}  // namespace libdnf::comps


/*
dnf:dnf/comps.py:attribute:Package.option_type
*/

#endif
