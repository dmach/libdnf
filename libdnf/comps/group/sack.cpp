#include "libdnf/comps/group/sack.hpp"
#include "libdnf/comps/group/group.hpp"

#include "libdnf/comps/group/sack_impl.hpp"
#include "libdnf/comps/comps.hpp"


namespace libdnf::comps {


GroupSackWeakPtr GroupSack::get_weak_ptr() {
    return GroupSackWeakPtr(this, &p_impl->data_guard);
}

GroupSack::GroupSack(Comps & comps)
    : comps{comps}
    , p_impl{new Impl()}
{
}

GroupSack::~GroupSack() {}


void GroupSack::add_group(std::unique_ptr<Group> && group) {
    add_item(std::move(group));
}


}  // namespace libdnf::comps
