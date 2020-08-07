/*
Copyright (C) 2020 Red Hat, Inc.

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


#include "libdnf-cli/progressbar/download_progress_bar.hpp"
#include "libdnf-cli/progressbar/multi_progress_bar.hpp"

#include <iostream>
#include <chrono>
#include <thread>


using namespace libdnf::cli::progressbar;


int main() {
    MultiProgressBar mbar;
    auto bar1 = new DownloadProgressBar(1000000000, "abc.rpm89012345678901234567890");
    mbar.add_bar(bar1);

    auto bar2 = new DownloadProgressBar(1000000000, "abc.rpm89012345678901234567890");
    mbar.add_bar(bar2);

    bar1->start();
    bar2->start();
    bar2->add_message(MessageType::ERROR, "Connecting to server example.com failed. Trying another mirror....................................................................................................................................");
    while (!bar1->is_finished() || !bar2->is_finished()) {
        bar1->add_ticks(19134565);
        if (bar1->get_ticks() > 19134565 * 50) {
            bar2->add_ticks(19134565);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(12));
        mbar.print();
    }
    // print bar once more to update "Total" and jump to the new line
    bar2->set_state(ProgressBarState::WARNING);
    mbar.print();
    std::cout << std::endl;
    return 0;
}
