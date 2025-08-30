/** @file
*
*  Copyright (c) 2007-2014, Allwinner Technology Co., Ltd. All rights reserved.
*  http://www.allwinnertech.com
*
*  Martin.Zheng <martinzheng@allwinnertech.com>
*  
*  This program and the accompanying materials                          
*  are licensed and made available under the terms and conditions of the BSD License         
*  which accompanies this distribution.  The full text of the license may be found at        
*  http://opensource.org/licenses/bsd-license.php                                            
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             
*
**/

#ifndef   __AXP22X_H__
#define   __AXP22X_H__

#include <Axp22xReg.h>

extern EFI_STATUS Axp22xProbe(IN CONST AXP_POWER_PROTOCOL *This);
extern EFI_STATUS Axp22xSetChargerOnOff(IN CONST AXP_POWER_PROTOCOL *This, IN UINTN OnOff);
extern EFI_STATUS Axp22xProbeBatteryRatio(IN CONST AXP_POWER_PROTOCOL *This, OUT UINTN *Ratio);
extern EFI_STATUS Axp22xProbePowerBusExistance(IN CONST AXP_POWER_PROTOCOL *This, OUT UINTN *Status);
extern EFI_STATUS Axp22xProbeBatteryExistance(IN CONST AXP_POWER_PROTOCOL *This, OUT UINTN *Status);
extern EFI_STATUS Axp22xProbeBatteryVoltage(IN CONST AXP_POWER_PROTOCOL *This, OUT UINTN *Voltage);
extern EFI_STATUS Axp22xProbePowerKey(IN CONST AXP_POWER_PROTOCOL *This, OUT UINTN *Pressed);
extern EFI_STATUS Axp22xProbePreSysMode(IN CONST AXP_POWER_PROTOCOL *This, OUT UINTN *Status);
extern EFI_STATUS Axp22xSetNextSysMode(IN CONST AXP_POWER_PROTOCOL *This, IN UINTN Status);
extern EFI_STATUS Axp22xProbeThisPowerOnCause(IN CONST AXP_POWER_PROTOCOL *This, IN UINTN *Status);
extern EFI_STATUS Axp22xSetPowerOff(IN CONST AXP_POWER_PROTOCOL *This);
extern EFI_STATUS Axp22xSetPowerOnoffVoltage(IN CONST AXP_POWER_PROTOCOL *This, IN UINTN Voltage,IN UINTN Stage);
extern EFI_STATUS Axp22xSetChargeCurrent(IN CONST AXP_POWER_PROTOCOL *This, IN UINTN Current);
extern EFI_STATUS Axp22xProbeChargeCurrent(IN CONST AXP_POWER_PROTOCOL *This, OUT UINTN *Current);
extern EFI_STATUS Axp22xSetVbusCurrentLimit(IN CONST AXP_POWER_PROTOCOL *This, IN UINTN Current);
extern EFI_STATUS Axp22xSetVbusVoltagelimit(IN CONST AXP_POWER_PROTOCOL *This, IN UINTN Voltage);
extern EFI_STATUS Axp22xProbeIntPending(IN CONST AXP_POWER_PROTOCOL *This, OUT UINT64 *IntMask);
extern EFI_STATUS Axp22xProbeIntEnable(IN CONST AXP_POWER_PROTOCOL *This, OUT UINT64 *IntMask);
extern EFI_STATUS Axp22xSetIntEnable(IN CONST AXP_POWER_PROTOCOL *This, IN UINT64 IntMask);
extern EFI_STATUS Axp22xSetIntDisable(IN CONST AXP_POWER_PROTOCOL *This, IN UINT64 IntMask);
extern EFI_STATUS Axp22xSetSupplyStatus(IN CONST AXP_POWER_PROTOCOL *This, IN UINTN VoltageIndex, IN UINTN Voltage, IN INTN  OnOff);
extern EFI_STATUS Axp22xSetSupplyStatusByName(IN CONST AXP_POWER_PROTOCOL *This, IN CHAR8 *VoltageName,IN UINTN  Voltage, IN INTN OnOff);
extern EFI_STATUS Axp22xProbeSupplyStatus(IN CONST AXP_POWER_PROTOCOL *This, IN UINTN VoltageIndex,IN UINTN  *Voltage, IN INTN *OnOff);

extern AXP_PM_BUS_OPS Axp22xPmBusOps; 

/// @brief Read a byte from the AXP PMU via the SMBus.
/// @param chip AXP PMU I2C address.
/// @param addr AXP PMU register address.
/// @param buffer Payload buffer
/// @return 
STATIC inline EFI_STATUS AxpI2cRead(IN CONST AXP_POWER_PROTOCOL *This, UINT8 chip, UINT8 addr, UINT8 *buffer)
{
  return Axp22xPmBusOps.AxpPmBusRead(This, chip, addr, buffer);
}

STATIC inline EFI_STATUS AxpI2cWrite(IN CONST AXP_POWER_PROTOCOL *This, UINT8 chip, UINT8 addr, UINT8 data)
{
  return Axp22xPmBusOps.AxpPmBusWrite(This, chip, addr, data);
}


#endif
