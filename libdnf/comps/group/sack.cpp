#include "libdnf/comps/group/sack.hpp"
#include "libdnf/comps/group/sack_impl.hpp"
#include "libdnf/comps/comps.hpp"


namespace libdnf::comps {


GroupSackWeakPtr GroupSack::get_weak_ptr() {
    return GroupSackWeakPtr(this, &p_impl->data_guard);
}

GroupSack::~GroupSack() {}


void GroupSack::add_group(std::unique_ptr<Group> && group) {
    add_item(std::move(group));
}


}  // namespace libdnf::comps
