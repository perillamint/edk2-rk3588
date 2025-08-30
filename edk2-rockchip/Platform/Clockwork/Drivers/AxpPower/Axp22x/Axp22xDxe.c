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

#include <Uefi.h>


#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/SmbusHc.h>
#include <Protocol/I2cIo.h>
#include <Protocol/I2c.h>
#include <Protocol/AxpPower.h>
#include <Axp22x.h>

STATIC EFI_STATUS Axp22xI2cRead(
  IN CONST AXP_POWER_PROTOCOL *This,
  UINT8 ChipAddress,
  UINT8 DeviceRegister,
  UINT8 *Buffer
) {
  EFI_I2C_REQUEST_PACKET    *RequestPacket;
  UINTN                     RequestPacketSize;
  EFI_STATUS                Status                  = EFI_SUCCESS;

  ASSERT (This -> I2cIo != NULL);

  RequestPacketSize  = sizeof (UINTN) + sizeof (EFI_I2C_OPERATION) * 2;
  RequestPacket      = AllocateZeroPool (RequestPacketSize);

  if (RequestPacket == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  RequestPacket -> OperationCount = 2;

  RequestPacket->Operation[0].Flags         = 0;
  RequestPacket->Operation[0].LengthInBytes = 1;
  RequestPacket->Operation[0].Buffer        = &DeviceRegister;

  RequestPacket->Operation[1].Flags         = I2C_FLAG_READ;
  RequestPacket->Operation[1].LengthInBytes = 1;
  RequestPacket->Operation[1].Buffer        = Buffer;

  This -> I2cIo -> QueueRequest (
    This -> I2cIo,
    0,
    NULL,
    RequestPacket,
    NULL
  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Axp22xI2cRead: error %d during transmission\n", Status));
  }

  return Status;
}

STATIC EFI_STATUS Axp22xI2cWrite(
  IN CONST AXP_POWER_PROTOCOL *This,
  UINT8 ChipAddress,
  UINT8 DeviceRegister,
  UINT8 data
) {
  EFI_I2C_REQUEST_PACKET    *RequestPacket;
  UINTN                     RequestPacketSize;
  EFI_STATUS                Status                  = EFI_SUCCESS;
  UINT8                     *Data;

  ASSERT (This -> I2cIo != NULL);

  RequestPacketSize  = sizeof (UINTN) + sizeof (EFI_I2C_OPERATION) * 2;
  RequestPacket      = AllocateZeroPool (RequestPacketSize);

  if (RequestPacket == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Data = AllocateZeroPool (1 + 1); //register + data
  if (Data == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Data[0] = DeviceRegister;
  Data[1] = data;

  RequestPacket -> OperationCount = 1;
  
  RequestPacket->Operation[0].Flags         = 0;
  RequestPacket->Operation[0].LengthInBytes = 2;
  RequestPacket->Operation[0].Buffer        = Data;

  This -> I2cIo -> QueueRequest (
    This -> I2cIo,
    0,
    NULL,
    RequestPacket,
    NULL
  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Axp22xI2cRead: error %d during transmission\n", Status));
  }

  return Status;
}

AXP_PM_BUS_OPS Axp22xPmBusOps = {
.AxpPmBusRead  = Axp22xI2cRead,
.AxpPmBusWrite = Axp22xI2cWrite
};

#include <Protocol/DriverBinding.h>

#include <Library/PcdLib.h>

#include <Protocol/I2cIo.h>
#include <Protocol/I2c.h>

// Hardcoded in Rockchip drivers...
#define I2C_GUID \
  { \
  0xadc1901b, 0xb83c, 0x4831, { 0x8f, 0x59, 0x70, 0x89, 0x8f, 0x26, 0x57, 0x1e } \
  }

STATIC CONST EFI_GUID  I2cGuid = I2C_GUID;

EFI_STATUS EFIAPI Axp22xSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
);

EFI_STATUS EFIAPI Axp22xStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                  ControllerHandle,
  IN  EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath OPTIONAL
);

typedef struct {
  UINT32                       Signature;
  EFI_HANDLE                   ControllerHandle;
  EFI_I2C_IO_PROTOCOL          *I2cIo;
  AXP_POWER_PROTOCOL           AxpPowerProtocol;
} AXP22X_REGULATOR_CONTEXT;

EFI_DRIVER_BINDING_PROTOCOL gDriverBindingProtocol = {
  Axp22xSupported,
  Axp22xStart,
  NULL
};


EFI_STATUS EFIAPI Axp22xInitialize(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable) {
  EFI_STATUS  Status;

  DEBUG ((DEBUG_INFO, "Initializing Axp22x driver...\n"));

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gEfiDriverBindingProtocolGuid,
                  &gDriverBindingProtocol,
                  NULL
                  );

  return Status;
}

EFI_STATUS EFIAPI Axp22xSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
) {
  EFI_STATUS           Status = EFI_UNSUPPORTED;
  EFI_I2C_IO_PROTOCOL  *TmpI2cIo;
  UINT8                Axp22xAddress;
  UINT8                Axp22xBus;

  DEBUG ((DEBUG_INFO, "Axp22xSupported: Probing Axp22x (sheeeeeee... it is dummy currently)\n"));

  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiI2cIoProtocolGuid,
                  (VOID **)&TmpI2cIo,
                  gImageHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  Axp22xAddress = PcdGet8 (PcdAxp22xAddress);
  Axp22xBus    = PcdGet8 (PcdAxp22xBus);

  DEBUG ((DEBUG_INFO, "Axp22xSupported: Using AXP22x address 0x%02x on bus 0x%02x\n", Axp22xAddress, Axp22xBus));
  DEBUG ((DEBUG_INFO, "Axp22xSupported: Looking for I2C device with GUID %g and index %d\n", &I2cGuid, I2C_DEVICE_INDEX(Axp22xBus, Axp22xAddress)));

  // Check I2C device index
  if (CompareGuid (TmpI2cIo->DeviceGuid, &I2cGuid) &&
      (TmpI2cIo->DeviceIndex == I2C_DEVICE_INDEX (
                                  Axp22xBus,
                                  Axp22xAddress
                                  )))
  {
    DEBUG ((DEBUG_INFO, "Axp22xSupported: Attached to Axp22x device\n"));
    DEBUG ((DEBUG_INFO, "Axp22xSupported: I2cIo %p, idx = %d\n", TmpI2cIo, TmpI2cIo->DeviceIndex));
    Status = EFI_SUCCESS;
  }

  gBS->CloseProtocol (
         ControllerHandle,
         &gEfiI2cIoProtocolGuid,
         gImageHandle,
         ControllerHandle
         );

  return Status;
}

EFI_STATUS EFIAPI Axp22xStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                  ControllerHandle,
  IN  EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath OPTIONAL
) {
  EFI_STATUS           Status = EFI_SUCCESS;
  AXP22X_REGULATOR_CONTEXT *Axp22xRegulatorContext;

  DEBUG ((DEBUG_INFO, "Axp22xStart: Starting Axp22x driver\n"));

  Axp22xRegulatorContext = AllocateZeroPool (sizeof (AXP22X_REGULATOR_CONTEXT));
  if (Axp22xRegulatorContext == NULL) {
    DEBUG ((DEBUG_ERROR, "Axp22xStart: Failed to allocate memory for AXP22X_REGULATOR_CONTEXT\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  Axp22xRegulatorContext->ControllerHandle = ControllerHandle;

  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiI2cIoProtocolGuid,
                  (VOID **)&Axp22xRegulatorContext->I2cIo,
                  gImageHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Axp22xStart: Failed to open I2C IO protocol\n"));
    FreePool (Axp22xRegulatorContext);
    return EFI_UNSUPPORTED;
  }

  DEBUG ((DEBUG_INFO, "Axp22xStart: I2cIo %p, idx = %d\n", Axp22xRegulatorContext->I2cIo, Axp22xRegulatorContext->I2cIo->DeviceIndex));

  AXP_POWER_PROTOCOL *Axp22xPowerProtocol = &Axp22xRegulatorContext->AxpPowerProtocol;

  Axp22xPowerProtocol->Identifier = Axp22xRegulatorContext->I2cIo->DeviceIndex;
  Axp22xPowerProtocol->I2cIo = Axp22xRegulatorContext->I2cIo;
  Axp22xPowerProtocol->Probe = Axp22xProbe;
  Axp22xPowerProtocol->SetSupplyStatus = Axp22xSetSupplyStatus;
  Axp22xPowerProtocol->SetSupplyStatusByName = Axp22xSetSupplyStatusByName;
  Axp22xPowerProtocol->ProbeSupplyStatus = Axp22xProbeSupplyStatus;
  
  Axp22xPowerProtocol->SetNextSystemMode = Axp22xSetNextSysMode;
  Axp22xPowerProtocol->ProbePreSystemMode = Axp22xProbePreSysMode;
  Axp22xPowerProtocol->ProbeThisPowerOnCause = Axp22xProbeThisPowerOnCause;

  Axp22xPowerProtocol->ProbePowerBusExistance = Axp22xProbePowerBusExistance;
  
  Axp22xPowerProtocol->ProbeBatteryVoltage = Axp22xProbeBatteryVoltage;
  Axp22xPowerProtocol->ProbeBatteryRatio = Axp22xProbeBatteryRatio;
  Axp22xPowerProtocol->ProbeBatteryExistance = Axp22xProbeBatteryExistance;

  Axp22xPowerProtocol->ProbePowerKey = Axp22xProbePowerKey;
  
  Axp22xPowerProtocol->SetPowerOff = Axp22xSetPowerOff;
  Axp22xPowerProtocol->SetPowerOnOffVoltage = Axp22xSetPowerOnoffVoltage;
  
  Axp22xPowerProtocol->SetChargerOnOff = Axp22xSetChargerOnOff;
  Axp22xPowerProtocol->SetVbusCurrentLimit = Axp22xSetVbusCurrentLimit;
  Axp22xPowerProtocol->SetVbusVoltageLimit = Axp22xSetVbusVoltagelimit;
  Axp22xPowerProtocol->SetChargeCurrent = Axp22xSetChargeCurrent;
  Axp22xPowerProtocol->ProbeChargeCurrent = Axp22xProbeChargeCurrent;

  Axp22xPowerProtocol->ProbeIntPendding = Axp22xProbeIntPending;
  Axp22xPowerProtocol->ProbeIntEnable = Axp22xProbeIntEnable;
  Axp22xPowerProtocol->SetIntEnable = Axp22xSetIntEnable;
  Axp22xPowerProtocol->SetIntDisable = Axp22xSetIntDisable;

  DEBUG ((DEBUG_INFO, "Installing Axp22xPowerProtocol handler...\n"));
  Status = gBS->InstallMultipleProtocolInterfaces(
    &ControllerHandle,
    &gAxpPowerProtocolGuid,
    Axp22xPowerProtocol,
    NULL
  );
  return Status;

//fail:
//  FreePool (Axp22xRegulatorContext);
//  gBS->CloseProtocol (
//         ControllerHandle,
//         &gEfiI2cIoProtocolGuid,
//         gImageHandle,
//         ControllerHandle
//         );
//
//  return Status;
}
