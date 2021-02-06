# This file is part of MSBOOT Pico project.
# Copyright (C) 2021 Mateusz Stadnik
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

# !/bin/python3

import serial

import time
import struct
import binascii

with serial.Serial("/dev/ttyACM0") as ser:
    time.sleep(1)
    ser.write('f'.encode('utf-8'))
    array = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
    binary_data = bytes(array)
    data = struct.pack("I", len(array))

    ser.write(data)
    line = ser.readline()
    print(line.decode('utf-8').replace('\n', ''))

    data = struct.pack("I", binascii.crc32(binary_data))
    ser.write(data)

    line = ser.readline()
    print(line.decode('utf-8').replace('\n', ''))

    data = bytes(array)
    ser.write(data)

    line = ser.readline()
    print(line.decode('utf-8').replace('\n', ''))

    line = ser.readline()
    print(line.decode('utf-8').replace('\n', ''))
