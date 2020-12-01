#include <libdnf/comps/comps.hpp>
#include <libdnf/comps/group/group-private.hpp>
#include <libdnf/utils/xml.hpp>

extern "C" {
#include <solv/pool.h>
#include <solv/repo.h>
#include <solv/repo_comps.h>
#include <solv/repodata.h>
}

#include <cstring>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <unistd.h>

namespace libdnf::comps {


void Comps::load_installed() {
    auto core = get_group_sack().new_group();
    core->set_id("core");
    core->set_name("Core");
    core->add_repo("@System");
    // TODO(dmach): load from transaction database
}


void Comps::load_from_file(const std::string & path, Repo * repo) {
    FILE * xml_doc = fopen(path.c_str(), "r");
    // TODO(pkratoch): libsolv doesn't support environments yet
    repo_add_comps(repo, xml_doc, 0);
    fclose(xml_doc);

    Id solvable_id;
    // loop over all solvables that haven't been processed yet
    FOR_POOL_SOLVABLES(solvable_id) {
        if (processed_solvable_ids.find(solvable_id) != processed_solvable_ids.end()) {
            continue;
        }
        processed_solvable_ids.insert(solvable_id);

        auto group = std::make_unique<Group>();

        load_group_from_solvable(*group, solvable_id, pool);

        // query sack for available (not installed) groups with given id
        auto q = get_group_sack().new_query();
        q.ifilter_installed(false);
        q.ifilter_id(libdnf::sack::QueryCmp::EQ, group->get_id());

        if (q.empty()) {
            // move the newly created group to the sack
            get_group_sack().add_group(std::move(group));
        } else {
            // update an existing group
            auto existing_group = q.get();
            *existing_group.get() += *group;
        }
    }
}


}  // namespace libdnf::comps
