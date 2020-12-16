#include <libdnf/comps/group/group.hpp>
#include <libdnf/comps/group/query.hpp>
#include <libdnf/utils/xml.hpp>
#include <libxml/tree.h>

extern "C" {
#include <solv/pool.h>
#include <solv/repo.h>
#include <solv/repodata.h>
}

#include <cstring>
#include <iostream>


namespace libdnf::comps {


Group::~Group() {}
    
std::string solvables_lookup_str(std::vector<Solvable *> solvables, Id key) {
    for (auto solvable: solvables) {
        if (solvable_lookup_str(solvable, key)) {
            return solvable_lookup_str(solvable, key);
        }
    }
    return "";
}


std::string Group::get_id() const {
    std::string solvable_name(solvables_lookup_str(get_solvables(), SOLVABLE_NAME));
    return solvable_name.substr(solvable_name.find(":") + 1);
}


std::string Group::get_name() const {
    return solvables_lookup_str(get_solvables(), SOLVABLE_SUMMARY);
}


std::string Group::get_description() const {
    return solvables_lookup_str(get_solvables(), SOLVABLE_DESCRIPTION);
}


std::string Group::get_translated_name(const char * lang) const {
    std::string translation;
    for (auto solvable: get_solvables()) {
        if (solvable_lookup_str_lang(solvable, SOLVABLE_SUMMARY, lang, 1)) {
            translation = solvable_lookup_str_lang(solvable, SOLVABLE_SUMMARY, lang, 1);
            // Return translation only if it's different from the untranslated string.
            if (translation != solvable_lookup_str(solvable, SOLVABLE_SUMMARY)) {
                return translation;
            }
        }
    }
    // Return the untranslated name when no translation was found
    return this->get_name();
}


std::string Group::get_translated_description(const char * lang) const {
    std::string translation;
    for (auto solvable: get_solvables()) {
        if (solvable_lookup_str_lang(solvable, SOLVABLE_DESCRIPTION, lang, 1)) {
            translation = solvable_lookup_str_lang(solvable, SOLVABLE_DESCRIPTION, lang, 1);
            // Return translation only if it's different from the untranslated string.
            if (translation != solvable_lookup_str(solvable, SOLVABLE_DESCRIPTION)) {
                return translation;
            }
        }
    }
    // Return the untranslated description when no translation was found
    return this->get_description();
}


std::string Group::get_order() const {
    return solvables_lookup_str(get_solvables(), SOLVABLE_ORDER);
}


std::string Group::get_langonly() const {
    return solvables_lookup_str(get_solvables(), SOLVABLE_LANGONLY);
}


bool Group::get_uservisible() const {
    return solvable_lookup_void(get_solvables()[0], SOLVABLE_ISVISIBLE);
}


bool Group::get_default() const {
    return solvable_lookup_void(get_solvables()[0], SOLVABLE_ISDEFAULT);
}

/*
std::set<std::string> Group::get_repos() const {
    for (auto solvable: get_solvables()) {
        if (solvable_lookup_str(solvable, repo_id)) {
            return solvable_lookup_str(solvable, key);
        }
    }
    //group.add_repo(pool_id2str(pool, solvable->repo->repoid));
    std::set<std::string> empty;
    return empty;
}


bool Group::get_installed() const {
    return get_repos().find("@System") != get_repos().end();
}
*/

Group & Group::operator+=(const Group & rhs) {
    this->solvables.insert(this->solvables.begin(), rhs.solvables.begin(), rhs.solvables.end());
    return *this;
}



// TODO(pkratoch): Store also packagelist
void load_group_from_solvable(Group & group, Id solvable_id, Pool * pool) {
    Solvable * solvable = pool_id2solvable(pool, solvable_id);
    group.add_solvable(solvable);
}

Group::Group(GroupQuery * query) : query(query->get_weak_ptr()) {}

}  // namespace libdnf::comps
