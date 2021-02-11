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
#include <cstring>


#include <unistd.h>

#include <pico/stdlib.h>

extern "C" 
{
#include <hardware/flash.h> 
#include <pico/bootrom.h>
#include <pico/runtime.h>
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

#include "firmware_loader.hpp"

#include <hardware/sync.h>
#include <hardware/uart.h>

#include "lfs.h"

constexpr uint32_t flash_drive_offset = (256 * 1024); 
constexpr uint32_t bootloader_start = XIP_BASE;
constexpr uint32_t configuration_memory_block = bootloader_start + flash_drive_offset;


constexpr uint32_t firmware_offset = flash_drive_offset + FLASH_SECTOR_SIZE;
constexpr uint32_t firmware_address = bootloader_start + firmware_offset;

#define UART_ID uart0
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
    uint32_t mask = save_and_disable_interrupts();
    flash_range_erase(flash_drive_offset, FLASH_SECTOR_SIZE);
    restore_interrupts(mask);
}

void reset_to_picoboot()
{
    reset_usb_boot(0, 0);
}



void prepare_for_flashing()
{
    uint32_t size;
    uart_read_blocking(UART_ID, reinterpret_cast<uint8_t*>(&size), sizeof(size));
    printf("Firmware size to fetch: %d\n", size);
    uint32_t firmware_crc; 
    uart_read_blocking(UART_ID, reinterpret_cast<uint8_t*>(&firmware_crc), sizeof(firmware_crc));
    printf("Firmware CRC: 0x%x\n", firmware_crc);

    uint8_t buffer[FLASH_PAGE_SIZE];

    uint32_t received_bytes = 0;
    uint32_t pages = 0;

    msboot::FirmwareLoader loader(firmware_offset);
    while (received_bytes < size)
    {
        uint32_t bytes_to_receive = (size - received_bytes) < FLASH_PAGE_SIZE ? (size - received_bytes) : FLASH_PAGE_SIZE;
        printf("Bytes to receive %d\n", bytes_to_receive);
        std::memset(buffer, 0, FLASH_PAGE_SIZE);
        for (uint32_t i = 0; i < bytes_to_receive; ++i)
        {
            uint8_t byte;
            uart_read_blocking(UART_ID, &byte, sizeof(byte));
            buffer[i] = byte;
        }
        received_bytes += bytes_to_receive;
        loader.write_page(pages++, std::span<const uint8_t>(buffer, FLASH_PAGE_SIZE));
        printf("ACK, received bytes: %d\n", received_bytes);
    }
//    uint32_t crc_tmp = calculate_crc32(std::span<const uint8_t>(buffer, size));
 //   printf("CRC Temp: 0x%x\n", crc_tmp);
    const uint8_t* firmware = reinterpret_cast<const uint8_t*>(firmware_address);
    uint32_t crc = calculate_crc32(std::span<const uint8_t>(firmware, size));
    printf("Got firmware\n");

    for (int i = 0; i < 512; ++i)
    {
        printf("0x%x, ", firmware[i]);
    }
    printf("\n");
    if (crc == firmware_crc) 
    {
        printf("Success\n"); 
    }
    else 
    {
        printf("Failed CRC: 0x%x\n", crc);
    }
    printf ("==FINISHED==\n");
}

extern "C" 
{
    void boot_firmware(uint32_t stackpointer, uint32_t entry);
} // extern "C"

void boot() 
{
    printf("Booting from: 0x%x\n", firmware_address);
    //save_and_disable_interrupts();
    uint32_t* VTOR = reinterpret_cast<uint32_t*>(0xE000ED08);
    printf("Old vector: 0x%x\n", *VTOR); 
    uint32_t* firmware = reinterpret_cast<uint32_t*>(firmware_address);
    printf("StackPointer: 0x%x\n", firmware[0]);
    printf("PC: 0x%x\n", firmware[1]);
    uint32_t mask = save_and_disable_interrupts();
    //std::memcpy(reinterpret_cast<uint32_t*>(0x20000000), firmware, sizeof(uint8_t)*256);
    *VTOR = firmware_address;
    restore_interrupts(mask); 
    boot_firmware(firmware[0], firmware[1]);
}

int flash_read(const struct lfs_config* c, lfs_block_t block, 
        lfs_off_t off, void* buffer, lfs_size_t size)
{
    const uint8_t* configuration_memory = reinterpret_cast<const uint8_t*>(configuration_memory_block + block * FLASH_SECTOR_SIZE + off);
    std::memcpy(buffer, configuration_memory, size); 

    return 0;
}

int flash_prog(const struct lfs_config* c, lfs_block_t block, 
        lfs_off_t off, const void* buffer, lfs_size_t size)
{
    const uint32_t mask = save_and_disable_interrupts();
    const uint32_t page_address = flash_drive_offset + block * FLASH_SECTOR_SIZE + off;
    flash_range_program(page_address, static_cast<const uint8_t*>(buffer), size);
    restore_interrupts(mask);
    return 0;
}

int flash_erase(const struct lfs_config* c, lfs_block_t block)
{
    printf ("Erasing block: %d\n", block);
 
    const uint32_t mask = save_and_disable_interrupts();
    const uint32_t block_address = flash_drive_offset + block * FLASH_SECTOR_SIZE;
    flash_range_erase(block_address, FLASH_SECTOR_SIZE);
    restore_interrupts(mask);
    
    return 0;
}

int flash_sync(const struct lfs_config* c)
{
    return 0;
}

int main() 
{
    stdio_init_all(); 

    lfs_t lfs; 
    
    const struct lfs_config cfg = {
        .read = &flash_read,
        .prog = &flash_prog,
        .erase = &flash_erase,
        .sync = &flash_sync,
        .read_size = sizeof(uint8_t),
        .prog_size = FLASH_PAGE_SIZE,
        .block_size = FLASH_SECTOR_SIZE,
        .block_count = 2,
        .block_cycles = -1,
        .cache_size = FLASH_PAGE_SIZE,
        .lookahead_size = FLASH_PAGE_SIZE, 
    };

    int err = lfs_mount(&lfs, &cfg);
    if (err) {
        printf("Mounting failed\n"); 
        lfs_format(&lfs, &cfg);
        lfs_mount(&lfs, &cfg);
    }

    uint32_t boot_count = 0;
    lfs_file_t file;
    lfs_file_open(&lfs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&lfs, &file, &boot_count, sizeof(boot_count));
    
    printf ("Boot number: %d\n", boot_count);
    ++boot_count;
    lfs_file_rewind(&lfs, &file);
    lfs_file_write(&lfs, &file, &boot_count, sizeof(boot_count));
    lfs_file_close(&lfs, &file);
    lfs_unmount(&lfs);

    msboot::Connection connection;
    
    connection.on_clear(&clear_configuration);
    connection.on_start(&start);
    connection.on_reset_to_picoboot(&reset_to_picoboot);
    connection.on_flash(&prepare_for_flashing);
    connection.on_boot(&boot);
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
