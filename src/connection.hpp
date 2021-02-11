// This file is part of MSBOOT project.
// Copyright (C) 2021 Mateusz Stadnik
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once 

#include <cstdint>

extern "C"
{
#include <hardware/flash.h>
}

namespace msboot
{

class Connection 
{
public: 
    Connection();

    void run(const char c);

    void on_start(void (*callback)());
    void on_clear(void (*callback)());
    void on_flash(void (*callback)());
    void on_reset_to_picoboot(void (*callback)());
    void on_boot(void (*callback)());
private:
    uint8_t buffer_[FLASH_PAGE_SIZE];

    void (*on_start_)();
    void (*on_clear_)();
    void (*on_flash_)();
    void (*on_reset_to_picoboot_)();
    void (*on_boot_)();
};

} // namespace msboot 

