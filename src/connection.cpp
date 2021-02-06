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

#include "connection.hpp"

#include <cstdio>
#include <unistd.h>

namespace msboot 
{

constexpr char clear_command = 'c';
constexpr char start_command = 's';
constexpr char flash_command = 'f';
constexpr char reset_to_picoboot = 'q';

Connection::Connection()
    : on_start_(nullptr)
    , on_clear_(nullptr)
    , on_flash_(nullptr)
    , on_reset_to_picoboot_(nullptr)
{

}

void Connection::on_start(void (*callback)())
{
    on_start_ = callback;
}

void Connection::on_clear(void (*callback)())
{
    on_clear_ = callback;
}

void Connection::on_flash(void (*callback)())
{
    on_flash_ = callback;
}

void Connection::on_reset_to_picoboot(void (*callback)())
{
    on_reset_to_picoboot_ = callback;
}

void Connection::run(const char c) 
{
    switch (c) 
    {
        case clear_command: 
        {
            
        } break; 
        case start_command:
        {
    
        } break;
        case flash_command:
        {

        } break; 
        case reset_to_picoboot: 
        {

        } break; 
        default: 
        {
        }
    }
}

} // namespace msboot 

