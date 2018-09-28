/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : IFsh1.c
**     Project     : TEAM_Robot
**     Processor   : MK22FX512VLK12
**     Component   : IntFLASH
**     Version     : Component 02.409, Driver 01.02, CPU db: 3.00.000
**     Repository  : Kinetis
**     Compiler    : GNU C Compiler
**     Date/Time   : 2018-09-28, 16:04, # CodeGen: 0
**     Abstract    :
**         This component "IntFLASH" implements an access to internal FLASH.
**         The component support reading/writing data into FLASH, erasing of
**         selected sector.
**         The component supports events if the write interrupt is supported.
**         The component supports following modes of write operations:
**           - Write - writing without any test.
**           - Destructive write - sector is erased if necessary.
**           - Safe write - user event is invoked to save and resore data
**                          from the current sector.
**         The component requires on-chip FLASH memory (not used/allocated by
**         other components).
**     Settings    :
**          Component name                                 : IFsh1
**          FLASH                                          : FTFE
**          FLASH_LDD                                      : FLASH_LDD
**          Write method                                   : Safe write (with save & erase)
**            Buffer type                                  : Implemented by the component
**          Interrupt service/event                        : Disabled
**          Wait in RAM                                    : yes
**          Virtual page                                   : Disabled
**          Initialization                                 : 
**            Events enabled in init.                      : yes
**            Wait enabled in init.                        : yes
**          CPU clock/speed selection                      : 
**            FLASH clock                                  : 
**            High speed mode                              : This component enabled
**            Low speed mode                               : This component disabled
**            Slow speed mode                              : This component disabled
**     Contents    :
**         DisableEvent  - byte IFsh1_DisableEvent(void);
**         EnableEvent   - byte IFsh1_EnableEvent(void);
**         SetByteFlash  - byte IFsh1_SetByteFlash(IFsh1_TAddress Addr, byte Data);
**         SetWordFlash  - byte IFsh1_SetWordFlash(IFsh1_TAddress Addr, word Data);
**         SetBlockFlash - byte IFsh1_SetBlockFlash(IFsh1_TDataAddress Source, IFsh1_TAddress Dest, word...
**         GetBlockFlash - byte IFsh1_GetBlockFlash(IFsh1_TAddress Source, IFsh1_TDataAddress Dest, word...
**
**     Copyright : 1997 - 2015 Freescale Semiconductor, Inc. 
**     All Rights Reserved.
**     
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**     
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**     
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**     
**     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**     
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**     
**     http: www.freescale.com
**     mail: support@freescale.com
** ###################################################################*/
/*!
** @file IFsh1.c
** @version 01.02
** @brief
**         This component "IntFLASH" implements an access to internal FLASH.
**         The component support reading/writing data into FLASH, erasing of
**         selected sector.
**         The component supports events if the write interrupt is supported.
**         The component supports following modes of write operations:
**           - Write - writing without any test.
**           - Destructive write - sector is erased if necessary.
**           - Safe write - user event is invoked to save and resore data
**                          from the current sector.
**         The component requires on-chip FLASH memory (not used/allocated by
**         other components).
*/         
/*!
**  @addtogroup IFsh1_module IFsh1 module documentation
**  @{
*/         

/* MODULE IFsh1. */

#include "IFsh1.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Internal method prototypes */

#define IFsh1_TOTAL_FLASH_BLOCK_COUNT  0x02U
#define IFsh1_P_FLASH_SIZE             0x00080000U
#define IFsh1_P_FLASH_BLOCK_COUNT      0x01U
#define IFsh1_P_FLASH_BLOCK_SIZE       0x00080000U
#define IFsh1_D_FLASH_START_ADDRESS    0x10000000U
#define IFsh1_D_FLASH_SIZE             0x00020000U
#define IFsh1_D_FLASH_BLOCK_COUNT      0x01U
#define IFsh1_D_FLASH_BLOCK_SIZE       0x00020000U

#define IFsh1_CMD_NONE                 0x00U
#define IFsh1_CMD_READ                 0x01U
#define IFsh1_CMD_ERASE_SECTOR         0x02U
#define IFsh1_CMD_ERASE_BLOCK          0x03U
#define IFsh1_CMD_VERIFY_ERASED_BLOCK  0x04U
#define IFsh1_CMD_WRITE                0x05U
#define IFsh1_CMD_WRITE_ERASE          0x06U

typedef struct IFsh1_TSector_Struct {
  uint32_t Data[IntFlashLdd1_ERASABLE_UNIT_SIZE/4U];
} IFsh1_TSector;

static uint32_t IFsh1_CurrentCommand;  /* Current command */
static bool IFsh1_CmdResult;           /* Last command result */
static bool IFsh1_CmdPending;          /* Current command state */
static bool IFsh1_EnEvent;             /* State of events (enabled/disabled) */

static LDD_TDeviceData* IntFlashLdd1_DevDataPtr;
static LDD_TData *IFsh1_CurrentDataAddress;
static LDD_FLASH_TDataSize IFsh1_CurrentDataSize;
static LDD_FLASH_TAddress IFsh1_FlashAddress;
static LDD_TData *IFsh1_NextSrcDataAddress;
static LDD_FLASH_TDataSize IFsh1_RemainingDataSize;
static IFsh1_TSector tmpSector;

byte IFsh1_SetFlash(IFsh1_TDataAddress Source, IFsh1_TAddress Dest, word Count);

/*
** ===================================================================
**     Method      :  IFsh1_SetFlash (component IntFLASH)
**
**     Description :
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
byte IFsh1_SetFlash(IFsh1_TDataAddress Src, IFsh1_TAddress Dst, word Count)
{
  LDD_TError                 Result;
  uint32_t                   x;
  uint32_t                   SectorAddress, SectorOffset;

  if (IFsh1_CmdPending) {
    return ERR_BUSY;
  }
  Result = ERR_OK;                     /* Suppose area is erased */
  for (x=0U; x<Count; x++) {           /* Check if written area is erased */
    if (((uint8_t*)Dst)[x] != 0xFFU) { /* Byte erased? */
      Result = ERR_FAILED;             /* No, Erase is required */
     break;
    }
  }
  if (Result == ERR_FAILED) {
    SectorAddress = Dst & ~(LDD_FLASH_TAddress)IntFlashLdd1_ERASABLE_UNIT_MASK;
    SectorOffset = Dst & IntFlashLdd1_ERASABLE_UNIT_MASK;
    tmpSector = *(IFsh1_TSector *)SectorAddress; /* Create copy of the sector */
    do {                               /* Rewrite part of the sector with new data */
      ((uint8_t*)(void*)&tmpSector)[SectorOffset] = *(uint8_t*)(void*)(Src);
      Src++;
      Count--;
    } while ((++SectorOffset != IntFlashLdd1_ERASABLE_UNIT_SIZE) && (Count != 0U));
    IFsh1_CmdPending = TRUE;
    IFsh1_CurrentCommand = IFsh1_CMD_WRITE_ERASE;
    IFsh1_CurrentDataAddress = (LDD_TData *)&tmpSector;
    IFsh1_CurrentDataSize = IntFlashLdd1_ERASABLE_UNIT_SIZE;
    IFsh1_FlashAddress = SectorAddress;
    IFsh1_NextSrcDataAddress = (LDD_TData *)Src;
    IFsh1_RemainingDataSize = (LDD_FLASH_TDataSize) Count;
    Result = IntFlashLdd1_Erase(IntFlashLdd1_DevDataPtr, IFsh1_FlashAddress, IFsh1_CurrentDataSize);
    if (Result == ERR_OK) {
      do {
        IntFlashLdd1_Main(IntFlashLdd1_DevDataPtr);
      } while (IFsh1_CmdPending);
      Result = IFsh1_CmdResult;
    } else {
      IFsh1_CmdPending = FALSE;        /* Command parameter error */
      if (Result == ERR_PARAM_ADDRESS) {
        Result = ERR_RANGE;
      }
    }
    return (byte)Result;
  }
  IFsh1_RemainingDataSize = 0U;
  IFsh1_CurrentCommand = IFsh1_CMD_WRITE;
  IFsh1_CmdPending = TRUE;
  Result = IntFlashLdd1_Write(IntFlashLdd1_DevDataPtr, (LDD_TData *)Src, Dst, (LDD_FLASH_TDataSize)Count); /* Start reading from the flash memory */
  if (Result == ERR_OK) {
    do {
      IntFlashLdd1_Main(IntFlashLdd1_DevDataPtr);
    } while (IFsh1_CmdPending);
    Result = IFsh1_CmdResult;
  } else {
    IFsh1_CmdPending = FALSE;          /* Command parameter error */
    if (Result == ERR_PARAM_ADDRESS) {
      Result = ERR_RANGE;
    }
  }
  return (byte)Result;
}

/*
** ===================================================================
**     Method      :  IFsh1_Init (component IntFLASH)
**
**     Description :
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void IFsh1_Init(void)
{
  IntFlashLdd1_DevDataPtr = IntFlashLdd1_Init(NULL);
  IFsh1_CmdPending = FALSE;
  IFsh1_EnEvent = TRUE;                /* Remember events state */
}

/*
** ===================================================================
**     Method      :  IFsh1_DisableEvent (component IntFLASH)
*/
/*!
**     @brief
**         This method disables all the events except [OnSaveBuffer],
**         [OnRestoreBuffer] and [OnEraseError]. The method is
**         available only if any event is enabled.
**     @return
**                         - Error code, possible codes: 
**                           - ERR_OK - OK 
**                           - ERR_SPEED - This device does not work in
**                           the active speed mode
*/
/* ===================================================================*/
byte IFsh1_DisableEvent(void)
{
  IFsh1_EnEvent = FALSE;
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  IFsh1_EnableEvent (component IntFLASH)
*/
/*!
**     @brief
**         This method enables all the events except [OnSaveBuffer],
**         [OnRestoreBuffer] and [OnEraseError]. The method is
**         available only if any event is enabled.
**     @return
**                         - Error code, possible codes: 
**                           - ERR_OK - OK 
**                           - ERR_SPEED - This device does not work in
**                           the active speed mode
*/
/* ===================================================================*/
byte IFsh1_EnableEvent(void)
{
  IFsh1_EnEvent = TRUE;
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  IFsh1_SetByteFlash (component IntFLASH)
*/
/*!
**     @brief
**         Writes a byte to an address in FLASH. The operation of this
**         method depends on the "Write method" property and state of
**         the flash. Please see more details on general info page of
**         the help.
**     @param
**         Addr            - Address to FLASH.
**     @param
**         Data            - Data to write.
**     @return
**                         - Error code, possible codes: 
**                           - ERR_OK - OK 
**                           - ERR_NOTAVAIL - Desired program/erase
**                           operation is not available 
**                           - ERR_RANGE - Address is out of range 
**                           - ERR_VALUE - Read value is not equal to
**                           written value 
**                           - ERR_SPEED - This device does not work in
**                           the active speed mode 
**                           - ERR_BUSY - Device is busy 
**                           - ERR_PROTECT - Flash is write protect
*/
/* ===================================================================*/
byte IFsh1_SetByteFlash(IFsh1_TAddress Addr, byte Data)
{
  return IFsh1_SetFlash((IFsh1_TDataAddress)(void*)&Data, Addr,1U);
}

/*
** ===================================================================
**     Method      :  IFsh1_SetWordFlash (component IntFLASH)
*/
/*!
**     @brief
**         Writes a word to an address in FLASH. The operation of this
**         method depends on the "Write method" property and state of
**         the flash. Please see more details on general info page of
**         the help.
**     @param
**         Addr            - Address to FLASH.
**     @param
**         Data            - Data to write.
**     @return
**                         - Error code, possible codes: 
**                           - ERR_OK - OK 
**                           - ERR_NOTAVAIL - Desired program/erase
**                           operation is not available 
**                           - ERR_RANGE - Address is out of range 
**                           - ERR_VALUE - Read value is not equal to
**                           written value 
**                           - ERR_SPEED - This device does not work in
**                           the active speed mode 
**                           - ERR_BUSY - Device is busy 
**                           - ERR_PROTECT - Flash is write protect
*/
/* ===================================================================*/
byte IFsh1_SetWordFlash(IFsh1_TAddress Addr, word Data)
{
  return IFsh1_SetFlash((IFsh1_TDataAddress)(void*)&Data, Addr, 2U);
}

/*
** ===================================================================
**     Method      :  IFsh1_SetBlockFlash (component IntFLASH)
*/
/*!
**     @brief
**         Writes data to FLASH. The operation of this method depends
**         on the "Write method" property and state of the flash.
**         Please see more details on general info page of the help.
**     @param
**         Source          - Source address of the data.
**     @param
**         Dest            - Destination address in FLASH.
**     @param
**         Count           - Count of the data fields (in the
**                           smallest addressable units).
**     @return
**                         - Error code, possible codes: 
**                           - ERR_OK - OK 
**                           - ERR_NOTAVAIL - Desired program/erase
**                           operation is not available 
**                           - ERR_RANGE - The address is out of range 
**                           - ERR_BUSY - Device is busy 
**                           - ERR_SPEED - This device does not work in
**                           the active speed mode 
**                           - ERR_PROTECT - Flash is write protect 
**                           - ERR_VALUE - Read value is not equal to
**                           written value
*/
/* ===================================================================*/
byte IFsh1_SetBlockFlash(IFsh1_TDataAddress Source, IFsh1_TAddress Dest, word Count)
{
  return IFsh1_SetFlash(Source, Dest, Count);
}

/*
** ===================================================================
**     Method      :  IFsh1_GetBlockFlash (component IntFLASH)
*/
/*!
**     @brief
**         Reads data from FLASH.
**     @param
**         Source          - Destination address in FLASH.
**     @param
**         Dest            - Source address of the data.
**     @param
**         Count           - Count of the data fields (in the
**                           smallest addressable units).
**     @return
**                         - Error code, possible codes: 
**                           - ERR_OK - OK 
**                           - ERR_NOTAVAIL - Desired program/erase
**                           operation is not available 
**                           - ERR_RANGE - The address is out of range 
**                           - ERR_BUSY - Device is busy
*/
/* ===================================================================*/
byte IFsh1_GetBlockFlash(IFsh1_TAddress Source, IFsh1_TDataAddress Dest, word Count)
{
  LDD_TError                 Result;
  
  if (IFsh1_CmdPending) {
    return ERR_BUSY;
  }
  IFsh1_CurrentCommand = IFsh1_CMD_READ;
  IFsh1_CmdPending = TRUE;
  Result = IntFlashLdd1_Read(IntFlashLdd1_DevDataPtr, (LDD_FLASH_TAddress)Source, (LDD_TData*)Dest, (LDD_FLASH_TDataSize)Count); /* Start reading from the flash memory */
  if (Result == ERR_OK) {              /* Command accepted? */
    do {
      IntFlashLdd1_Main(IntFlashLdd1_DevDataPtr);
    } while (IFsh1_CmdPending);
    Result = IFsh1_CmdResult;
  } else if (Result == ERR_PARAM_ADDRESS) {
    IFsh1_CmdPending = FALSE;
    Result = ERR_RANGE;
  } else {
  }
  return (byte)Result;
}

/*
** ===================================================================
**     Method      :  IFsh1_OnOperationComplete (component IntFLASH)
**
**     Description :
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void IntFlashLdd1_OnOperationComplete(LDD_TUserData *UserDataPtr)
{
  uint32_t  x;
  
  (void)UserDataPtr;                   /* Parameter is not used, suppress unused argument warning */
  IFsh1_CmdResult = ERR_OK;            /* No error appears */
  switch (IFsh1_CurrentCommand) {
    case IFsh1_CMD_WRITE:
      if (IFsh1_RemainingDataSize != 0U) {
        IFsh1_CurrentDataAddress = IFsh1_NextSrcDataAddress;
        IFsh1_FlashAddress += IFsh1_CurrentDataSize;
        if (IFsh1_RemainingDataSize % IntFlashLdd1_ERASABLE_UNIT_SIZE) {
          /* Remaining data size is NOT multiple of sector size */
          if (IFsh1_RemainingDataSize < IntFlashLdd1_ERASABLE_UNIT_SIZE) { /* Last sector? */
            for (x=0U; x<IntFlashLdd1_ERASABLE_UNIT_SIZE;x++) {
              if (x<IFsh1_RemainingDataSize) {
                ((uint8_t*)(void*)&tmpSector)[x] = ((uint8_t*)(void*)IFsh1_NextSrcDataAddress)[x];
              } else {
                ((uint8_t*)(void*)&tmpSector)[x] = ((uint8_t*)(void*)IFsh1_FlashAddress)[x];
              }
            }
            IFsh1_CurrentDataAddress = &tmpSector;
            IFsh1_CurrentDataSize = IntFlashLdd1_ERASABLE_UNIT_SIZE;
            IFsh1_RemainingDataSize = 0U;
          } else {
            IFsh1_CurrentDataSize = (IFsh1_RemainingDataSize / IntFlashLdd1_ERASABLE_UNIT_SIZE)*IntFlashLdd1_ERASABLE_UNIT_SIZE;
            IFsh1_RemainingDataSize = IFsh1_RemainingDataSize % IntFlashLdd1_ERASABLE_UNIT_SIZE;
            IFsh1_NextSrcDataAddress = (IFsh1_TDataAddress)((uint32_t)IFsh1_NextSrcDataAddress + IFsh1_CurrentDataSize);
          }
        } else {
          /* Remaining data size is multiple of sector size */
          IFsh1_CurrentDataSize = IFsh1_RemainingDataSize;
          IFsh1_RemainingDataSize = 0U;
        }
        IFsh1_CurrentCommand = IFsh1_CMD_WRITE_ERASE;
        IFsh1_CmdResult = (byte)IntFlashLdd1_Erase(IntFlashLdd1_DevDataPtr, IFsh1_FlashAddress, IFsh1_CurrentDataSize);
        if (IFsh1_CmdResult != ERR_OK) { /* Erase command error? */
          IFsh1_CmdPending = FALSE;    /* Command is finished */
        }
        return;
      } else {
        IFsh1_CmdPending = FALSE;      /* Command is finished */
      }
      break;
    case IFsh1_CMD_WRITE_ERASE:
      IFsh1_CurrentCommand = IFsh1_CMD_WRITE;
      IFsh1_CmdResult = (byte)IntFlashLdd1_Write(IntFlashLdd1_DevDataPtr, IFsh1_CurrentDataAddress, IFsh1_FlashAddress, IFsh1_CurrentDataSize); /* Write data */
      if (IFsh1_CmdResult != ERR_OK) { /* Write command error? */
        IFsh1_CmdPending = FALSE;      /* Command is finished */
      }
      return;
    default:
      IFsh1_CmdPending = FALSE;        /* Command is finished */
      return;
  }
}

/*
** ===================================================================
**     Method      :  IFsh1_OnError (component IntFLASH)
**
**     Description :
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void IntFlashLdd1_OnError(LDD_TUserData *UserDataPtr)
{
  LDD_FLASH_TErrorStatus FLASH_LDD_ErrorStatus;

  (void)UserDataPtr;                   /* Parameter is not used, suppress unused argument warning */
  IFsh1_CmdPending = FALSE;
  IFsh1_CurrentCommand = IFsh1_CMD_NONE;
  IntFlashLdd1_GetError(IntFlashLdd1_DevDataPtr, &FLASH_LDD_ErrorStatus);
  if (FLASH_LDD_ErrorStatus.CurrentErrorFlags & LDD_FLASH_PROTECTION_VIOLATION) {
    IFsh1_CmdResult = ERR_PROTECT;     /* Protection violation */
  } else {
    IFsh1_CmdResult = ERR_VALUE;       /* Access error or Read collision error */
  }
}

/* END IFsh1. */

#ifdef __cplusplus
}  /* extern "C" */
#endif

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
