/*
Copyright (C) 2019-2020 Red Hat, Inc.

This file is part of microdnf: https://github.com/rpm-software-management/libdnf/

Microdnf is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Microdnf is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with microdnf.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "test.hpp"

#include "../../context.hpp"

#include <libdnf/comps/comps.hpp>
#include <libdnf/comps/group/group.hpp>
#include <libdnf/comps/group/sack.hpp>
#include <libdnf/comps/group/query.hpp>
#include <libdnf/conf/option_string.hpp>

extern "C" {
#include <solv/pool.h>
#include <solv/repo.h>
#include <solv/repo_comps.h>
#include <solv/repodata.h>
}

#include <filesystem>
#include <fstream>
#include <iostream>
#include <libsmartcols/libsmartcols.h>
#include <set>
#include <unistd.h>


namespace microdnf {


void CmdTest::set_argument_parser(Context & ctx) {
    auto test = ctx.arg_parser.add_new_command("test");
    test->set_short_description("test command");
    test->set_description("");
    test->set_named_args_help_header("Optional arguments:");
    test->set_positional_args_help_header("Positional arguments:");
    test->set_parse_hook_func([this, &ctx](
                            [[maybe_unused]] libdnf::cli::ArgumentParser::Argument * arg,
                            [[maybe_unused]] const char * option,
                            [[maybe_unused]] int argc,
                            [[maybe_unused]] const char * const argv[]) {
        ctx.select_command(this);
        return true;
    });
    ctx.arg_parser.get_root_command()->register_command(test);
}

void CmdTest::run([[maybe_unused]] Context & ctx) {
    std::filesystem::path data_path = PROJECT_SOURCE_DIR "/test/libdnf/comps/data/";

    libdnf::comps::Comps comps;
    Pool * pool = comps.get_pool();
    const char *langs[] = {"ja"};
    pool_set_languages(pool, langs, sizeof(langs)/sizeof(*langs));
    Repo * repo = repo_create(pool, "repo");

    comps.load_from_file(data_path / "core.xml", repo);
    comps.load_from_file(data_path / "standard.xml", repo);

    Id solvable_id;
    // loop over all package solvables
    FOR_POOL_SOLVABLES(solvable_id) {
        Solvable * solvable = pool_id2solvable(pool, solvable_id);
        std::cout << "  provides:" << std::endl;
        if (solvable->provides) {
            for (Id * r_id = solvable->repo->idarraydata + solvable->provides; *r_id; ++r_id) {
                std::cout << "    " << pool_id2str(pool, *r_id) << std::endl;
            }
        }
        std::cout << "  obsoletes:" << std::endl;
        if (solvable->obsoletes) {
            for (Id * r_id = solvable->repo->idarraydata + solvable->obsoletes; *r_id; ++r_id) {
                std::cout << "    " << pool_id2str(pool, *r_id) << std::endl;
            }
        }
        std::cout << "  conflicts:" << std::endl;
        if (solvable->conflicts) {
            for (Id * r_id = solvable->repo->idarraydata + solvable->conflicts; *r_id; ++r_id) {
                std::cout << "    " << pool_id2str(pool, *r_id) << std::endl;
            }
        }
        std::cout << "  requires:" << std::endl;
        if (solvable->requires) {
            for (Id * r_id = solvable->repo->idarraydata + solvable->requires; *r_id; ++r_id) {
                std::cout << "    " << pool_id2str(pool, *r_id) << std::endl;
            }
        }
        std::cout << "  recommends:" << std::endl;
        if (solvable->recommends) {
            for (Id * r_id = solvable->repo->idarraydata + solvable->recommends; *r_id; ++r_id) {
                std::cout << "    " << pool_id2str(pool, *r_id) << std::endl;
                if (strcmp(pool_id2rel(pool, *r_id), "") != 0) {
                    std::cout << "      " << pool_id2rel(pool, *r_id) << pool_id2evr(pool, *r_id) << std::endl;
                }
            }
        }
        std::cout << "  suggests:" << std::endl;
        if (solvable->suggests) {
            for (Id * r_id = solvable->repo->idarraydata + solvable->suggests; *r_id; ++r_id) {
                std::cout << "    " << pool_id2str(pool, *r_id) << std::endl;
            }
        }
        std::cout << "  supplements:" << std::endl;
        if (solvable->supplements) {
            for (Id * r_id = solvable->repo->idarraydata + solvable->supplements; *r_id; ++r_id) {
                std::cout << "    " << pool_id2str(pool, *r_id) << std::endl;
            }
        }
        std::cout << "  enhances:" << std::endl;
        if (solvable->enhances) {
            for (Id * r_id = solvable->repo->idarraydata + solvable->enhances; *r_id; ++r_id) {
                std::cout << "    " << pool_id2str(pool, *r_id) << std::endl;
            }
        }
        std::cout << std::endl;
    }
}

}  // namespace microdnf
