################################################################################
# \file common.mk
# \version 1.0
#
# \brief
# Settings shared across all projects.
#
################################################################################
# \copyright
# Copyright 2025, Cypress Semiconductor Corporation (an Infineon company)
# SPDX-License-Identifier: Apache-2.0
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
################################################################################

MTB_TYPE=PROJECT

# Target board/hardware (BSP).
# To change the target, it is recommended to use the Library manager
# ('make library-manager' from command line), which will also update Eclipse IDE launch
# configurations.
TARGET=KIT_PSE84_EVAL_EPC2

# Name of toolchain to use. Options include:
# ARM      - Arm Compiler for Embedded (must be installed separately - version 6.22).
# LLVM_ARM - LLVM Compiler for Arm ((must be installed separately -version 19.1.5).

TOOLCHAIN=LLVM_ARM

ifeq ($(TOOLCHAIN),ARM)
#   Uncomment after importing the project or set this variable as env variable in the PC.
#    CY_COMPILER_ARM_DIR=C:/Program Files/ArmCompilerforEmbedded6.22
endif

ifeq ($(TOOLCHAIN),LLVM_ARM)
#   Uncomment after importing the project or set this variable as env variable in the PC.
#   CY_COMPILER_LLVM_ARM_DIR=C:/llvm/LLVM-ET-Arm-19.1.5-Windows-x86_64
endif

MTB_SUPPORTED_TOOLCHAINS?=LLVM_ARM ARM
ifeq ($(TOOLCHAIN),GCC_ARM)
$(error This Code example supports only Arm and LLVM compilers)
endif

# Default build configuration. Options include:
#
# Debug   - build with minimal optimizations, focus on debugging.
# Release - build with full optimizations
# Custom  - build with custom configuration, set the optimization flag in CFLAGS
# 
# If CONFIG is manually edited, ensure to update or regenerate launch configurations 
# for your IDE.
CONFIG=Release

#########################################################################################
##################### Voice Core ########################################################

# Voice Core functionality. Options are
# LIMITED - Limited time functionality. (Default)
# FULL    - Full functionality.

CONFIG_VOICE_CORE_MODE=LIMITED

#########################################################################################
##################### AFE Mode ##########################################################

# AE mode : 
# FUNCTIONAL    - Demonstrates Audio Enhancement processing with single USB channel to PC.
# TUNING        - Demonstrates Audio Enhancement processing and tuning with quad USB channels to PC.

CONFIG_AE_MODE=FUNCTIONAL

#########################################################################################


# Search path for
# 1. Common source code shared between CM33 and CM55 cores.
# 2. Third party licensed code.

SEARCH+=../common_modules

# Config file for postbuild sign and merge operations.
# NOTE: Check the JSON file for the command parameters
COMBINE_SIGN_JSON?=configs/boot_with_extended_boot.json

##########################################################################################
##########################################################################################

include ../common_app.mk
