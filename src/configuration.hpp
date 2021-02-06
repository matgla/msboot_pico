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

#include <pico/platform.h>

namespace msboot
{

struct Configuration
{
public:
    uint8_t version_major;
    uint8_t version_minor;

    uint32_t firmware_address;
    uint32_t firmware_size;
    uint32_t firmware_crc;
};

struct ConfigurationEntry
{
    Configuration configuration;
    uint32_t configuration_crc;
};

class ConfigurationLoader
{
public:
    bool load_from_memory(const uint32_t* address);
    void print() const;
    void print_crc() const;
private: 
    uint32_t crc_;
    ConfigurationEntry configuration_entry_;
    Configuration* configuration_; 
};

void __no_inline_not_in_flash_func(write_configuration)(uint32_t address, const Configuration& data);

class ConfigurationWriter
{
public:
    ConfigurationWriter(const Configuration& configuration);

    bool write(const uint32_t address);
private:
    ConfigurationEntry configuration_;
};

} // namespace msboot

