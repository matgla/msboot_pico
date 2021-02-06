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

#include "configuration.hpp"

#include <cstring>
#include <cstdio>

#include <eul/crc/crc.hpp>

extern "C" 
{
#include <hardware/flash.h>
}

namespace 
{
void print_configuration(const msboot::Configuration* configuration)
{
    printf("Configuration:\n");
    printf("  Firmware version: %d.%d\n", 
        configuration->version_major, configuration->version_minor);
    printf("  Firmware address: 0x%x\n", configuration->firmware_address);
    printf("  Firmware size:    0x%x\n", configuration->firmware_size);
    printf("  Firmware CRC:     0x%x\n", configuration->firmware_crc);
}

}

namespace msboot 
{

bool ConfigurationLoader::load_from_memory(const uint32_t* address)
{
    memcpy(&configuration_entry_, address, sizeof(ConfigurationEntry));
    configuration_ = &configuration_entry_.configuration;
    
    crc_ = calculate_crc32(configuration_);

    if (crc_ != configuration_entry_.configuration_crc)
    {
        return false;
    }

    return true;
}

void ConfigurationLoader::print() const 
{
    print_configuration(configuration_);
}

void ConfigurationLoader::print_crc() const 
{
    printf("Readed CRC: 0x%x\n", configuration_entry_.configuration_crc);
    printf("Calculated CRC: 0x%x\n", crc_);
}

ConfigurationWriter::ConfigurationWriter(const Configuration& configuration)
{
    configuration_.configuration = configuration;
    configuration_.configuration_crc = calculate_crc32(&configuration);
}

void __no_inline_not_in_flash_func(clear_memory)(uint32_t address)
{
    flash_range_erase(address, FLASH_SECTOR_SIZE);
}

void __no_inline_not_in_flash_func(flash_memory)(uint32_t address, const uint8_t* test_data)
{
    flash_range_program(address, test_data, FLASH_PAGE_SIZE);
}

bool ConfigurationWriter::write(const uint32_t address)
{
    uint8_t test_data[FLASH_PAGE_SIZE];
    std::memset(test_data, 0, FLASH_PAGE_SIZE);
    std::memcpy(test_data, &configuration_, sizeof(ConfigurationEntry));
    for (int i = 0; i < 10; i++ )
    {
        printf("%x ", test_data[i]);
    }
    printf(" \n");
    printf("Erasing sector\n");
    clear_memory(address);
    printf("Program page\n");
    flash_memory(address, test_data);
    // flash_range_program(reinterpret_cast<uint32_t>(address), 
        // test_data, 4); 
    printf("Write configuration to: 0x%p\n", address);
    printf("%d \n", *reinterpret_cast<uint32_t*>(XIP_BASE + address));
    print_configuration(&configuration_.configuration);

    printf("In memory: \n");
    ConfigurationEntry* entry = reinterpret_cast<ConfigurationEntry*>(reinterpret_cast<uint32_t*>(XIP_BASE + address));
    print_configuration(&entry->configuration);
    return true;
}

} // namespace msbootk
