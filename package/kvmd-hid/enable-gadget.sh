# MIT License

# Copyright (c) 2020 Michael Lynch

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

#!/bin/bash

# Exit on first error.
set -e

# Echo commands to stdout.
set -x

# Treat undefined environment variables as errors.
set -u

USB_DEVICE_DIR="g1"
USB_GADGET_PATH="/sys/kernel/config/usb_gadget"
USB_DEVICE_PATH="${USB_GADGET_PATH}/${USB_DEVICE_DIR}"

USB_STRINGS_DIR="strings/0x409"
USB_KEYBOARD_FUNCTIONS_DIR="functions/hid.keyboard"
USB_MOUSE_FUNCTIONS_DIR="functions/hid.mouse"
USB_MASS_STORAGE_NAME="mass_storage.0"
USB_MASS_STORAGE_FUNCTIONS_DIR="functions/${USB_MASS_STORAGE_NAME}"

USB_CONFIG_INDEX=1
USB_CONFIG_DIR="configs/c.${USB_CONFIG_INDEX}"
USB_ALL_CONFIGS_DIR="configs/*"
USB_ALL_FUNCTIONS_DIR="functions/*"
USB_MSD_DIR="/mnt/msd/ventoy"
MSD_FILE="none.txt"

modprobe libcomposite

#modprobe configfs
modprobe usb_f_mass_storage

cd "${USB_GADGET_PATH}"
mkdir -p "${USB_DEVICE_DIR}"
cd "${USB_DEVICE_DIR}"

echo 0x1d6b > idVendor  # Linux Foundation
echo 0x0106 > idProduct # Multifunction Composite Gadget
echo 0x0100 > bcdDevice # v1.0.0
echo 0x0200 > bcdUSB    # USB2

mkdir -p "$USB_STRINGS_DIR"
echo "6b65796d696d6570690" > "${USB_STRINGS_DIR}/serialnumber"
echo "smart-kvm" > "${USB_STRINGS_DIR}/manufacturer"
echo "Multifunction USB Device" > "${USB_STRINGS_DIR}/product"

# Keyboard
mkdir -p "$USB_KEYBOARD_FUNCTIONS_DIR"
echo 1 > "${USB_KEYBOARD_FUNCTIONS_DIR}/protocol" # Keyboard
echo 1 > "${USB_KEYBOARD_FUNCTIONS_DIR}/subclass" # Boot interface subclass
echo 8 > "${USB_KEYBOARD_FUNCTIONS_DIR}/report_length"
# Write the report descriptor
D=$(mktemp)
{
  echo -ne \\x05\\x01       # Usage Page (Generic Desktop Ctrls)
  echo -ne \\x09\\x06       # Usage (Keyboard)
  echo -ne \\xA1\\x01       # Collection (Application)
  echo -ne \\x05\\x08       #   Usage Page (LEDs)
  echo -ne \\x19\\x01       #   Usage Minimum (Num Lock)
  echo -ne \\x29\\x03       #   Usage Maximum (Scroll Lock)
  echo -ne \\x15\\x00       #   Logical Minimum (0)
  echo -ne \\x25\\x01       #   Logical Maximum (1)
  echo -ne \\x75\\x01       #   Report Size (1)
  echo -ne \\x95\\x03       #   Report Count (3)
  echo -ne \\x91\\x02       #   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  echo -ne \\x09\\x4B       #   Usage (Generic Indicator)
  echo -ne \\x95\\x01       #   Report Count (1)
  echo -ne \\x91\\x02       #   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  echo -ne \\x95\\x04       #   Report Count (4)
  echo -ne \\x91\\x01       #   Output (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  echo -ne \\x05\\x07       #   Usage Page (Kbrd/Keypad)
  echo -ne \\x19\\xE0       #   Usage Minimum (0xE0)
  echo -ne \\x29\\xE7       #   Usage Maximum (0xE7)
  echo -ne \\x95\\x08       #   Report Count (8)
  echo -ne \\x81\\x02       #   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  echo -ne \\x75\\x08       #   Report Size (8)
  echo -ne \\x95\\x01       #   Report Count (1)
  echo -ne \\x81\\x01       #   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  echo -ne \\x19\\x00       #   Usage Minimum (0x00)
  echo -ne \\x29\\x91       #   Usage Maximum (0x91)
  echo -ne \\x26\\xFF\\x00  #   Logical Maximum (255)
  echo -ne \\x95\\x06       #   Report Count (6)
  echo -ne \\x81\\x00       #   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  echo -ne \\xC0            # End Collection
} >> "$D"
cp "$D" "${USB_KEYBOARD_FUNCTIONS_DIR}/report_desc"
# Enable pre-boot events (if the gadget driver supports it).
if [[ -f "${USB_KEYBOARD_FUNCTIONS_DIR}/no_out_endpoint" ]]; then
  echo 1 > "${USB_KEYBOARD_FUNCTIONS_DIR}/no_out_endpoint"
fi

# Mouse
mkdir -p "$USB_MOUSE_FUNCTIONS_DIR"
echo 0 > "${USB_MOUSE_FUNCTIONS_DIR}/protocol"
echo 0 > "${USB_MOUSE_FUNCTIONS_DIR}/subclass"
echo 7 > "${USB_MOUSE_FUNCTIONS_DIR}/report_length"

# Write the report descriptor
D=$(mktemp)
{
echo -ne \\x05\\x01      # USAGE_PAGE (Generic Desktop)
echo -ne \\x09\\x02      # USAGE (Mouse)
echo -ne \\xA1\\x01      # COLLECTION (Application)
                         #   8-buttons
echo -ne \\x05\\x09      #   USAGE_PAGE (Button)
echo -ne \\x19\\x01      #   USAGE_MINIMUM (Button 1)
echo -ne \\x29\\x08      #   USAGE_MAXIMUM (Button 8)
echo -ne \\x15\\x00      #   LOGICAL_MINIMUM (0)
echo -ne \\x25\\x01      #   LOGICAL_MAXIMUM (1)
echo -ne \\x95\\x08      #   REPORT_COUNT (8)
echo -ne \\x75\\x01      #   REPORT_SIZE (1)
echo -ne \\x81\\x02      #   INPUT (Data,Var,Abs)
                         #   x,y absolute coordinates
echo -ne \\x05\\x01      #   USAGE_PAGE (Generic Desktop)
echo -ne \\x09\\x30      #   USAGE (X)
echo -ne \\x09\\x31      #   USAGE (Y)
echo -ne \\x16\\x00\\x00 #   LOGICAL_MINIMUM (0)
echo -ne \\x26\\xFF\\x7F #   LOGICAL_MAXIMUM (32767)
echo -ne \\x75\\x10      #   REPORT_SIZE (16)
echo -ne \\x95\\x02      #   REPORT_COUNT (2)
echo -ne \\x81\\x02      #   INPUT (Data,Var,Abs)
                         #   vertical wheel
echo -ne \\x09\\x38      #   USAGE (wheel)
echo -ne \\x15\\x81      #   LOGICAL_MINIMUM (-127)
echo -ne \\x25\\x7F      #   LOGICAL_MAXIMUM (127)
echo -ne \\x75\\x08      #   REPORT_SIZE (8)
echo -ne \\x95\\x01      #   REPORT_COUNT (1)
echo -ne \\x81\\x06      #   INPUT (Data,Var,Rel)
                         #   horizontal wheel
echo -ne \\x05\\x0C      #   USAGE_PAGE (Consumer Devices)
echo -ne \\x0A\\x38\\x02 #   USAGE (AC Pan)
echo -ne \\x15\\x81      #   LOGICAL_MINIMUM (-127)
echo -ne \\x25\\x7F      #   LOGICAL_MAXIMUM (127)
echo -ne \\x75\\x08      #   REPORT_SIZE (8)
echo -ne \\x95\\x01      #   REPORT_COUNT (1)
echo -ne \\x81\\x06      #   INPUT (Data,Var,Rel)
echo -ne \\xC0           # END_COLLECTION
} >> "$D"
cp "$D" "${USB_MOUSE_FUNCTIONS_DIR}/report_desc"

if [[ -f "${USB_MOUSE_FUNCTIONS_DIR}/no_out_endpoint" ]]; then
  echo 1 > "${USB_MOUSE_FUNCTIONS_DIR}/no_out_endpoint"
fi

#MSD
mkdir -p "$USB_MASS_STORAGE_FUNCTIONS_DIR"
 
#config msd paramter
shopt -s nullglob
if [ -d "$USB_MSD_DIR" ]
then
	for file in $USB_MSD_DIR/*.img
	do
		if [[ $file == *.img ]] 
		then
			MSD_FILE=$file
			echo "$file" > "${USB_MASS_STORAGE_FUNCTIONS_DIR}/lun.0/file"
			echo 1 > "${USB_MASS_STORAGE_FUNCTIONS_DIR}/lun.0/removable"
			echo 0 > "${USB_MASS_STORAGE_FUNCTIONS_DIR}/lun.0/nofua"
			#echo 0 > functions/mass_storage.0/lun.0/ro
			#echo 1 > functions/mass_storage.0/lun.0/cdrom
			#echo 0 > functions/mass_storage.0/stall
			break
		fi
	done
	
fi


 


mkdir -p "${USB_CONFIG_DIR}"
echo 250 > "${USB_CONFIG_DIR}/MaxPower"

CONFIGS_STRINGS_DIR="${USB_CONFIG_DIR}/${USB_STRINGS_DIR}"
mkdir -p "${CONFIGS_STRINGS_DIR}"
echo "Config ${USB_CONFIG_INDEX}: ECM network" > "${CONFIGS_STRINGS_DIR}/configuration"

ln -s "${USB_KEYBOARD_FUNCTIONS_DIR}" "${USB_CONFIG_DIR}/"
ln -s "${USB_MOUSE_FUNCTIONS_DIR}" "${USB_CONFIG_DIR}/"

#config config.1 link
if [ -f "$MSD_FILE" ] 
then
	ln -s "${USB_MASS_STORAGE_FUNCTIONS_DIR}" "${USB_CONFIG_DIR}/"
fi

ls /sys/class/udc > "${USB_DEVICE_PATH}/UDC"
chmod 777 /dev/hidg0
chmod 777 /dev/hidg1
