/** @file

  Copyright (c) 2007 - 2013, Allwinner Technology Co., Ltd. <www.allwinnertech.com>

  Martin.zheng <zhengjiewen@allwinnertech.com>
  
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>

#include <Protocol/I2cIo.h>
#include <Protocol/I2c.h>
#include <Protocol/AxpPower.h>
#include <Axp22x.h>

STATIC EFI_STATUS axp22_set_dc1sw(IN CONST AXP_POWER_PROTOCOL *This, int onoff)
{
  UINT8   reg_value;

  if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
  {
    return EFI_DEVICE_ERROR;
  }
  if(onoff)
  {
    reg_value |= (1 << 7);
  }
  else
  {
    reg_value &= ~(1 << 7);
  }
  if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL2, reg_value))
  {
    DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to set dc1sw\n"));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}
/*
************************************************************************************************************
*
*                                             function
*
*    �������ƣ�
*
*    �����б���
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
STATIC EFI_STATUS axp22_set_dc5ldo(IN CONST AXP_POWER_PROTOCOL *This, int onoff)
{
  UINT8   reg_value;

  if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL1, &reg_value))
  {
    return EFI_DEVICE_ERROR;
  }
  if(onoff)
  {
    reg_value |= (1 << 0);
  }
  else
  {
    reg_value &= ~(1 << 0);
  }
  if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL1, reg_value))
  {
    DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to set dc5ldo\n"));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/*
************************************************************************************************************
*
*                                             function
*
*    �������ƣ�
*
*    �����б���
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
STATIC EFI_STATUS axp22_set_dcdc1(IN CONST AXP_POWER_PROTOCOL *This, int set_vol, int onoff)
{
  UINT8   reg_value;

  if(set_vol > 0)
  {
    if(set_vol < 1600)
    {
      set_vol = 1600;
    }
    else if(set_vol > 3400)
    {
      set_vol = 3400;
    }
    if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_DC1OUT_VOL, &reg_value))
    {
      return EFI_DEVICE_ERROR;
    }
    reg_value = ((set_vol - 1600)/100);
    if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_DC1OUT_VOL, reg_value))
    {
      DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to set dcdc1\n"));
      return EFI_DEVICE_ERROR;
    }
  }

  if(onoff < 0)
  {
    return EFI_SUCCESS;
  }
  if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL1, &reg_value))
  {
    return EFI_DEVICE_ERROR;
  }
  if(onoff == 0)
  {
    reg_value &= ~(1 << 1);
  }
  else
  {
    reg_value |=  (1 << 1);
  }
  if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL1, reg_value))
  {
    DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to onoff dcdc1\n"));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}
/*
************************************************************************************************************
*
*                                             function
*
*    �������ƣ�
*
*    �����б���
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
STATIC EFI_STATUS axp22_set_dcdc2(IN CONST AXP_POWER_PROTOCOL *This, int set_vol, int onoff)
{
  UINT8   reg_value;

  if(set_vol > 0)
  {
    if(set_vol < 600)
    {
      set_vol = 600;
    }
    else if(set_vol > 1540)
    {
      set_vol = 1540;
    }
    if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_DC2OUT_VOL, &reg_value))
    {
      return EFI_DEVICE_ERROR;
    }
    reg_value &= ~0x3f;
    reg_value |= (set_vol - 600)/20;
    if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_DC2OUT_VOL, reg_value))
    {
      DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to set dcdc2\n"));
      return EFI_DEVICE_ERROR;
    }
  }

  if(onoff < 0)
  {
    return EFI_SUCCESS;
  }
  if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL1, &reg_value))
  {
    return EFI_DEVICE_ERROR;
  }
  if(onoff == 0)
  {
    reg_value &= ~(1 << 2);
  }
  else
  {
    reg_value |=  (1 << 2);
  }
  if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL1, reg_value))
  {
    DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to onoff dcdc2\n"));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}
/*
************************************************************************************************************
*
*                                             function
*
*    �������ƣ�
*
*    �����б���
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
STATIC EFI_STATUS axp22_set_dcdc3(IN CONST AXP_POWER_PROTOCOL *This, int set_vol, int onoff)
{
  UINT8   reg_value;

  if(set_vol > 0)
  {
    if(set_vol < 600)
    {
      set_vol = 600;
    }
    else if(set_vol > 1860)
    {
      set_vol = 1860;
    }
    if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_DC3OUT_VOL, &reg_value))
    {
      return EFI_DEVICE_ERROR;
    }
    reg_value = ((set_vol - 600)/20);
    if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_DC3OUT_VOL, reg_value))
    {
      DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to set dcdc3\n"));
      return EFI_DEVICE_ERROR;
    }
  }

  if(onoff < 0)
  {
    return EFI_SUCCESS;
  }
  if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL1, &reg_value))
  {
    return EFI_DEVICE_ERROR;
  }
  if(onoff == 0)
  {
    reg_value &= ~(1 << 3);
  }
  else
  {
    reg_value |=  (1 << 3);
  }
  if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL1, reg_value))
  {
    DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to set onoff dcdc3\n"));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}
/*
************************************************************************************************************
*
*                                             function
*
*    �������ƣ�
*
*    �����б���
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
STATIC EFI_STATUS axp22_set_dcdc4(IN CONST AXP_POWER_PROTOCOL *This, int set_vol, int onoff)
{
  UINT8   reg_value;

  if(set_vol > 0)
  {
    if(set_vol < 600)
    {
      set_vol = 600;
    }
    else if(set_vol > 1540)
    {
      set_vol = 1540;
    }
    if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_DC4OUT_VOL, &reg_value))
    {
      return EFI_DEVICE_ERROR;
    }
    reg_value = ((set_vol - 600)/20);
    if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_DC4OUT_VOL, reg_value))
    {
      DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to set dcdc4\n"));
      return EFI_DEVICE_ERROR;
    }
  }

  if(onoff < 0)
  {
    return EFI_SUCCESS;
  }
  if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL1, &reg_value))
  {
    return EFI_DEVICE_ERROR;
  }
  if(onoff == 0)
  {
    reg_value &= ~(1 << 4);
  }
  else
  {
    reg_value |=  (1 << 4);
  }
    if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL1, reg_value))
  {
    DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to set onoff dcdc4\n"));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}
/*
************************************************************************************************************
*
*                                             function
*
*    �������ƣ�
*
*    �����б���
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
STATIC EFI_STATUS axp22_set_dcdc5(IN CONST AXP_POWER_PROTOCOL *This, int set_vol, int onoff)
{
  UINT8   reg_value;

  if(set_vol > 0)
  {
    if(set_vol < 1000)
    {
      set_vol = 1000;
    }
    else if(set_vol > 2550)
    {
      set_vol = 2550;
    }
    if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_DC5OUT_VOL, &reg_value))
    {
      return EFI_DEVICE_ERROR;
    }
    reg_value = ((set_vol - 1000)/50);
    if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_DC5OUT_VOL, reg_value))
    {
      DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to set dcdc5\n"));
      return EFI_DEVICE_ERROR;
    }
  }

  if(onoff < 0)
  {
    return EFI_SUCCESS;
  }
  if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL1, &reg_value))
  {
    return EFI_DEVICE_ERROR;
  }
  if(onoff == 0)
  {
    reg_value &= ~(1 << 5);
  }
  else
  {
    reg_value |=  (1 << 5);
  }
  if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL1, reg_value))
  {
    DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to set onoff dcdc5\n"));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/*
************************************************************************************************************
*
*                                             function
*
*    �������ƣ�
*
*    �����б���
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
STATIC EFI_STATUS axp22_set_aldo1(IN CONST AXP_POWER_PROTOCOL *This, int set_vol, int onoff)
{
  UINT8 reg_value;

  if(set_vol > 0)
  {
    if(set_vol < 700)
    {
      set_vol = 700;
    }
    else if(set_vol > 3300)
    {
      set_vol = 3300;
    }
    if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_ALDO1OUT_VOL, &reg_value))
    {
      return EFI_DEVICE_ERROR;
    }
    reg_value &= 0xE0;
    reg_value |= ((set_vol - 700)/100);
    if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_ALDO1OUT_VOL, reg_value))
    {
      DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to set aldo1\n"));
      return EFI_DEVICE_ERROR;
    }
  }

  if(onoff < 0)
  {
    return EFI_SUCCESS;
  }
  if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL1, &reg_value))
  {
    return EFI_DEVICE_ERROR;
  }
  if(onoff == 0)
  {
    reg_value &= ~(1 << 6);
  }
  else
  {
    reg_value |=  (1 << 6);
  }
  if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL1, reg_value))
  {
    DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to onoff aldo1\n"));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}
/*
************************************************************************************************************
*
*                                             function
*
*    �������ƣ�
*
*    �����б���
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
STATIC EFI_STATUS axp22_set_aldo2(IN CONST AXP_POWER_PROTOCOL *This, int set_vol, int onoff)
{
  UINT8 reg_value;

  if(set_vol > 0)
  {
    if(set_vol < 700)
    {
      set_vol = 700;
    }
    else if(set_vol > 3300)
    {
      set_vol = 3300;
    }
    if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_ALDO2OUT_VOL, &reg_value))
    {
      return EFI_DEVICE_ERROR;
    }
    reg_value &= 0xE0;
    reg_value |= ((set_vol - 700)/100);
    if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_ALDO2OUT_VOL, reg_value))
    {
      DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to set aldo2\n"));
      return EFI_DEVICE_ERROR;
    }
  }

  if(onoff < 0)
  {
    return EFI_SUCCESS;
  }
  if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL1, &reg_value))
  {
    return EFI_DEVICE_ERROR;
  }
  if(onoff == 0)
  {
    reg_value &= ~(1 << 7);
  }
  else
  {
    reg_value |=  (1 << 7);
  }
  if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL1, reg_value))
  {
    DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to onoff aldo2\n"));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}
/*
************************************************************************************************************
*
*                                             function
*
*    �������ƣ�
*
*    �����б���
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
STATIC EFI_STATUS axp22_set_aldo3(IN CONST AXP_POWER_PROTOCOL *This, int set_vol, int onoff)
{
  UINT8 reg_value;

  if(set_vol > 0)
  {
    if(set_vol < 700)
    {
      set_vol = 700;
    }
    else if(set_vol > 3300)
    {
      set_vol = 3300;
    }
    if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_ALDO3OUT_VOL, &reg_value))
    {
      return EFI_DEVICE_ERROR;
    }
    reg_value &= 0xE0;
    reg_value |= ((set_vol - 700)/100);
    if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_ALDO3OUT_VOL, reg_value))
    {
      DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to set aldo3\n"));
      return EFI_DEVICE_ERROR;
    }
  }

  if(onoff < 0)
  {
    return EFI_SUCCESS;
  }
  if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_ALDO_CTL, &reg_value))
  {
    return EFI_DEVICE_ERROR;
  }
  if(onoff == 0)
  {
    reg_value &= ~(1 << 7);
  }
  else
  {
    reg_value |=  (1 << 7);
  }
  if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_ALDO_CTL, reg_value))
  {
    DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to onoff aldo3\n"));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}
/*
************************************************************************************************************
*
*                                             function
*
*    �������ƣ�
*
*    �����б���
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
STATIC EFI_STATUS axp22_set_dldo1(IN CONST AXP_POWER_PROTOCOL *This, int set_vol, int onoff)
{
  UINT8 reg_value;

  if(set_vol > 0)
  {
    if(set_vol < 700)
    {
      set_vol = 700;
    }
    else if(set_vol > 3300)
    {
      set_vol = 3300;
    }
    if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_DLDO1_VOL, &reg_value))
    {
      return EFI_DEVICE_ERROR;
    }
    reg_value &= 0xE0;
    reg_value |= ((set_vol - 700)/100);
    if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_DLDO1_VOL, reg_value))
    {
      DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to set dldo1\n"));
      return EFI_DEVICE_ERROR;
    }
  }

  if(onoff < 0)
  {
    return EFI_SUCCESS;
  }
  if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
  {
    return EFI_DEVICE_ERROR;
  }
  if(onoff == 0)
  {
    reg_value &= ~(1 << 3);
  }
  else
  {
    reg_value |=  (1 << 3);
  }
  if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL2, reg_value))
  {
    DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to onoff dldo1\n"));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}
/*
************************************************************************************************************
*
*                                             function
*
*    �������ƣ�
*
*    �����б���
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
STATIC EFI_STATUS axp22_set_dldo2(IN CONST AXP_POWER_PROTOCOL *This, int set_vol, int onoff)
{
  UINT8 reg_value;

  if(set_vol > 0)
  {
    if(set_vol < 700)
    {
      set_vol = 700;
    }
    else if(set_vol > 3300)
    {
      set_vol = 3300;
    }
    if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_DLDO2_VOL, &reg_value))
    {
      return EFI_DEVICE_ERROR;
    }
    reg_value &= 0xE0;
    reg_value |= ((set_vol - 700)/100);
    if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_DLDO2_VOL, reg_value))
    {
      DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to set dldo2\n"));
      return EFI_DEVICE_ERROR;
    }
  }

  if(onoff < 0)
  {
    return EFI_SUCCESS;
  }
  if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
  {
    return EFI_DEVICE_ERROR;
  }
  if(onoff == 0)
  {
    reg_value &= ~(1 << 4);
  }
  else
  {
    reg_value |=  (1 << 4);
  }
  if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL2, reg_value))
  {
    DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to onoff dldo2\n"));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}
/*
************************************************************************************************************
*
*                                             function
*
*    �������ƣ�
*
*    �����б���
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
STATIC EFI_STATUS axp22_set_dldo3(IN CONST AXP_POWER_PROTOCOL *This, int set_vol, int onoff)
{
  UINT8 reg_value;

  if(set_vol > 0)
  {
    if(set_vol < 700)
    {
      set_vol = 700;
    }
    else if(set_vol > 3300)
    {
      set_vol = 3300;
    }
    if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_DLDO3_VOL, &reg_value))
    {
      return EFI_DEVICE_ERROR;
    }
    reg_value &= 0xE0;
    reg_value |= ((set_vol - 700)/100);
    if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_DLDO3_VOL, reg_value))
    {
      DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to set dldo3\n"));
      return EFI_DEVICE_ERROR;
    }
  }

  if(onoff < 0)
  {
    return EFI_SUCCESS;
  }
  if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
  {
    return EFI_DEVICE_ERROR;
  }
  if(onoff == 0)
  {
    reg_value &= ~(1 << 5);
  }
  else
  {
    reg_value |=  (1 << 5);
  }
  if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL2, reg_value))
  {
    DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to onoff dldo3\n"));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}
/*
************************************************************************************************************
*
*                                             function
*
*    �������ƣ�
*
*    �����б���
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
STATIC EFI_STATUS axp22_set_dldo4(IN CONST AXP_POWER_PROTOCOL *This, int set_vol, int onoff)
{
  UINT8 reg_value;

  if(set_vol > 0)
  {
    if(set_vol < 700)
    {
      set_vol = 700;
    }
    else if(set_vol > 3300)
    {
      set_vol = 3300;
    }
    if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_DLDO4_VOL, &reg_value))
    {
      return EFI_DEVICE_ERROR;
    }
    reg_value &= 0xE0;
    reg_value |= ((set_vol - 700)/100);
    if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_DLDO4_VOL, reg_value))
    {
      DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to set dldo4\n"));
      return EFI_DEVICE_ERROR;
    }
  }

  if(onoff < 0)
  {
    return EFI_SUCCESS;
  }
  if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
  {
    return EFI_DEVICE_ERROR;
  }
  if(onoff == 0)
  {
    reg_value &= ~(1 << 6);
  }
  else
  {
    reg_value |=  (1 << 6);
  }
  if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL2, reg_value))
  {
    DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to onoff dldo4\n"));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}
/*
************************************************************************************************************
*
*                                             function
*
*    �������ƣ�
*
*    �����б���
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
STATIC EFI_STATUS axp22_set_eldo1(IN CONST AXP_POWER_PROTOCOL *This, int set_vol, int onoff)
{
  UINT8 reg_value;

  if(set_vol > 0)
  {
    if(set_vol < 700)
    {
      set_vol = 700;
    }
    else if(set_vol > 3300)
    {
      set_vol = 3300;
    }
    if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_ELDO1_VOL, &reg_value))
    {
      return EFI_DEVICE_ERROR;
    }
    reg_value &= 0xE0;
    reg_value |= ((set_vol - 700)/100);
    if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_ELDO1_VOL, reg_value))
    {
      DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to set eldo1\n"));
      return EFI_DEVICE_ERROR;
    }
  }

  if(onoff < 0)
  {
    return EFI_SUCCESS;
  }
  if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
  {
    return EFI_DEVICE_ERROR;
  }
  if(onoff == 0)
  {
    reg_value &= ~(1 << 0);
  }
  else
  {
    reg_value |=  (1 << 0);
  }
  if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL2, reg_value))
  {
    DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to onoff eldo1\n"));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}
/*
************************************************************************************************************
*
*                                             function
*
*    �������ƣ�
*
*    �����б���
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
STATIC EFI_STATUS axp22_set_eldo2(IN CONST AXP_POWER_PROTOCOL *This, int set_vol, int onoff)
{
  UINT8 reg_value;

  if(set_vol > 0)
  {
    if(set_vol < 700)
    {
      set_vol = 700;
    }
    else if(set_vol > 3300)
    {
      set_vol = 3300;
    }
    if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_ELDO2_VOL, &reg_value))
    {
      return EFI_DEVICE_ERROR;
    }
    reg_value &= 0xE0;
    reg_value |= ((set_vol - 700)/100);
    if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_ELDO2_VOL, reg_value))
    {
      DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to set eldo2\n"));
      return EFI_DEVICE_ERROR;
    }
  }

  if(onoff < 0)
  {
    return EFI_SUCCESS;
  }
  if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
  {
    return EFI_DEVICE_ERROR;
  }
  if(onoff == 0)
  {
    reg_value &= ~(1 << 1);
  }
  else
  {
    reg_value |=  (1 << 1);
  }
  if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL2, reg_value))
  {
    DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to onoff eldo2\n"));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}
/*
************************************************************************************************************
*
*                                             function
*
*    �������ƣ�
*
*    �����б���
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
STATIC EFI_STATUS axp22_set_eldo3(IN CONST AXP_POWER_PROTOCOL *This, int set_vol, int onoff)
{
  UINT8 reg_value;

  if(set_vol > 0)
  {
    if(set_vol < 700)
    {
      set_vol = 700;
    }
    else if(set_vol > 3300)
    {
      set_vol = 3300;
    }
    if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_ELDO3_VOL, &reg_value))
    {
      return EFI_DEVICE_ERROR;
    }
    reg_value &= 0xE0;
    reg_value |= ((set_vol - 700)/100);
    if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_ELDO3_VOL, reg_value))
    {
      DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to set eldo3\n"));
      return EFI_DEVICE_ERROR;
    }
  }

  if(onoff < 0)
  {
    return EFI_SUCCESS;
  }
  if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
  {
    return EFI_DEVICE_ERROR;
  }
  if(onoff == 0)
  {
    reg_value &= ~(1 << 2);
  }
  else
  {
    reg_value |=  (1 << 2);
  }
  if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_OUTPUT_CTL2, reg_value))
  {
    DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to onoff edlo3\n"));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}
/*
************************************************************************************************************
*
*                                             function
*
*    �������ƣ�
*
*    �����б���
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
STATIC EFI_STATUS axp22_set_gpio0ldo(IN CONST AXP_POWER_PROTOCOL *This, int set_vol, int onoff)
{
  UINT8 reg_value;

  if(set_vol > 0)
  {
    if(set_vol < 700)
    {
      set_vol = 700;
    }
    else if(set_vol > 3300)
    {
      set_vol = 3300;
    }
    if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_GPIO0_VOL, &reg_value))
    {
      return EFI_DEVICE_ERROR;
    }
    reg_value &= 0xE0;
    reg_value |= ((set_vol - 700)/100);
    if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_GPIO0_VOL, reg_value))
    {
      DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to set gpio0ldo\n"));
      return EFI_DEVICE_ERROR;
    }
  }

  if(onoff < 0)
  {
    return EFI_SUCCESS;
  }
  if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_GPIO0_CTL, &reg_value))
  {
    return EFI_DEVICE_ERROR;
  }
  if(onoff == 0)
  {
    reg_value &= ~(7 << 0);
    reg_value |=  (3 << 0);
  }
  else
  {
    reg_value &= ~(7 << 0);
    reg_value |=  (2 << 0);
  }
  if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_GPIO0_CTL, reg_value))
  {
    DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to onoff gpio0ldo\n"));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}
/*
************************************************************************************************************
*
*                                             function
*
*    �������ƣ�
*
*    �����б���
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
STATIC EFI_STATUS axp22_set_gpio1ldo(IN CONST AXP_POWER_PROTOCOL *This, int set_vol, int onoff)
{
  UINT8 reg_value;

  if(set_vol > 0)
  {
    if(set_vol < 700)
    {
      set_vol = 700;
    }
    else if(set_vol > 3300)
    {
      set_vol = 3300;
    }
    if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_GPIO1_VOL, &reg_value))
    {
      return EFI_DEVICE_ERROR;
    }
    reg_value &= 0xE0;
    reg_value |= ((set_vol - 700)/100);
    if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_GPIO1_VOL, reg_value))
    {
      DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to set gpio1ldo\n"));
      return EFI_DEVICE_ERROR;
    }
  }

  if(onoff < 0)
  {
    return EFI_SUCCESS;
  }
  if(AxpI2cRead(This, AXP22X_ADDR, BOOT_POWER22_GPIO1_CTL, &reg_value))
  {
    return EFI_DEVICE_ERROR;
  }
  if(onoff == 0)
  {
    reg_value &= ~(7 << 0);
    reg_value |=  (3 << 0);
  }
  else
  {
    reg_value &= ~(7 << 0);
    reg_value |=  (2 << 0);
  }
  if(AxpI2cWrite(This, AXP22X_ADDR, BOOT_POWER22_GPIO1_CTL, reg_value))
  {
    DEBUG((EFI_D_ERROR,"sunxi pmu error : unable to onoff gpio1ldo\n"));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
STATIC EFI_STATUS axp22_set_dcdc_output(IN CONST AXP_POWER_PROTOCOL *This, int sppply_index, int vol_value, int onoff)
{
  switch(sppply_index)
  {
    case 1:
      return axp22_set_dcdc1(This, vol_value, onoff);
    case 2:
      return axp22_set_dcdc2(This, vol_value, onoff);
    case 3:
      return axp22_set_dcdc3(This, vol_value, onoff);
    case 4:
      return axp22_set_dcdc4(This, vol_value, onoff);
    case 5:
      return axp22_set_dcdc5(This, vol_value, onoff);
  }

  return EFI_UNSUPPORTED;
}

STATIC EFI_STATUS axp22_set_aldo_output(IN CONST AXP_POWER_PROTOCOL *This, int sppply_index, int vol_value, int onoff)
{
  switch(sppply_index)
  {
    case 1:
      return axp22_set_aldo1(This, vol_value, onoff);
    case 2:
      return axp22_set_aldo2(This, vol_value, onoff);
    case 3:
      return axp22_set_aldo3(This, vol_value, onoff);
  }

  return EFI_UNSUPPORTED;
}

STATIC EFI_STATUS axp22_set_dldo_output(IN CONST AXP_POWER_PROTOCOL *This, int sppply_index, int vol_value, int onoff)
{
  switch(sppply_index)
  {
    case 1:
      return axp22_set_dldo1(This, vol_value, onoff);
    case 2:
      return axp22_set_dldo2(This, vol_value, onoff);
    case 3:
      return axp22_set_dldo3(This, vol_value, onoff);
    case 4:
      return axp22_set_dldo4(This, vol_value, onoff);
  }

  return EFI_UNSUPPORTED;
}

STATIC EFI_STATUS axp22_set_eldo_output(IN CONST AXP_POWER_PROTOCOL *This, int sppply_index, int vol_value, int onoff)
{
  switch(sppply_index)
  {
    case 1:
      return axp22_set_eldo1(This, vol_value, onoff);
    case 2:
      return axp22_set_eldo2(This, vol_value, onoff);
    case 3:
      return axp22_set_eldo3(This, vol_value, onoff);
  }

  return EFI_UNSUPPORTED;
}

STATIC EFI_STATUS axp22_set_gpioldo_output(IN CONST AXP_POWER_PROTOCOL *This, int sppply_index, int vol_value, int onoff)
{
  switch(sppply_index)
  {
    case 1:
      return axp22_set_gpio0ldo(This, vol_value, onoff);
    case 2:
      return axp22_set_gpio1ldo(This, vol_value, onoff);
  }

  return EFI_UNSUPPORTED;
}

STATIC EFI_STATUS axp22_set_misc_output(IN CONST AXP_POWER_PROTOCOL *This, int sppply_index, int vol_value, int onoff)
{
  switch(sppply_index)
  {
    case PMU_SUPPLY_DC5LDO:
      return axp22_set_dc5ldo(This, onoff);
    case PMU_SUPPLY_DC1SW:
      return axp22_set_dc1sw(This, onoff);
  }

  return EFI_UNSUPPORTED;
}


EFI_STATUS Axp22xSetSupplyStatus(IN CONST AXP_POWER_PROTOCOL *This, IN UINTN VoltageIndex, IN UINTN Voltage, IN INTN OnOff)
{
  int supply_type;
  int sppply_index;

  supply_type  = VoltageIndex & 0xffff0000;
  sppply_index = VoltageIndex & 0x0000ffff;

  switch(supply_type)
  {
    case PMU_SUPPLY_DCDC_TYPE:
      return axp22_set_dcdc_output(This, sppply_index, Voltage, OnOff);

    case PMU_SUPPLY_ALDO_TYPE:
      return axp22_set_aldo_output(This, sppply_index, Voltage, OnOff);

    case PMU_SUPPLY_ELDO_TYPE:
      return axp22_set_eldo_output(This, sppply_index, Voltage, OnOff);

    case PMU_SUPPLY_DLDO_TYPE:
      return axp22_set_dldo_output(This, sppply_index, Voltage, OnOff);

    case PMU_SUPPLY_GPIOLDO_TYPE:
      return axp22_set_gpioldo_output(This, sppply_index, Voltage, OnOff);

    case PMU_SUPPLY_MISC_TYPE:
      return axp22_set_misc_output(This, VoltageIndex, Voltage, OnOff);

    default:
      return EFI_UNSUPPORTED;
  }
}

EFI_STATUS Axp22xSetSupplyStatusByName(IN CONST AXP_POWER_PROTOCOL *This, IN CHAR8 *VoltageName,IN UINTN Voltage, IN INTN OnOff)
{
  int sppply_index;

  if(!AsciiStrnCmp(VoltageName, "dcdc", 4))
  {
    sppply_index = AsciiStrDecimalToUintn(VoltageName + 4);

    return axp22_set_dcdc_output(This, sppply_index, Voltage, OnOff);
  }
  else if(!AsciiStrnCmp(VoltageName, "aldo", 4))
  {
    sppply_index = AsciiStrDecimalToUintn(VoltageName + 4);

    return axp22_set_aldo_output(This, sppply_index, Voltage, OnOff);
  }
  else if(!AsciiStrnCmp(VoltageName, "eldo", 4))
  {
    sppply_index = AsciiStrDecimalToUintn(VoltageName + 4);

    return axp22_set_eldo_output(This, sppply_index, Voltage, OnOff);
  }
  else if(!AsciiStrnCmp(VoltageName, "dldo", 4))
  {
    sppply_index = AsciiStrDecimalToUintn(VoltageName + 4);

    return axp22_set_dldo_output(This, sppply_index, Voltage, OnOff);
  }
  else if(!AsciiStrnCmp(VoltageName, "gpio", 4))
  {
    sppply_index = AsciiStrDecimalToUintn(VoltageName + 4);

    return axp22_set_gpioldo_output(This, sppply_index, Voltage, OnOff);
  }

  return EFI_UNSUPPORTED;
}

EFI_STATUS Axp22xProbeSupplyStatus(IN CONST AXP_POWER_PROTOCOL *This, IN UINTN VoltageIndex,IN UINTN  *Voltage, IN INTN *OnOff)
{
  return EFI_UNSUPPORTED;
}




