## @file
#
#  Copyright (c) 2023-2024, Mario Bălănică <mariobalanica02@gmail.com>
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

  # Axp22x PMIC
  INF $(VENDOR_DIRECTORY)/Drivers/AxpPower/Axp22x/Axp22xDxe.inf

  # ACPI Support
  INF RuleOverride = ACPITABLE $(PLATFORM_DIRECTORY)/AcpiTables/AcpiTables.inf

  # Device Tree Support
  #  INF RuleOverride = DTB $(PLATFORM_DIRECTORY)/DeviceTree/Vendor.inf
  INF RuleOverride = DTB $(PLATFORM_DIRECTORY)/DeviceTree/Mainline.inf

  # Splash screen logo
  INF $(VENDOR_DIRECTORY)/Drivers/LogoDxe/LogoDxe.inf