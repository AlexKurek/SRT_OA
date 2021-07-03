#include "encoderRegisters.h"

 const uint16_t encoderRegPositionH                      = 0x0001;//0x9C42;
 const uint16_t encoderRegPositionL                      = 0x0002;//0x9C43;
 const uint16_t encoderRegActualReverseState             = 0x0003;//0x9C44;
 const uint16_t encoderRegTermResetState                 = 0x0004;//0x9C45;
 const uint16_t encoderRegSpeedH                         = 0x0005;//0x9C46;
 const uint16_t encoderRegSpeedL                         = 0x0006;//0x9C47;
 const uint16_t encoderRegLimitSwitchState               = 0x0007;//0x9C48;
 const uint16_t encoderRegPhysicalSTResolutionH          = 0x000C;//0x9C4D;
 const uint16_t encoderRegPhysicalSTResolutionL          = 0x000D;//0x9C4E;
 const uint16_t encoderRegPhysicalMTResolutionH          = 0x000E;//0x9C4F;
 const uint16_t encoderRegPhysicalMTResolutionL          = 0x000F;//0x9C50;
 const uint16_t encoderRegScalingEnabled                 = 0x0010;//0x9C51;
 const uint16_t encoderRegSTResolutionH                  = 0x0011;//0x9C52;
 const uint16_t encoderRegSTResolutionL                  = 0x0012;//0x9C53;
 const uint16_t encoderRegTotResolutionH                 = 0x0013;//0x9C54;
 const uint16_t encoderRegTotResolutionL                 = 0x0014;//0x9C55;
 const uint16_t encoderRegPresetH                        = 0x0015;//0x9C56;
 const uint16_t encoderRegPresetL                        = 0x0016;//0x9C57;
 const uint16_t encoderRegOffsetH                        = 0x0017;//0x9C58;
 const uint16_t encoderRegOffsetL                        = 0x0018;//0x9C59;
 const uint16_t encoderRegCountDirection                 = 0x0019;//0x9C5A;
 const uint16_t encoderRegSpeedMode                      = 0x001A;//0x9C5B;
 const uint16_t encoderRegSpeedFilter                    = 0x001B;//0x9C5C;
 const uint16_t encoderRegLimitSwitchEnable              = 0x001C;//0x9C5D;
 const uint16_t encoderRegLowLimitSwitchH                = 0x001D;//0x9C5E;
 const uint16_t encoderRegLowLimitSwitchL                = 0x001E;//0x9C5F;
 const uint16_t encoderRegHighLimitSwitchH               = 0x001F;//0x9C60;
 const uint16_t encoderRegHighLimitSwitchL               = 0x0020;//0x9C61;
 const uint16_t encoderRegDelay                          = 0x0021;//0x9C62;
 const uint16_t encoderRegErrorReg                       = 0x0022;//0x9C63;
 const uint16_t encoderRegDeviceResetStore               = 0x0023;//0x9C63;
 const uint16_t encoderRegParameters                     = 0x0024;//0x9C64;
 const uint16_t encoderRegAutoStore                      = 0x0025;//0x9C65;
 const uint16_t encoderRegRestoreAllParameters           = 0x0026;//0x9C66;
 const uint16_t encoderRegRestoreAplicationParameters    = 0x0027;//0x9C67;
 const uint16_t encoderRegAutoTest                       = 0x0028;//0x9C68;
 const uint16_t encoderRegSoftwareVersion                = 0x0029;//0x9C69;
 const uint16_t encoderRegSerialNumberH                  = 0x002A;//0x9C70;
 const uint16_t encoderRegSerialNumberL                  = 0x002B;//0x9C71;
 const uint16_t encoderRegLifeCycleCounterH              = 0x0031;//0x9C72;
 const uint16_t encoderRegLifeCycleCounterL              = 0x0032;//0x9C73;
 const uint16_t encoderRegRollCounter                    = 0x0033;//0x9C74;
 const uint16_t encoderRegBaudrate                       = 0x0100;//0x9D41;
 const uint16_t encoderRegNumberData                     = 0x0101;//0x9D42;
 const uint16_t encoderRegParity                         = 0x0102;//0x9D43;
 const uint16_t encoderRegStopbits                       = 0x0103;//0x9D44;
 const uint16_t encoderRegCommUpdate                     = 0x0104;//0x9D45;
 const uint16_t encoderRegNodeAddress                    = 0x0105;//0x9D46;
 const uint16_t encoderRegNodeUpdate                     = 0x0106;//0x9D47;
 const uint16_t encoderRegAutoBaudEnable                 = 0x0107;//0x9D48;
 const uint16_t encoderRegAutoBaudTimeout                = 0x0108;//0x9D49;
 const uint16_t encoderRegRestoreBusParameters           = 0x0109;//0x9D4A;
 const uint16_t encoderRegTermination                    = 0x010C;//0x9D4D;
 const uint16_t encoderRegTermUpdate                     = 0x010D;//0x9D4E;