// vreg_defs.h  (Auszug, erweiterbar)
#pragma once
#include <stdint.h>

struct VRegDef
{
  uint16_t id;           // VREG ID (hex)
  const char* name;      // short name
  const char* type;      // interpretation: "uint8","uint16","int32","float","string","blob","bitmask"
  int length;            // typical length in bytes (0 = variable / string / blob)
  float scale;          // multiply raw value by scale to get SI value (e.g. 0.001 for mV->V)
  const char* unit;      // human unit, empty if none
  const char* desc;      // short description
  const char* note;      // extra notes / source
};

static const VRegDef vregDefs[] =
{
  // Device / product info
  { 0x0100, "ProductID",         "uint16", 2, 1.0,    "", "Product identifier (vendor/product code)", "BlueSolar HEX / VE.Direct docs" },
  { 0x0101, "HardwareVersion",   "uint16", 2, 1.0,    "", "Hardware revision", "VE.Direct docs" },
  { 0x0102, "FirmwareVersion",   "uint16", 2, 1.0,    "", "Firmware version (often major<<8 | minor)", "VE.Direct docs" },
  { 0x010A, "SerialNumber",      "string", 0, 1.0,    "", "Serial number (ASCII/string)", "VE.Direct docs" },
  { 0x010B, "ModelName",         "string", 0, 1.0,    "", "Model name (string)", "VE.Direct docs" },
  { 0x0104, "GroupId",           "uint16", 2, 1.0,    "", "Device group identifier", "BlueSolar HEX / VE.Direct docs" },

  // Capabilities / features
  { 0x0140, "Capabilities",      "bitmask",4, 1.0,    "", "Feature bitmask (load output, BLE, etc.)", "BlueSolar HEX" },

  // Device mode / state / off-reason etc.
  { 0x0200, "DeviceMode",        "uint8",  1, 1.0,    "", "Device mode (0=off,1=on etc.) — see docs for firmware variations", "BlueSolar HEX; note: firmware dependent" },
  { 0x0201, "DeviceState",       "uint8",  1, 1.0,    "", "Operational state (charger state)", "Phoenix / MPPT docs" },
  { 0x0202, "RemoteControlMask", "uint32", 4, 1.0,    "", "Remote control mask (bits: remote on/off allowed etc.)", "BlueSolar HEX" },
  { 0x0205, "OffReason8",        "uint8",  1, 1.0,    "", "Off reason (8-bit mask) legacy", "BlueSolar HEX" },
  { 0x0206, "OffReason16",       "uint16", 2, 1.0,    "", "Off reason (16-bit mask)", "BlueSolar HEX" },
  { 0x0207, "OffReason32",       "uint32", 4, 1.0,    "", "Off reason (32-bit mask), bits documented in BlueSolar HEX", "BlueSolar HEX" },

  // Example control registers (writeable on many devices)
  { 0x0004, "FactoryReset",      "command",0, 1.0,    "", "Command to restore factory defaults (write-only)", "Phoenix HEX" },
  { 0xEB99, "NVM_Save",          "command",0, 1.0,    "", "Save to NVM / commit settings", "Phoenix HEX" },

  // MPPT common set / get registers (examples - device specific)
  { 0x2001, "ChargeVoltageSet",  "int32",  4, 0.001,  "V", "Charge voltage setpoint (mV raw -> scale 0.001)", "BlueSolar HEX" },
  { 0x200E, "NetworkMode",       "uint8",  1, 1.0,    "", "Network mode / comm mode", "BlueSolar HEX" },
  { 0x2015, "ChargeCurrentLimit","int32",  4, 0.001, "A", "Charge current limit (mA->A)", "Community / docs" },

  // Status / telemetry example registers (text-mode equivalents also exist)
  { 0x0300, "BatteryVoltage",    "int32",  4, 0.001,  "V", "Battery voltage (raw mV)", "typical mapping" },
  { 0x0301, "BatteryCurrent",    "int32",  4, 0.001,  "A", "Battery current (raw mA)", "typical mapping" },
  { 0x0302, "BatteryPower",      "int32",  4, 0.001,  "W", "Battery power (raw mW -> W)", "typical mapping" },
  { 0x0303, "SolarInputPower",   "int32",  4, 0.001,  "W", "Solar panel input power", "typical mapping" },
  { 0x0304, "LoadPower",         "int32",  4, 0.001,  "W", "Load power (raw mW -> W)", "typical mapping" },
  { 0x031C, "WarningReason",     "bitmask",4, 1.0,    "", "Warning reason mask", "Phoenix HEX" },
  { 0x031E, "AlarmReason",       "bitmask",4, 1.0,    "", "Alarm reason mask", "Phoenix HEX" },

  // History / day archive (complex blob)
  { 0x1050, "HistoryDay00",      "blob",   0, 1.0,    "", "Day archive struct (contains yield, max/min volt, etc.) — parse per device doc", "BlueSolar / VE.Direct history docs" },
  { 0x1040, "HistoryDay01",      "blob",   0, 1.0,    "", "Second day archive", "BlueSolar / VE.Direct history docs" },
  { 0x1041, "HistoryDay02",      "blob",   0, 1.0,    "", "Third day archive", "BlueSolar / VE.Direct history docs" },

  // Terminator
  { 0x0000, nullptr, nullptr, 0, 0.0, nullptr, nullptr }
};
