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

cd "${USB_GADGET_PATH}"

if [ ! -d "${USB_DEVICE_DIR}" ]; then
    echo "Gadget does not exist, quitting."
    exit 0
fi

pushd "${USB_DEVICE_DIR}"

# Disable all gadgets
if [ -n "$(cat UDC)" ]; then
  echo "" > UDC
fi

# Walk items in `configs`
for config in ${USB_ALL_CONFIGS_DIR} ; do
    # Exit early if there are no entries
    [ "${config}" == "${USB_ALL_CONFIGS_DIR}" ] && break

    # Remove all functions from config
    for function in ${USB_ALL_FUNCTIONS_DIR} ; do
      file="${config}/$(basename "${function}")"
      [ -e "${file}" ] && rm "${file}"
    done

    # Remove strings in config
    [ -d "${config}/${USB_STRINGS_DIR}" ] && rmdir "${config}/${USB_STRINGS_DIR}"

    rmdir "${config}"
done

# Remove functions
for function in ${USB_ALL_FUNCTIONS_DIR} ; do
    [ -d "${function}" ] && rmdir "${function}"
done

# Remove strings from device
[ -d "${USB_STRINGS_DIR}" ] && rmdir "${USB_STRINGS_DIR}"

popd

rmdir "${USB_DEVICE_DIR}"
