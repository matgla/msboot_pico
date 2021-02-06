// This file is part of MSBOOT pico project.
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
#include <cstdint>
#include <cstdio>

#include <unistd.h>

#include <pico/stdlib.h>

extern "C" 
{
#include <hardware/flash.h> 
#include <pico/bootrom.h>
}
/* 128 KB registered for bootloader */

// MEMORY MAP // 
// 0-128K Bootloader 
// 128K-132K Configuration Memory 
// 132K-... FOR OS 
//

#include <eul/crc/crc.hpp>

#include "configuration.hpp"
#include "connection.hpp"

constexpr uint32_t flash_drive_offset = (256 * 1024); 
constexpr uint32_t bootloader_start = XIP_BASE;
constexpr uint32_t configuration_memory_block = bootloader_start + flash_drive_offset;

void start()
{
    printf("===============================\n");
    printf("=       MSOS Bootloader       =\n");
    printf("===============================\n");
   
    printf("Bootloader version: 0.1\n");

    printf("Bootloader start:  0x%x\n", bootloader_start);
    printf("Bootloader end:    0x%x\n", bootloader_start + flash_drive_offset);

    msboot::ConfigurationLoader config_loader; 
    uint32_t* configuration_memory = reinterpret_cast<uint32_t*>(configuration_memory_block);
    if (config_loader.load_from_memory(configuration_memory) == false)
    {
        printf("No valid configuration found!\n");
        config_loader.print_crc();
        // Wait for Software, but for test write some
        msboot::Configuration conf; 
        conf.firmware_address = 0x12345678;
        conf.firmware_crc = 0x12121222;
        conf.firmware_size = 1024; 
        conf.version_major = 1;
        conf.version_minor = 2;
        msboot::ConfigurationWriter writer(conf);
        writer.write(flash_drive_offset);

    }
    config_loader.print();
}

void __no_inline_not_in_flash_func(clear_configuration)()
{
    flash_range_erase(flash_drive_offset, FLASH_SECTOR_SIZE);
}

void prepare_for_flashing()
{
    uint32_t size;
    read(STDIN_FILENO, &size, sizeof(size));
    printf("Firmware size to fetch: %d\n", size);
    uint32_t firmware_crc; 
    read(STDIN_FILENO, &firmware_crc, sizeof(firmware_crc));
    printf("Firmware CRC: 0x%x\n", firmware_crc);

    uint8_t buffer[255];
    for (uint32_t i = 0; i < size; ++i)
    {
        uint8_t byte;
        read(STDIN_FILENO, &byte, sizeof(byte));
        buffer[i] = byte;
    }

    uint32_t crc = calculate_crc32(std::span<uint8_t>(buffer, size));
    printf("Got firmware\n");
    printf("CRC: 0x%x\n", crc);
}

int main() 
{
    stdio_init_all(); 
 
    msboot::Connection connection;
    while (true) 
    {
        char c = getchar();
        connection.run(c);
        //char c = getchar(); 
        //if (c == 's') 
        //{
        //    start();
        //}
        //if (c == 'c')
        //{
        //    clear_configuration();
        //}
        //if (c == 'f')
        //{
        //    prepare_for_flashing();
        //}
        //if (c == 'q')
        //{
        //    reset_usb_boot(0, 0);
        //}
    }
}
