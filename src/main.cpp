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

#include <pico/stdlib.h>

extern "C" 
{
#include <hardware/flash.h> 
}
/* 128 KB registered for bootloader */

// MEMORY MAP // 
// 0-128K Bootloader 
// 128K-132K Configuration Memory 
// 132K-... FOR OS 
//

constexpr uint32_t flash_drive_offset = (128 * 1024); 

//jjjxtern "C" 
//{
//    void flash_range_erase(uint32_t offset, size_t sector_size);
//}

int main() 
{
    stdio_init_all(); 
  
    sleep_ms(5000);
    printf("===============================\n");
    printf("=       MSOS Bootloader       =\n");
    printf("===============================\n");
   
    printf("Bootloader version: 0.1\n");
    printf("Bootloader start: 0x10000000\n");
    printf("Bootloader end: 0x10001000\n");

    flash_range_erase(flash_drive_offset, FLASH_SECTOR_SIZE);
   
    while (true) 
    {
        sleep_ms(1000);
        printf("ELO\n");
    }
}
