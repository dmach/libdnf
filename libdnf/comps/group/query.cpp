#include "libdnf/comps/group/query.hpp"
#include "libdnf/comps/group/query_impl.hpp"
#include "libdnf/comps/group/sack.hpp"


namespace libdnf::comps {


GroupQueryWeakPtr GroupQuery::get_weak_ptr() {
    return GroupQueryWeakPtr(this, &p_impl->data_guard);
}


GroupQuery::GroupQuery(GroupSack * sack) : Query(), sack(sack->get_weak_ptr()) {}
GroupQuery::GroupQuery(const Set<Group> & src_set, GroupSack * sack) : Query(src_set), sack(sack->get_weak_ptr()) {}
GroupQuery::GroupQuery(Set<Group> && src_set, GroupSack * sack) : Query(src_set), sack(sack->get_weak_ptr()) {}

GroupQuery::~GroupQuery() {}

}  // namespace libdnf::comps
