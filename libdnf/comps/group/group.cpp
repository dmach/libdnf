#include <libdnf/comps/group/group.hpp>
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


bool Group::get_installed() const {
    return repos.find("@System") != repos.end();
}


Group & Group::operator+=(const Group & rhs) {
    if (!rhs.id.empty()) {
        this->id = rhs.id;
    }
    if (!rhs.name.empty()) {
        this->name = rhs.name;
    }
    if (!rhs.description.empty()) {
        this->description = rhs.description;
    }
    if (!rhs.order.empty()) {
        this->order = rhs.order;
    }
    if (!rhs.langonly.empty()) {
        this->langonly = rhs.langonly;
    }
    this->is_uservisible = rhs.is_uservisible;
    this->is_default = rhs.is_default;
    this->repos.insert(rhs.repos.begin(), rhs.repos.end());
    this->solvables.insert(this->solvables.begin(), rhs.solvables.begin(), rhs.solvables.end());
    return *this;
}


std::string Group::get_translated_name(const char * lang) {
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
    return this->name;
}


std::string Group::get_translated_description(const char * lang) {
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
    return this->description;
}


// TODO(pkratoch): Store also packagelist
void load_group_from_solvable(Group & group, Id solvable_id, Pool * pool) {
    Solvable * solvable = pool_id2solvable(pool, solvable_id);
    std::string solvable_name(pool_id2str(pool, solvable->name));
    group.set_id(solvable_name.substr(solvable_name.find(":") + 1));
    if (pool_lookup_str(pool, solvable_id, SOLVABLE_SUMMARY)) {
        group.set_name(solvable_lookup_str_poollang(solvable, SOLVABLE_SUMMARY));
    }
    if (pool_lookup_str(pool, solvable_id, SOLVABLE_DESCRIPTION)) {
        group.set_description(pool_lookup_str(pool, solvable_id, SOLVABLE_DESCRIPTION));
    }
    group.set_uservisible(pool_lookup_void(pool, solvable_id, SOLVABLE_ISVISIBLE));
    group.set_default(pool_lookup_void(pool, solvable_id, SOLVABLE_ISDEFAULT));
    if (pool_lookup_str(pool, solvable_id, SOLVABLE_ORDER)) {
        group.set_order(pool_lookup_str(pool, solvable_id, SOLVABLE_ORDER));
    }
    if (pool_lookup_str(pool, solvable_id, SOLVABLE_LANGONLY)) {
        group.set_langonly(pool_lookup_str(pool, solvable_id, SOLVABLE_LANGONLY));
    }
    group.add_repo(pool_id2str(pool, solvable->repo->repoid));
    group.add_solvable(solvable);
}


}  // namespace libdnf::comps
