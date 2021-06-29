/*
* @file encoder.c
* @author Bartosz Śmierciak
* @Date 2021-05-31
*
* @brief A brief description of encoder.h.
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <modbus/modbus.h>
#include "encoder.h"
#include "encoderRegisters.h"

//modbus contex
modbus_t *ctx;


struct encoder 
{
        uint32_t PositionH;
        uint32_t PositionL;
        uint32_t Position;
        double   STAngle;
        uint32_t ActualReverseState;
        uint32_t TermResetState;
        uint32_t SpeedH;
        uint32_t SpeedL;
        uint32_t Speed;
        uint32_t LimitSwitchState;
        uint32_t PhysicalSTResolutionH;
        uint32_t PhysicalSTResolutionL;
        uint32_t PhysicalSTResolution;
        uint32_t PhysicalMTResolutionH;
        uint32_t PhysicalMTResolutionL;
        uint32_t PhysicalMTResolution;
        uint32_t ScalingEnabled;
        uint32_t STResolutionH;
        uint32_t STResolutionL;
        uint32_t STResolution;
        uint32_t TotResolutionH;
        uint32_t TotResolutionL;
        uint32_t TotResolution;
        uint32_t PresetH;
        uint32_t PresetL;
        uint32_t Preset;
        uint32_t OffsetH;
        uint32_t OffsetL;
        uint32_t Offset;
        uint32_t CountDirection;
        uint32_t SpeedMode;
        uint32_t SpeedFilter;
        uint32_t LimitSwitchEnable;
        uint32_t LowLimitSwitchH;
        uint32_t LowLimitSwitchL;
        uint32_t LowLimitSwitch;
        uint32_t HighLimitSwitchH;
        uint32_t HighLimitSwitchL;
        uint32_t HighLimitSwitch;
        uint32_t Delay;
        uint32_t ErrorReg;
        uint32_t DeviceResetStore;
        uint32_t Parameters;
        uint32_t AutoStore;
        uint32_t RestoreAllParameters;
        uint32_t RestoreAplicationParameters;
        uint32_t AutoTest;
        uint32_t SoftwareVersion;
        uint32_t SerialNumberH;
        uint32_t SerialNumberL;
        uint32_t SerialNumber;
        uint32_t LifeCycleCounterH;
        uint32_t LifeCycleCounterL;
        uint32_t LifeCycleCounter;
        uint32_t RollCounter;
        uint32_t Baudrate;
        uint32_t NumberData;
        uint32_t Parity;
        uint32_t Stopbits;
        uint32_t CommUpdate;
        uint32_t NodeAddress;
        uint32_t NodeUpdate;
        uint32_t AutoBaudEnable;
        uint32_t AutoBaudTimeout;
        uint32_t RestoreBusParameters;
        uint32_t Termination;
        uint32_t TermUpdate;
}_encoder[128];

int EncoderInit(const char *device, int baud, char parity, int data_bit, int stop_bit)
{

    uint32_t tv_sec  = 0;
    uint32_t tv_usec = 0;

    uint32_t resTimeSec = 0;
    uint32_t resTimeuSec = 600000;

    printf("\n");
    printf("Trying to connect...");
    ctx = modbus_new_rtu(device, baud, parity, data_bit, stop_bit);
    if (modbus_connect(ctx) == -1)
    {
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        modbus_close(ctx);
        modbus_free(ctx);
        return -1;
    }
    if (NULL == ctx)
    {
        printf("Unable to create modbus context\n");
        modbus_close(ctx);
        modbus_free(ctx);
        return -1;
    }
    printf("OK.\nCreated modbus context\n");
    /* Get response timeout */
    modbus_get_response_timeout(ctx, &tv_sec, &tv_usec); 
    printf("Default response timeout: %d sec %d usec \n", tv_sec, tv_usec );

    /* Set response timeout */
    modbus_set_response_timeout(ctx, resTimeSec, resTimeuSec); 
    modbus_get_response_timeout(ctx, &tv_sec, &tv_usec); 
    printf("Set response timeout:     %d sec %d usec \n", tv_sec, tv_usec );
    return 0;
}

uint16_t EncoderReadModbus(int slaveAddress, int regAddress)
{
    //Set the Modbus address of the remote slave
    int rc = modbus_set_slave(ctx, slaveAddress);
    if (rc == -1) 
    {
        fprintf(stderr, "Invalid slave ID: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

	// will store read register value
    uint16_t reg;

    int num = modbus_read_registers(ctx, regAddress, 1, &reg);
    if (num != 1)
    {
        fprintf(stderr, "Failed to read modbus. Num: %d, error: %s\n", num, modbus_strerror(errno));
    }
    return reg;
}

int EncoderWriteModbus(int slaveAddress, int regAddress, uint16_t value)
{

	modbus_set_slave(ctx, slaveAddress);
    int modbusAnswear = modbus_write_register(ctx, regAddress, value);
    if (modbusAnswear != 1)
   {
        printf("ERROR modbus_write_register (%d)\n", modbusAnswear);
        printf("Address = %d, value = %d (0x%X)\n", regAddress, value, value);
    }
    return modbusAnswear;
}

void EncoderClose(void)
{
    modbus_close(ctx);
    modbus_free(ctx);
}
void EncoderPrintRegisters(int slaveAddress)
{
	printf("Position: %u steps\n",EncoderGetPosition(slaveAddress));
	printf("Actual Reverse State: ");
	switch(EncoderGetActualReverseState(slaveAddress))
	{
		case 0: printf("CW\n"); break;
		case 1: printf("CCW\n"); break;
		default: printf("error\n");
	}

	printf("Term Reset State: ");
	switch(EncoderGetTermResetState(slaveAddress))
	{
		case 0: printf("off\n"); break;
		case 1: printf("on\n"); break;
		default: printf("error\n");
	}

	printf("Speed: %u\n", EncoderGetSpeed(slaveAddress));

	printf("Limit Switch State: %u\n", EncoderGetLimitSwitchState(slaveAddress));

	printf("Physical Single Turn Resolution: %u\n", EncoderGetPhysicalSTResolution(slaveAddress));

	printf("Physical Multi Turn Resolution: %u\n", EncoderGetPhysicalMTResolution(slaveAddress));

	printf("Scaling Enabled: ");
	switch(EncoderGetScalingEnabled(slaveAddress))
	{
		case 0: printf("off\n"); break;
		case 1: printf("on\n"); break;
		default: printf("error\n");
	}

	printf("Single Turn Resolution: %u\n", EncoderGetSTResolution(slaveAddress));

	printf("Total resolution: %u\n",EncoderGetTotResolution(slaveAddress));

	printf("Preset: %u\n", EncoderGetPreset(slaveAddress));

	printf("Offset: %u\n", EncoderGetOffset(slaveAddress));

	printf("Count direction: %u\n",EncoderGetCountDirection(slaveAddress));

	printf("Speed mode: ");
	switch(EncoderGetSpeedMode(slaveAddress))
	{
		case 0: printf("disable\n"); break;
		case 1: printf("rmp\n"); break;
		case 2: printf("degree/sec\n"); break;
		case 3: printf("steps/s\n"); break;
		case 4: printf("rad/sec\n"); break;
		case 5: printf("Hz\n"); break;
		default: printf("error\n");
	}

	printf("Speed filter. Moving average: %u\n",EncoderGetSpeedFilter(slaveAddress));

	printf("Limit Switch Enable: %u\n",EncoderGetLimitSwitchEnable(slaveAddress));

	printf("Low Limit Switch: %u\n",EncoderGetLowLimitSwitch(slaveAddress));

	printf("High Limit Switch: %u\n",EncoderGetHighLimitSwitch(slaveAddress));

	printf("Delay: %u\n",EncoderGetDelay(slaveAddress));

	printf("Error Register: %u\n",EncoderGetErrorReg(slaveAddress));

	printf("Device Reset Store: %u\n",EncoderGetDeviceResetStore(slaveAddress));

	printf("Parameters: %u\n",EncoderGetParameters(slaveAddress));

	printf("Auto Store: %u\n",EncoderGetAutoStore(slaveAddress));

	printf("Restore All Parameters: %u\n",EncoderGetRestoreAllParameters(slaveAddress));

	printf("Restore Application Parameters: %u\n",EncoderGetRestoreAplicationParameters(slaveAddress));

	printf("Auto Test: %u\n",EncoderGetAutoTest(slaveAddress));

	printf("Software Version: %u\n",EncoderGetSoftwareVersion(slaveAddress));

	printf("Serial Number: %u\n",EncoderGetSerialNumber(slaveAddress));

	printf("Life Cycle Counter %u\n",EncoderGetLifeCycleCounter(slaveAddress));

	printf("Roll Counter: %u\n",EncoderGetRollCounter(slaveAddress));

	printf("Baudrate: ");
	switch(EncoderGetBaudrate(slaveAddress))
	{
		case 0: printf("1200"); break;
		case 1: printf("2400"); break;
		case 2: printf("4800"); break;
		case 3: printf("9600"); break;
		case 4: printf("14400"); break;
		case 5: printf("19200"); break;
		case 6: printf("38400"); break;
		case 7: printf("56000"); break;
		case 8: printf("57600"); break;
		case 9: printf("115200"); break;
		case 10: printf("128000"); break;
		case 11: printf("256000"); break;
		default: printf("error read");
	}
	printf(" bps\n");

	printf("Number data bits: ");
	switch(EncoderGetNumberData(slaveAddress))
	{
		case 0: printf("6\n"); break;
		case 1: printf("7\n"); break;
		case 2: printf("8\n"); break;
		default: printf("error\n");
	}
	printf("Parity: ");
	switch(EncoderParity(slaveAddress))
	{
		case 0: printf("No\n"); break;
		case 1: printf("Odd\n"); break;
		case 2: printf("Even\n"); break;
		default: printf("error\n");
	}

	printf("Stop bits: %u\n",EncoderGetStopbits(slaveAddress));

	printf("Comm Update Execute: %u\n",EncoderGetCommUpdate(slaveAddress));

	printf("Node Address: %u\n",EncoderGetNodeAddress(slaveAddress));

	printf("Node Update: %u\n",EncoderGetNodeUpdate(slaveAddress));

	printf("Auto Baud Enable: %u\n",EncoderGetAutoBaudEnable(slaveAddress));

	printf("Auto Baud Timeout: %u\n",EncoderGetAutoBaudTimeout(slaveAddress));

	printf("Restore Bus Parameters: %u\n",EncoderGetRestoreBusParameters(slaveAddress));

	printf("Termination: %u\n",EncoderGetTermination(slaveAddress));

	printf("Term update: %u\n",EncoderGetTermUpdate(slaveAddress));

}
uint32_t EncoderGetPosition(int slaveAddress)
{
    _encoder[slaveAddress].PositionH = EncoderReadModbus(slaveAddress, encoderRegPositionH);
    _encoder[slaveAddress].PositionL = EncoderReadModbus(slaveAddress, encoderRegPositionL);
    _encoder[slaveAddress].Position = (_encoder[slaveAddress].PositionH ) << 16 | _encoder[slaveAddress].PositionL;
    return _encoder[slaveAddress].Position;
}

uint32_t EncoderGetPositionH(int slaveAddress)
{
    _encoder[slaveAddress].PositionH = EncoderReadModbus(slaveAddress, encoderRegPositionH);
    _encoder[slaveAddress].PositionH = (_encoder[slaveAddress].PositionH ) << 16;
    return _encoder[slaveAddress].PositionH;
}

uint32_t EncoderGetPositionL(int slaveAddress)
{
    _encoder[slaveAddress].PositionL = EncoderReadModbus(slaveAddress, encoderRegPositionL);
    return _encoder[slaveAddress].PositionL;
}

angle EncoderGetSTAngle(int slaveAddress)
{
    _encoder[slaveAddress].PositionH = EncoderReadModbus(slaveAddress, encoderRegPositionH);
    _encoder[slaveAddress].PositionL = EncoderReadModbus(slaveAddress, encoderRegPositionL);
    _encoder[slaveAddress].Position = (_encoder[slaveAddress].PositionH ) << 16 | _encoder[slaveAddress].PositionL;

    _encoder[slaveAddress].STResolutionH = EncoderReadModbus(slaveAddress, encoderRegSTResolutionH);
    _encoder[slaveAddress].STResolutionL = EncoderReadModbus(slaveAddress, encoderRegSTResolutionL);
    _encoder[slaveAddress].STResolution = (_encoder[slaveAddress].STResolutionH) << 16 | _encoder[slaveAddress].STResolutionL;

    _encoder[slaveAddress].TotResolutionH = EncoderReadModbus(slaveAddress, encoderRegTotResolutionH);
    _encoder[slaveAddress].TotResolutionL = EncoderReadModbus(slaveAddress, encoderRegTotResolutionL);
    _encoder[slaveAddress].TotResolution = (_encoder[slaveAddress].TotResolutionH) << 16 | _encoder[slaveAddress].TotResolutionL;

    _encoder[slaveAddress].STAngle = (((double)(_encoder[slaveAddress].Position % _encoder[slaveAddress].STResolution) / _encoder[slaveAddress].STResolution))*360.0;
    angle angle;
    angle.deg = _encoder[slaveAddress].STAngle;
    angle.d = (int)angle.deg;
    angle.degm = (angle.deg - angle.d) * 60.0;
    angle.dm = (int)angle.degm;
    angle.degs = (angle.degm - angle.dm) * 60.0;
    angle.ds = (int)angle.degs;
    angle.hour = angle.deg /15.0;
    angle.h = (int)angle.hour;
    angle.hourm = (angle.hour - angle.h) * 60.0;
    angle.hm = (int)angle.hourm;
    angle.hours = (angle.hourm - angle.hm) * 60.0;
    angle.hs = (int)angle.hours;
    return angle;
}

uint32_t EncoderGetAngleMT(int slaveAddress)
{
    _encoder[slaveAddress].PositionH = EncoderReadModbus(slaveAddress, encoderRegPositionH);
    _encoder[slaveAddress].PositionL = EncoderReadModbus(slaveAddress, encoderRegPositionL);
    _encoder[slaveAddress].Position = (_encoder[slaveAddress].PositionH ) << 16 | _encoder[slaveAddress].PositionL;
    return _encoder[slaveAddress].Position;
}

uint32_t EncoderGetActualReverseState(int slaveAddress)
{
    _encoder[slaveAddress].ActualReverseState = EncoderReadModbus(slaveAddress, encoderRegActualReverseState);
    return _encoder[slaveAddress].ActualReverseState;
}

uint32_t EncoderGetTermResetState(int slaveAddress)
{
    _encoder[slaveAddress].TermResetState = EncoderReadModbus(slaveAddress, encoderRegTermResetState);
    return _encoder[slaveAddress].TermResetState;
}

uint32_t EncoderGetSpeed(int slaveAddress)
{
    _encoder[slaveAddress].SpeedH = EncoderReadModbus(slaveAddress, encoderRegSpeedH);
    _encoder[slaveAddress].SpeedL = EncoderReadModbus(slaveAddress, encoderRegSpeedL);
    _encoder[slaveAddress].Speed = (_encoder[slaveAddress].SpeedH) << 16 | _encoder[slaveAddress].SpeedL;
    return _encoder[slaveAddress].Speed;
}

uint32_t EncoderGetLimitSwitchState(int slaveAddress)
{
    _encoder[slaveAddress].LimitSwitchState = EncoderReadModbus(slaveAddress, encoderRegLimitSwitchState);
    return _encoder[slaveAddress].LimitSwitchState;
}

uint32_t EncoderGetPhysicalSTResolution(int slaveAddress)
{
    _encoder[slaveAddress].PhysicalSTResolutionH = EncoderReadModbus(slaveAddress, encoderRegPhysicalSTResolutionH);
    _encoder[slaveAddress].PhysicalSTResolutionL = EncoderReadModbus(slaveAddress, encoderRegPhysicalSTResolutionL);
    _encoder[slaveAddress].PhysicalSTResolution = (_encoder[slaveAddress].PhysicalSTResolutionH) << 16 | _encoder[slaveAddress].PhysicalSTResolutionL;
    return _encoder[slaveAddress].PhysicalSTResolution;
}

uint32_t EncoderGetPhysicalMTResolution(int slaveAddress)
{
    _encoder[slaveAddress].PhysicalMTResolutionH = EncoderReadModbus(slaveAddress, encoderRegPhysicalMTResolutionH);
    _encoder[slaveAddress].PhysicalMTResolutionL = EncoderReadModbus(slaveAddress, encoderRegPhysicalMTResolutionL);
    _encoder[slaveAddress].PhysicalMTResolution = (_encoder[slaveAddress].PhysicalMTResolutionH) << 16 | _encoder[slaveAddress].PhysicalMTResolutionL;
    return _encoder[slaveAddress].PhysicalMTResolution;
}

uint32_t EncoderGetScalingEnabled(int slaveAddress)
{
    _encoder[slaveAddress].ScalingEnabled = EncoderReadModbus(slaveAddress, encoderRegScalingEnabled);
    return _encoder[slaveAddress].ScalingEnabled;
}

void EncoderSetScalingEnabled(int slaveAddress, uint32_t scalingEnabled)
{
    _encoder[slaveAddress].ScalingEnabled = scalingEnabled;
    EncoderWriteModbus(slaveAddress, encoderRegScalingEnabled, _encoder[slaveAddress].ScalingEnabled);
}

uint32_t EncoderGetSTResolution(int slaveAddress)
{
    _encoder[slaveAddress].STResolutionH = EncoderReadModbus(slaveAddress, encoderRegSTResolutionH);
    _encoder[slaveAddress].STResolutionL = EncoderReadModbus(slaveAddress, encoderRegSTResolutionL);
    _encoder[slaveAddress].STResolution = (_encoder[slaveAddress].STResolutionH) << 16 | _encoder[slaveAddress].STResolutionL;
    return _encoder[slaveAddress].STResolution;
}

uint32_t EncoderGetTotResolution(int slaveAddress)
{
    _encoder[slaveAddress].TotResolutionH = EncoderReadModbus(slaveAddress, encoderRegTotResolutionH);
    _encoder[slaveAddress].TotResolutionL = EncoderReadModbus(slaveAddress, encoderRegTotResolutionL);
    _encoder[slaveAddress].TotResolution = (_encoder[slaveAddress].TotResolutionH) << 16 | _encoder[slaveAddress].TotResolutionL;
    return _encoder[slaveAddress].TotResolution;
}

void EncoderSetTotResolution(int slaveAddress, uint32_t totResolution)
{
    _encoder[slaveAddress].TotResolution = totResolution;
    EncoderWriteModbus(slaveAddress, encoderRegTotResolutionL, _encoder[slaveAddress].TotResolution | 0x00FF);
    EncoderWriteModbus(slaveAddress, encoderRegTotResolutionH, (_encoder[slaveAddress].TotResolution | 0xFF00) >> 16);
}

uint32_t EncoderGetPreset(int slaveAddress)
{
    _encoder[slaveAddress].PresetH = EncoderReadModbus(slaveAddress, encoderRegPresetH);
    _encoder[slaveAddress].PresetL = EncoderReadModbus(slaveAddress, encoderRegPresetL);
    _encoder[slaveAddress].Preset = (_encoder[slaveAddress].PresetH) << 16 | _encoder[slaveAddress].PresetL;
    return _encoder[slaveAddress].Preset;
}

void EncoderSetPreset(int slaveAddress, uint32_t preset)
{
    _encoder[slaveAddress].Preset = preset;
    EncoderWriteModbus(slaveAddress, encoderRegPresetL, _encoder[slaveAddress].Preset | 0x00FF);
    EncoderWriteModbus(slaveAddress, encoderRegPresetH, (_encoder[slaveAddress].Preset | 0xFF00) >> 16);
    
}

uint32_t EncoderGetOffset(int slaveAddress)
{
    _encoder[slaveAddress].OffsetH = EncoderReadModbus(slaveAddress, encoderRegOffsetH);
    _encoder[slaveAddress].OffsetL = EncoderReadModbus(slaveAddress, encoderRegOffsetL);
    _encoder[slaveAddress].Offset = (_encoder[slaveAddress].OffsetH) << 16 | _encoder[slaveAddress].OffsetL;
    return _encoder[slaveAddress].Offset;
}

uint32_t EncoderGetCountDirection(int slaveAddress)
{
    _encoder[slaveAddress].CountDirection = EncoderReadModbus(slaveAddress, encoderRegCountDirection);
    return _encoder[slaveAddress].CountDirection;
}

void EncoderSetCountDirection(int slaveAddress, uint32_t countDirection)
{
    _encoder[slaveAddress].CountDirection = countDirection;
    EncoderWriteModbus(slaveAddress, encoderRegCountDirection, _encoder[slaveAddress].CountDirection);
}

uint32_t EncoderGetSpeedMode(int slaveAddress)
{
    _encoder[slaveAddress].SpeedMode = EncoderReadModbus(slaveAddress, encoderRegSpeedMode);
    return _encoder[slaveAddress].SpeedMode;
}

void EncoderSetSpeedMode(int slaveAddress, uint32_t speedMode)
{
    _encoder[slaveAddress].SpeedMode = speedMode;
    EncoderWriteModbus(slaveAddress, encoderRegSpeedMode, _encoder[slaveAddress].SpeedMode);
}

uint32_t EncoderGetSpeedFilter(int slaveAddress)
{
    _encoder[slaveAddress].SpeedFilter = EncoderReadModbus(slaveAddress, encoderRegSpeedFilter);
    return _encoder[slaveAddress].SpeedFilter;
}

void EncoderSetSpeedFilter(int slaveAddress, uint32_t speedFilter)
{
    _encoder[slaveAddress].SpeedFilter = speedFilter;
    EncoderWriteModbus(slaveAddress, encoderRegSpeedFilter, _encoder[slaveAddress].SpeedFilter);
}

uint32_t EncoderGetLimitSwitchEnable(int slaveAddress)
{
    _encoder[slaveAddress].LimitSwitchEnable = EncoderReadModbus(slaveAddress, encoderRegLimitSwitchEnable);
    return _encoder[slaveAddress].LimitSwitchEnable;
}

void EncoderSetLimitSwitchEnable(int slaveAddress, uint32_t limitSwitchEnable)
{
    _encoder[slaveAddress].LimitSwitchEnable = limitSwitchEnable;
    EncoderWriteModbus(slaveAddress, encoderRegLimitSwitchEnable, _encoder[slaveAddress].LimitSwitchEnable);
}

uint32_t EncoderGetLowLimitSwitch(int slaveAddress)
{
    _encoder[slaveAddress].LowLimitSwitchH = EncoderReadModbus(slaveAddress, encoderRegLowLimitSwitchH);
    _encoder[slaveAddress].LowLimitSwitchL = EncoderReadModbus(slaveAddress, encoderRegLowLimitSwitchL);
    _encoder[slaveAddress].LowLimitSwitch = (_encoder[slaveAddress].LowLimitSwitchH) << 16 | _encoder[slaveAddress].LowLimitSwitchL;
    return _encoder[slaveAddress].LowLimitSwitch;
}

void EncoderSetLowLimitSwitch(int slaveAddress, uint32_t lowLimitSwitch)
{
    _encoder[slaveAddress].LowLimitSwitch = lowLimitSwitch;
    EncoderWriteModbus(slaveAddress, encoderRegLowLimitSwitchL, _encoder[slaveAddress].LowLimitSwitch | 0x00FF);
    EncoderWriteModbus(slaveAddress, encoderRegLowLimitSwitchH, (_encoder[slaveAddress].LowLimitSwitch | 0xFF00) >> 16);
}

uint32_t EncoderGetHighLimitSwitch(int slaveAddress)
{
    _encoder[slaveAddress].HighLimitSwitchH = EncoderReadModbus(slaveAddress, encoderRegHighLimitSwitchH);
    _encoder[slaveAddress].HighLimitSwitchL = EncoderReadModbus(slaveAddress, encoderRegHighLimitSwitchL);
    _encoder[slaveAddress].HighLimitSwitch = (_encoder[slaveAddress].HighLimitSwitchH) << 16 | _encoder[slaveAddress].HighLimitSwitchL;
    return _encoder[slaveAddress].HighLimitSwitch;
}

void EncoderSetHighLimitSwitch(int slaveAddress, uint32_t highLimitSwitch)
{
    _encoder[slaveAddress].HighLimitSwitch = highLimitSwitch;
    EncoderWriteModbus(slaveAddress, encoderRegHighLimitSwitchL, _encoder[slaveAddress].HighLimitSwitch | 0x00FF);
    EncoderWriteModbus(slaveAddress, encoderRegHighLimitSwitchH, (_encoder[slaveAddress].HighLimitSwitch | 0x00FF ) >> 16);
}

uint32_t EncoderGetDelay(int slaveAddress)
{
    _encoder[slaveAddress].Delay = EncoderReadModbus(slaveAddress, encoderRegDelay);
    return _encoder[slaveAddress].Delay;
}

void EncoderSetDelay(int slaveAddress, uint32_t delay)
{
    _encoder[slaveAddress].Delay = delay;
    EncoderWriteModbus(slaveAddress, encoderRegDelay, _encoder[slaveAddress].Delay);
}

uint32_t EncoderGetErrorReg(int slaveAddress)
{
    _encoder[slaveAddress].ErrorReg = EncoderReadModbus(slaveAddress, encoderRegErrorReg);
    return _encoder[slaveAddress].ErrorReg;
}

void EncoderSetErrorReg(int slaveAddress, uint32_t errorReg)
{
    _encoder[slaveAddress].ErrorReg = errorReg;
    EncoderWriteModbus(slaveAddress, encoderRegErrorReg, _encoder[slaveAddress].ErrorReg);
}

uint32_t EncoderGetDeviceResetStore(int slaveAddress)
{
    _encoder[slaveAddress].DeviceResetStore = EncoderReadModbus(slaveAddress, encoderRegDeviceResetStore);
    return _encoder[slaveAddress].DeviceResetStore;
}

void EncoderSetDeviceResetStore(int slaveAddress, uint32_t deviceResetStore)
{
    _encoder[slaveAddress].DeviceResetStore = deviceResetStore;
    EncoderWriteModbus(slaveAddress, encoderRegDeviceResetStore, _encoder[slaveAddress].DeviceResetStore);
}

uint32_t EncoderGetParameters(int slaveAddress)
{
    _encoder[slaveAddress].Parameters = EncoderReadModbus(slaveAddress, encoderRegParameters);
    return _encoder[slaveAddress].Parameters;
}

void EncoderSetParameters(int slaveAddress, uint32_t parameters)
{
    _encoder[slaveAddress].Parameters = parameters;
    EncoderWriteModbus(slaveAddress, encoderRegParameters, _encoder[slaveAddress].Parameters);
}

uint32_t EncoderGetAutoStore(int slaveAddress)
{
    _encoder[slaveAddress].AutoStore = EncoderReadModbus(slaveAddress, encoderRegAutoStore);
    return _encoder[slaveAddress].AutoStore;
}

void EncoderSetAutoStore(int slaveAddress, uint32_t autoStore)
{
    _encoder[slaveAddress].AutoStore = autoStore;
    EncoderWriteModbus(slaveAddress, encoderRegAutoStore, _encoder[slaveAddress].AutoStore);
}

uint32_t EncoderGetRestoreAllParameters(int slaveAddress)
{
    _encoder[slaveAddress].RestoreAllParameters = EncoderReadModbus(slaveAddress, encoderRegRestoreAllParameters);
    return _encoder[slaveAddress].RestoreAllParameters;
}

void EncoderSetRestoreAllParameters(int slaveAddress, uint32_t restoreAllParameters)
{
    _encoder[slaveAddress].RestoreAllParameters = restoreAllParameters;
    EncoderWriteModbus(slaveAddress, encoderRegRestoreAllParameters, _encoder[slaveAddress].RestoreAllParameters);
}

uint32_t EncoderGetRestoreAplicationParameters(int slaveAddress)
{
    _encoder[slaveAddress].RestoreAplicationParameters = EncoderReadModbus(slaveAddress, encoderRegRestoreAplicationParameters);
    return _encoder[slaveAddress].RestoreAplicationParameters;
}

void SteRestoteAplicationParameters(int slaveAddress, uint32_t restoreAplicationParameters)
{
    _encoder[slaveAddress].RestoreAplicationParameters = restoreAplicationParameters;
    EncoderWriteModbus(slaveAddress, encoderRegRestoreAplicationParameters, _encoder[slaveAddress].RestoreAplicationParameters);
}

uint32_t EncoderGetAutoTest(int slaveAddress)
{
    _encoder[slaveAddress].AutoTest = EncoderReadModbus(slaveAddress, encoderRegAutoTest);
    return _encoder[slaveAddress].AutoTest;
}

void EncoderSetAutoTest(int slaveAddress, uint32_t autoTest)
{
    _encoder[slaveAddress].AutoTest = autoTest;
    EncoderWriteModbus(slaveAddress, encoderRegAutoTest, _encoder[slaveAddress].AutoTest);
}
    
uint32_t EncoderGetSoftwareVersion(int slaveAddress)
{
    _encoder[slaveAddress].SoftwareVersion = EncoderReadModbus(slaveAddress, encoderRegSoftwareVersion);
    return _encoder[slaveAddress].SoftwareVersion;
}
        
uint32_t EncoderGetSerialNumber(int slaveAddress)
{
    _encoder[slaveAddress].SerialNumberH = EncoderReadModbus(slaveAddress, encoderRegSerialNumberH);
    _encoder[slaveAddress].SerialNumberL = EncoderReadModbus(slaveAddress, encoderRegSerialNumberL);
    _encoder[slaveAddress].SerialNumber = (_encoder[slaveAddress].SerialNumberH) << 16 | _encoder[slaveAddress].SerialNumberL;
    return _encoder[slaveAddress].SerialNumber;
}
    
uint32_t EncoderGetLifeCycleCounter(int slaveAddress)
{
    _encoder[slaveAddress].LifeCycleCounterH = EncoderReadModbus(slaveAddress, encoderRegLifeCycleCounterH);
    _encoder[slaveAddress].LifeCycleCounterL = EncoderReadModbus(slaveAddress, encoderRegLifeCycleCounterL);
    _encoder[slaveAddress].LifeCycleCounter = (_encoder[slaveAddress].LifeCycleCounterH) << 16 | _encoder[slaveAddress].LifeCycleCounterL;
    return _encoder[slaveAddress].LifeCycleCounter;
}
    
uint32_t EncoderGetRollCounter(int slaveAddress)
{
    _encoder[slaveAddress].RollCounter = EncoderReadModbus(slaveAddress, encoderRegRollCounter);
    return _encoder[slaveAddress].RollCounter;
}
        
uint32_t EncoderGetBaudrate(int slaveAddress)
{
    _encoder[slaveAddress].Baudrate = EncoderReadModbus(slaveAddress, encoderRegBaudrate);
    return _encoder[slaveAddress].Baudrate;
}

void EncoderSetBaudrate(int slaveAddress, uint32_t baudrate)
{
    _encoder[slaveAddress].Baudrate = baudrate;
    EncoderWriteModbus(slaveAddress, encoderRegBaudrate, _encoder[slaveAddress].Baudrate);
}

uint32_t EncoderGetNumberData(int slaveAddress)
{
    _encoder[slaveAddress].NumberData = EncoderReadModbus(slaveAddress, encoderRegNumberData);
    return _encoder[slaveAddress].NumberData;
}

void EncoderSetNumberData(int slaveAddress, uint32_t numberData)
{
    _encoder[slaveAddress].NumberData = numberData;
    EncoderWriteModbus(slaveAddress, encoderRegNumberData, _encoder[slaveAddress].NumberData);
}

uint32_t EncoderParity(int slaveAddress)
{
    _encoder[slaveAddress].Parity = EncoderReadModbus(slaveAddress, encoderRegParity);
    return _encoder[slaveAddress].Parity;
}

void EncoderSetParity(int slaveAddress, uint32_t parity)
{
    _encoder[slaveAddress].Parity = parity;
    EncoderWriteModbus(slaveAddress, encoderRegParity, _encoder[slaveAddress].Parity);
}

uint32_t EncoderGetStopbits(int slaveAddress)
{
    _encoder[slaveAddress].Stopbits = EncoderReadModbus(slaveAddress, encoderRegStopbits);
    return _encoder[slaveAddress].Stopbits;
}

void EncoderSetStopbits(int slaveAddress, uint32_t stopbits)
{
    _encoder[slaveAddress].Stopbits = stopbits;
    EncoderWriteModbus(slaveAddress, encoderRegStopbits, _encoder[slaveAddress].Stopbits);
}

uint32_t EncoderGetCommUpdate(int slaveAddress)
{
    _encoder[slaveAddress].CommUpdate = EncoderReadModbus(slaveAddress, encoderRegCommUpdate);
    return _encoder[slaveAddress].CommUpdate;
}

void EncoderSetCommUpdate(int slaveAddress, uint32_t commUpdate)
{
    _encoder[slaveAddress].CommUpdate = commUpdate;
    EncoderWriteModbus(slaveAddress, encoderRegCommUpdate, _encoder[slaveAddress].CommUpdate);
}

uint32_t EncoderGetNodeAddress(int slaveAddress)
{
    _encoder[slaveAddress].NodeAddress = EncoderReadModbus(slaveAddress, encoderRegNodeAddress);
    return _encoder[slaveAddress].NodeAddress;
}

void EncoderSetNodeAddress(int slaveAddress, uint32_t nodeAddress)
{
    _encoder[slaveAddress].NodeAddress = nodeAddress;
    EncoderWriteModbus(slaveAddress, encoderRegNodeAddress, _encoder[slaveAddress].NodeAddress);
}

uint32_t EncoderGetNodeUpdate(int slaveAddress)
{
    _encoder[slaveAddress].NodeUpdate = EncoderReadModbus(slaveAddress, encoderRegNodeUpdate);
    return _encoder[slaveAddress].NodeUpdate;
}

void EncoderSetNodeUpdate(int slaveAddress, uint32_t nodeUpdate)
{
    _encoder[slaveAddress].NodeUpdate = nodeUpdate;
    EncoderWriteModbus(slaveAddress, encoderRegNodeUpdate, _encoder[slaveAddress].NodeUpdate);
}

uint32_t EncoderGetAutoBaudEnable(int slaveAddress)
{
    _encoder[slaveAddress].AutoBaudEnable = EncoderReadModbus(slaveAddress, encoderRegAutoBaudEnable);
    return _encoder[slaveAddress].AutoBaudEnable;
}

void EncoderSetAutoBaudEnable(int slaveAddress, uint32_t autoBaudEnable)
{
    _encoder[slaveAddress].AutoBaudEnable = autoBaudEnable;
    EncoderWriteModbus(slaveAddress, encoderRegAutoBaudEnable, _encoder[slaveAddress].AutoBaudEnable);
}

uint32_t EncoderGetAutoBaudTimeout(int slaveAddress)
{
    _encoder[slaveAddress].AutoBaudTimeout = EncoderReadModbus(slaveAddress, encoderRegAutoBaudTimeout);
    return _encoder[slaveAddress].AutoBaudTimeout;
}

void EncoderSetAutoBaudTimeout(int slaveAddress, uint32_t autoBaudTimeout)
{
    _encoder[slaveAddress].AutoBaudTimeout = autoBaudTimeout;
    EncoderWriteModbus(slaveAddress, encoderRegAutoBaudTimeout, _encoder[slaveAddress].AutoBaudTimeout);
}

uint32_t EncoderGetRestoreBusParameters(int slaveAddress)
{
    _encoder[slaveAddress].RestoreBusParameters = EncoderReadModbus(slaveAddress, encoderRegRestoreBusParameters);
    return _encoder[slaveAddress].RestoreBusParameters;
}

void EncoderSetRestoreBusParameters(int slaveAddress, uint32_t restoreBusParameters)
{
    _encoder[slaveAddress].RestoreBusParameters = restoreBusParameters;
    EncoderWriteModbus(slaveAddress, encoderRegRestoreBusParameters, _encoder[slaveAddress].RestoreBusParameters);
}

uint32_t EncoderGetTermination(int slaveAddress)
{
    _encoder[slaveAddress].Termination = EncoderReadModbus(slaveAddress, encoderRegTermination);
    return _encoder[slaveAddress].Termination;
}

void EncoderSetTermination(int slaveAddress, uint32_t termination)
{
    _encoder[slaveAddress].Termination = termination;
    EncoderWriteModbus(slaveAddress, encoderRegTermination, _encoder[slaveAddress].Termination);
}

uint32_t EncoderGetTermUpdate(int slaveAddress)
{
    _encoder[slaveAddress].TermUpdate = EncoderReadModbus(slaveAddress, encoderRegTermUpdate);
    return _encoder[slaveAddress].TermUpdate;
}

void EncoderSetTermUpdate(int slaveAddress, uint32_t termUpdate)
{
    _encoder[slaveAddress].TermUpdate = termUpdate;
    EncoderWriteModbus(slaveAddress, encoderRegTermUpdate, _encoder[slaveAddress].TermUpdate);
}
