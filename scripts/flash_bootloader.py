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

import argparse
import json
import subprocess

parser = argparse.ArgumentParser(description =
    "Script to flash Rasbperry PICO from command line")
parser.add_argument("--mount", dest="mount",
    action="store", help="Mount point for Raspberry PICO", required=True)
parser.add_argument("--image", dest="image",
    action="store", help="Path to image for flash", required=True)

args, rest = parser.parse_known_args()

drives = subprocess.check_output(["lsblk -O -J"], stderr=subprocess.PIPE, shell=True)

y = json.loads(drives)

rc = subprocess.call(["mount " + args.mount], shell=True)

if rc == 0:
    subprocess.call(["cp " + args.image + " /mnt/rpi_pico -v"], shell=True)
else:
    print("Mouting failed")


