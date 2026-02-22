#!/bin/bash

# Setup script for CH32X033 development environment
# This script installs the USB programming tool (chprog) for WCH microcontrollers

set -e

echo "=========================================="
echo "CH32X033 Development Environment Setup"
echo "=========================================="
echo ""

# Check if running on Linux
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "Detected Linux system"
    echo ""

    # Setup udev rules for USB access
    echo "Setting up udev rules for WCH USB bootloader..."
    echo "This requires sudo privileges."
    echo ""

    echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="4348", ATTR{idProduct}=="55e0", MODE="666"' | sudo tee /etc/udev/rules.d/99-ch55x.rules > /dev/null
    echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="1a86", ATTR{idProduct}=="55e0", MODE="666"' | sudo tee -a /etc/udev/rules.d/99-ch55x.rules > /dev/null

    echo "Reloading udev rules..."
    sudo udevadm control --reload-rules

    echo "✓ udev rules configured successfully"
    echo ""
elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
    echo "Detected Windows system"
    echo ""
    echo "NOTE: On Windows, you need to install the CH372 driver manually."
    echo "Alternatively, use the Zadig tool to install the libusb-win32 driver."
    echo "See the documentation for detailed instructions."
    echo ""
fi

# Check if Python is installed
if ! command -v python3 &> /dev/null && ! command -v python &> /dev/null; then
    echo "ERROR: Python is not installed."
    echo "Please install Python 3.x from https://www.python.org/"
    exit 1
fi

# Determine python command
if command -v python3 &> /dev/null; then
    PYTHON_CMD=python3
    PIP_CMD=pip3
else
    PYTHON_CMD=python
    PIP_CMD=pip
fi

echo "Using Python: $($PYTHON_CMD --version)"
echo ""

# Check if pip is installed
if ! command -v $PIP_CMD &> /dev/null; then
    echo "ERROR: pip is not installed."
    echo "Please install pip for Python package management."
    exit 1
fi

# Install chprog
echo "Installing chprog USB programming tool..."
echo ""

$PIP_CMD install --user chprog

if [ $? -eq 0 ]; then
    echo ""
    echo "✓ chprog installed successfully"
    echo ""
    echo "=========================================="
    echo "Setup completed!"
    echo "=========================================="
    echo ""
    echo "You can now use the following command to flash your firmware:"
    echo "  chprog firmware.bin"
    echo ""
    echo "For more information, see the FLASHING.md documentation."
else
    echo ""
    echo "ERROR: Failed to install chprog"
    echo "Please check your internet connection and try again."
    exit 1
fi
