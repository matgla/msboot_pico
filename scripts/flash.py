#!/bin/python3

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


import serial

import time
import struct
import binascii

import argparse

parser = argparse.ArgumentParser(description="Script to flashing via msboot")
parser.add_argument("--image", dest="image", action="store",
                    help="Image to burn", required=True)

parser.add_argument("--serial", dest="serial", action="store",
                    help="Serial port device", required=True)

args, rest = parser.parse_known_args()

with open(args.image, "rb") as file:
    print ("Opening device: " + args.serial)
    with serial.Serial(args.serial, 115200) as ser:
        array = file.read()
        print("File size: " + str(len(array)))
        time.sleep(1)
        command ='f'.encode('utf-8')
        print ("Command size: ", len(command))
        
        ser.write(command)
        binary_data = bytes(array)
        data = struct.pack("I", len(array))

        print (data, len(data))
        ser.write(data)
        line = ser.readline()
        print(line.decode('utf-8').replace('\n', ''))

        data = struct.pack("I", binascii.crc32(binary_data))
        ser.write(data)

        line = ser.readline()
        print(line.decode('utf-8').replace('\n', ''))

        bytes_transmitted = 0
        while bytes_transmitted < len(array):
            line = ser.readline().decode('utf-8').replace('\n', '')
            print (line)
            bytes_to_send = 0
            if len(array) - bytes_transmitted < 256:
                bytes_to_send = len(array) - bytes_transmitted
            else:
                bytes_to_send = 256
            data = bytes(
                array[
                    bytes_transmitted: bytes_transmitted +
                    bytes_to_send])
            print("sending bytes: " + str(len(data)))
            ser.write(data)
            bytes_transmitted = bytes_transmitted + bytes_to_send
            line = ser.readline().decode('utf-8').replace('\n', '')
            print (line)
        while line.find("==FINISHED==") == -1:
            line = ser.readline().decode('utf-8').replace('\n', '')
            print(line)
