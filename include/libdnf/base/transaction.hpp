/*
Copyright (C) 2021 Red Hat, Inc.

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


#ifndef LIBDNF_BASE_TRANSACTION_HPP
#define LIBDNF_BASE_TRANSACTION_HPP

#include "libdnf/rpm/package.hpp"
#include "libdnf/transaction/transaction_item_action.hpp"
#include "libdnf/transaction/transaction_item_reason.hpp"
#include "libdnf/transaction/transaction_item_state.hpp"
#include "libdnf/transaction/transaction_item_type.hpp"


namespace libdnf {

class Goal;

}  // namespace libdnf

namespace libdnf::base {

class Transaction;

class TransactionItem {
public:
    using Action = transaction::TransactionItemAction;
    using Reason = transaction::TransactionItemReason;
    using State = transaction::TransactionItemState;
    using Type = transaction::TransactionItemType;

    TransactionItem(Action action, Reason reason) : action(action), reason(reason) {}

    /// Get action associated with the transaction item in the transaction
    ///
    /// @replaces libdnf:transaction/TransactionItem.hpp:method:TransactionItemBase.getAction()
    Action get_action() const noexcept { return action; }

    /// Get reason of the action associated with the transaction item in the transaction
    ///
    /// @replaces libdnf:transaction/TransactionItem.hpp:method:TransactionItemBase.getReason()
    Reason get_reason() const noexcept { return reason; }

    /// Set reason of the action associated with the transaction item in the transaction
    ///
    /// @replaces libdnf:transaction/TransactionItem.hpp:method:TransactionItemBase.setReason(libdnf::TransactionItemReason value)
    void set_reason(Reason value) { reason = value; }

    /// Get transaction item state
    ///
    /// @replaces libdnf:transaction/TransactionItem.hpp:method:TransactionItemBase.getState()
    State get_state() const noexcept { return state; }

private:
    void set_state(State value) noexcept { state = value; }

    Action action;
    Reason reason;
    State state = State::UNKNOWN;
};

class TransactionPackageItem : public rpm::Package, public TransactionItem {
public:
private:
    friend class Transaction;

    TransactionPackageItem(libdnf::rpm::Package pkg, Action action, Reason reason)
        : rpm::Package(pkg),
          TransactionItem(action, reason) {}
};

class Transaction {
public:
    Transaction(const Transaction & transaction);
    ~Transaction();

    /// Return all rpm packages associated with the transaction
    ///
    /// @replaces libdnf:transaction/Transaction.hpp:method:Transaction.getItems()
    const std::vector<TransactionPackageItem> & get_packages() const noexcept;


private:
    friend class libdnf::Goal;

    Transaction(const BaseWeakPtr & base);

    class Impl;
    std::unique_ptr<Impl> p_impl;
};

}  // namespace libdnf::base

#endif  // LIBDNF_BASE_TRANSACTION_HPP