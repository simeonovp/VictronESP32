#pragma once
#include <inttypes.h>
#include <vector>

namespace VeDirectProt
{
enum class Command : uint8_t
{
  EnterBoot = 0u, // 0x51FA51FA51FA51FA51FA as payload will enable bootloader mode.
  Ping = 1u, // Check for presence, the response is an ‘Rsp ping’ containing version and firmware type. See the response ping message.
  AppVersion = 3u, // Returns the version of the firmware as stored in the header in an ‘RspDone’ message.
  ProductId = 4u, // Returns the Product Id of the firmware as stored in the header in an ‘RspDone’ message.
  Restart = 6u, // Restarts the device, no response is sent.
  Get = 7u, //Returns a get response with the requested data or error is returned.
    // uint16 the id of the value to get
    // uint8 flags, should be set to zero
  Set = 8u, // Returns a set response with the requested data or error is returned.
    // uint16 the id of the value to set
    // uint8 flags, should be set to zero
    // type depends on id value
  Async = 0xAu, // Asynchronous data message. Should not be replied.
    // uint16 the id of the value being returned
    // uint8 flags, defined below
    // type depends on id value
  // 2, 5, 9, B-F reserved
};

enum class Response : uint8_t
{
  Done = 1u, // Successful execution of the received command. Payload depends on command.
  Unknown = 3u, // Unknown command, data is the unknown command.
  Error = 4u, // Frame error (payload=0xAAAA), unable to enter bootloader (payload=0).
  Ping = 5u, // The version number is directly interpreted from the hex representation,
    //e.g. 0x0101 is version 1.01. The two most significant bits indicate the
    // firmware type:
    // b00: bootloader
    // b01: application
    // b10: tester
    // b11: release candidate
    // In case of release candidate the lowest two bits of the highest nibble
    // together with type indicate the release candidate number. E.g. 0xD101
    // represents release candidate D of version 1.01.
    // Note that there can only be 4 release candidates per version.
  Get = 7u, //
    // uint16 id: of the value being returned
    // uint8 flags: defined below
    // type depends on id value
  Set = 8u, //
    // uint16 id of the value which was set
    // uint8 flags: defined below
    // type depends on id value
};

enum ReplyFlags : uint8_t
{
  UnknownId = 0x01u, // The specified id does not exist
  NotSupported = 0x02u, // Attempting to write to a read only value
  ParameterError = 0x04u, // The new value is out of range or inconsistent
};

enum ProductIdValues : uint16_t
{
/*
0x0300 BlueSolar MPPT 70|15 (*2 *3) 12..24V-15A 15A -
0xA040 BlueSolar MPPT 75|50 (*3) 12..24V-50A - pv only
0xA041 BlueSolar MPPT 150|35 (*3) 12..48V-35A - pv only
0xA042 BlueSolar MPPT 75|15 12..24V-15A 15A -
0xA043 BlueSolar MPPT 100|15 12..24V-15A 15A -
0xA044 BlueSolar MPPT 100|30 (*3) 12..24V-30A - pv only
0xA045 BlueSolar MPPT 100|50 (*3) 12..24V-50A - pv only
0xA046 BlueSolar MPPT 150|70 12..48V-70A - -
0xA047 BlueSolar MPPT 150|100 12..48V-100A - -
0xA048 BlueSolar MPPT 75|50 rev2 (*3) 12..24V-50A - -
0xA049 BlueSolar MPPT 100|50 rev2 12..24V-50A - -
0xA04A BlueSolar MPPT 100|30 rev2 12..24V-30A - -
0xA04B BlueSolar MPPT 150|35 rev2 12..48V-35A - -
0xA04C BlueSolar MPPT 75|10 12..24V-10A 10A -
0xA04D BlueSolar MPPT 150|45 12..48V-45A - -
0xA04E BlueSolar MPPT 150|60 12..48V-60A - -
0xA04F BlueSolar MPPT 150|85 12..48V-85A - -
0xA050 SmartSolar MPPT 250|100 12..48V-100A - ble
0xA051 SmartSolar MPPT 150|100 (*3 *4) 12..48V-100A - ble
0xA052 SmartSolar MPPT 150|85 (*3 *4) 12..48V-85A - ble
0xA053 SmartSolar MPPT 75|15 12..24V-15A 15A ble net
0xA054 SmartSolar MPPT 75|10 12..24V-10A 10A ble net
0xA055 SmartSolar MPPT 100|15 12..24V-15A 15A ble net
0xA056 SmartSolar MPPT 100|30 12..24V-30A - ble net
0xA057 SmartSolar MPPT 100|50 12..24V-50A - ble net
0xA058 SmartSolar MPPT 150|35 12..48V-35A - ble net
0xA059 SmartSolar MPPT 150|100 rev2 12..48V-100A - ble net
0xA05A SmartSolar MPPT 150|85 rev2 12..48V-85A - ble net
0xA05B SmartSolar MPPT 250|70 12..48V-70A - ble net
0xA05C SmartSolar MPPT 250|85 12..48V-85A - ble
0xA05D SmartSolar MPPT 250|60 12..48V-60A - ble net
0xA05E SmartSolar MPPT 250|45 12..48V-45A - ble net
0xA05F SmartSolar MPPT 100|20 12..24V-20A 20A ble net
0xA060 SmartSolar MPPT 100|20 48V 12..48V-20A 100mA ble net
0xA061 SmartSolar MPPT 150|45 12..48V-45A - ble net
0xA062 SmartSolar MPPT 150|60 12..48V-60A - ble net
0xA063 SmartSolar MPPT 150|70 12..48V-70A - ble net
0xA064 SmartSolar MPPT 250|85 rev2 12..48V-85A - ble net
0xA065 SmartSolar MPPT 250|100 rev2 12..48V-100A - ble net
0xA066 BlueSolar MPPT 100|20 12..24V-20A 20A -
0xA067 BlueSolar MPPT 100|20 48V 12..48V-20A 100mA -
0xA068 SmartSolar MPPT 250|60 rev2 12..48V-60A - ble net
0xA069 SmartSolar MPPT 250|70 rev2 12..48V-70A - ble net
0xA06A SmartSolar MPPT 150|45 rev2 12..48V-45A - ble net
0xA06B SmartSolar MPPT 150|60 rev2 12..48V-60A - ble net
0xA06C SmartSolar MPPT 150|70 rev2 12..48V-70A - ble net
0xA06D SmartSolar MPPT 150|85 rev2 12..48V-85A - ble net
0xA06E SmartSolar MPPT 150|100 rev3 12..48V-100A - ble net
0xA06F BlueSolar MPPT 150|45 rev2 12..48V-45A - -
0xA070 BlueSolar MPPT 150|60 rev2 12..48V-60A - -
0xA071 BlueSolar MPPT 150|70 rev2 12..48V-70A - -
0xA072 BlueSolar MPPT 150|45 rev3 12..48V-45A - -
0xA073 SmartSolar MPPT 150|45 rev3 12..48V-45A - ble net
0xA074 SmartSolar MPPT 75|10 rev2 12..24V-10A 10A ble net
0xA075 SmartSolar MPPT 75|15 rev2 12..24V-15A 15A ble net
0xA076 BlueSolar MPPT 100|30 rev3 12..24V-30A - -
0xA077 BlueSolar MPPT 100|50 rev3 12..24V-50A - -
0xA078 BlueSolar MPPT 150|35 rev2 12..48V-35A - -
0xA079 BlueSolar MPPT 75|10 rev2 12..24V-10A 10A -
0xA07A BlueSolar MPPT 75|15 rev2 12..24V-15A 15A -
0xA07B BlueSolar MPPT 100|15 rev2 12..24V-15A 15A -
0xA07C BlueSolar MPPT 75/10 rev3 12..24V-10A 10A -
0xA07D BlueSolar MPPT 75/15 rev3 12..24V-15A 15A -
0xA07E SmartSolar Charger MPPT 100/30 12V-30A - ble net
0xA102 SmartSolar MPPT VE.Can 150|70 12..48V-70A - ble net
0xA103 SmartSolar MPPT VE.Can 150|45 12..48V-45A - ble net
0xA104 SmartSolar MPPT VE.Can 150|60 12..48V-60A - ble net
0xA105 SmartSolar MPPT VE.Can 150|85 12..48V-85A - ble net
0xA106 SmartSolar MPPT VE.Can 150|100 12..48V-100A - ble net
0xA107 SmartSolar MPPT VE.Can 250|45 12..48V-45A - ble net
0xA108 SmartSolar MPPT VE.Can 250|60 12..48V-60A - ble net
0xA109 SmartSolar MPPT VE.Can 250|70 12..48V-70A - ble net
0xA10A SmartSolar MPPT VE.Can 250|85 12..48V-85A - ble net
0xA10B SmartSolar MPPT VE.Can 250|100 12..48V-100A - ble net
0xA10C SmartSolar MPPT VE.Can 150|70
rev2
12..48V-70A - ble net
0xA10D SmartSolar MPPT VE.Can 150|85
rev2
12..48V-85A - ble net
0xA10E SmartSolar MPPT VE.Can 150|100
rev2
12..48V-100A - ble net
0xA10F BlueSolar MPPT VE.Can 150|100 12..48V-100A - -
0xA110 SmartSolar MPPT RS 450/100 48V-100A - 2 trackers
0xA111 SmartSolar MPPT RS 450/200 48V-200A - 4 BlueSolar MPPT VE.Can 250|70 12..48V-70A - -
0xA113 BlueSolar MPPT VE.Can 250|100 12..48V-100A - -
0xA114 SmartSolar MPPT VE.Can 250|70
rev2
12..48V-70A - ble net
0xA115 SmartSolar MPPT VE.Can 250|100
rev2
12..48V-7100A - ble net
0xA116 SmartSolar MPPT VE.Can 250|85
rev2
12..48V-85A - ble net
0xA117 BlueSolar MPPT VE.Can 150|100
rev2
12..48V-100A - -
*/
};

enum class RT //RegType
{
  none,
  un8,
  un16,
  un32,
  sn8,
  sn16,
  sn32,
  string,
  raw,
};

const char* to_string(RT regType)
{
  switch (regType)
  {
  case RT::un8: return "un8";
  case RT::un16: return "un16";
  case RT::un32: return "un32";
  case RT::sn8: return "sn8";
  case RT::sn16: return "sn16";
  case RT::sn32: return "sn32";
  case RT::string: return "string";
  case RT::raw: return "raw";
  default: return "";
  }
}

enum class Unit
{
  none,
  V,
  A,
  W,
  kWh,
  Ohm,
  C,
  K,
  mV_K,
  hours,
  min,
  sec,
  ms,
  perc,
  // raw
  _hdr,
};

const char* to_string(Unit unit)
{
  switch (unit)
  {
  case Unit::V: return "V";
  case Unit::A: return "A";
  case Unit::W: return "W";
  case Unit::kWh: return "kWh";
  case Unit::Ohm: return "Ohm";
  case Unit::C: return "°C";
  case Unit::K: return "K";
  case Unit::mV_K: return "mV/K";
  case Unit::hours: return "hours";
  case Unit::min: return "min";
  case Unit::sec: return "s";
  case Unit::ms: return "ms";
  case Unit::perc: return "%";
  default: return "";
  }
};

struct VRegDefine
{
  uint16_t id;           // VREG ID (hex)
  const char* name;      // short name
  float scale;          // multiply raw value by scale to get SI value (e.g. 0.001 for mV->V)
  RT type;      // interpretation: "uint8","uint16","int32","float",RT::string,"blob","bitmask"
  Unit unit;      // human unit, empty if none
  const char* desc;      // short description
  const char* note;      // extra notes / source
  const char* mqttTopic;  // eg. "ve/device/state"
  const char* dbusPath;   // eg. "/Device/State"
};

static const VRegDefine RegDefs[] =
{
  // Product information registers
  { 0x0100, "Product Id",                     0.,     RT::un32,  Unit::none,  "",                                "", "ve/product/productid",         "/Product/ProductId" },
  { 0x0104, "Group Id",                       0.,     RT::un8,   Unit::none,  "",                                "", "ve/product/groupid",           "/Product/GroupId" },
  { 0x010A, "Serial number",                  0.,     RT::string,Unit::none,  "",                                "", "ve/product/serial",            "/Product/SerialNumber" },
  { 0x010B, "Model name",                     0.,     RT::string,Unit::none,  "",                                "", "ve/product/model",             "/Product/ModelName" },
  { 0x0140, "Capabilities",                   0.,     RT::un32,  Unit::none,  "",                                "", "ve/product/capabilities",      "/Product/Capabilities" },

  // Generic device control registers
  { 0x0200, "Device mode",                    0.,     RT::un8,   Unit::none,  "",                                "", "ve/device/mode",               "/Device/Mode" },
  { 0x0201, "Device state",                   0.,     RT::un8,   Unit::none,  "",                                "", "ve/device/state",              "/Device/State" },
  { 0x0202, "Remote control used",            0.,     RT::un32,  Unit::none,  "",                                "", "ve/device/remotecontrolused",  "/Device/RemoteControlUsed" },
  { 0x0205, "Device off reason",              0.,     RT::un8,   Unit::none,  "",                                "", "ve/device/offreason",          "/Device/OffReason" },
  { 0x0207, "Device off reason",              0.,     RT::un32,  Unit::none,  "",                                "", "ve/device/offreason_extended", "/Device/OffReasonExtended" },

  // Battery settings registers
  { 0xEDFF, "Batterysafe mode",               0.,     RT::un8,   Unit::none,  "0=off, 1=on",                     "", "ve/battery/safe_mode",         "/Battery/SafeMode" },
  { 0xEDFE, "Adaptive mode",                  0.,     RT::un8,   Unit::none,  "0=off, 1=on",                     "", "ve/battery/adaptive_mode",     "/Battery/AdaptiveMode" },
  { 0xEDFD, "Automatic equalisation mode",    0.,     RT::un8,   Unit::none,  "0=off, 1..250",                   "", "ve/battery/auto_equalisation", "/Battery/AutomaticEqualisationMode" },
  { 0xEDFC, "Battery bulk time limit",        0.01,  RT::un16,  Unit::hours, "",                                "", "ve/battery/bulk_time_limit",   "/Battery/BulkTimeLimit" },
  { 0xEDFB, "Battery absorption time limit",  0.01,  RT::un16,  Unit::hours, "",                                "", "ve/battery/absorption_time",   "/Battery/AbsorptionTimeLimit" },
  { 0xEDF7, "Battery absorption voltage",     0.01,  RT::un16,  Unit::V,     "",                                "", "ve/battery/absorption_voltage","/Battery/AbsorptionVoltage" },
  { 0xEDF6, "Battery float voltage",          0.01,  RT::un16,  Unit::V,     "",                                "", "ve/battery/float_voltage",     "/Battery/FloatVoltage" },
  { 0xEDF4, "Battery equalisation voltage",   0.01,  RT::un16,  Unit::V,     "",                                "", "ve/battery/equalisation_voltage","/Battery/EqualisationVoltage" },
  { 0xEDF2, "Battery temp. compensation",     0.01,  RT::sn16,  Unit::mV_K,  "",                                "", "ve/battery/temp_compensation", "/Battery/TempCompensation" },
  { 0xEDF1, "Battery type",                    1.,    RT::un8,   Unit::none,  "0xFF = user",                     "", "ve/battery/type",              "/Battery/Type" },
  { 0xEDF0, "Battery maximum current",        0.1,   RT::un16,  Unit::A,     "",                                "", "ve/battery/max_current",       "/Battery/MaximumCurrent" },
  { 0xEDEF, "Battery voltage",                1.,    RT::un8,   Unit::V,     "",                                "", "ve/battery/voltage",           "/Battery/Voltage" },
  { 0xEDEC, "Battery temperature",            0.01,  RT::un16,  Unit::K,     "0xFFFF=N/A",                      "", "ve/battery/temperature",       "/Battery/Temperature" },
  { 0xEDEA, "Battery voltage setting",        1.,    RT::un8,   Unit::V,     "",                                "", "ve/battery/voltage_setting",   "/Battery/VoltageSetting" },
  { 0xEDE8, "BMS present",                    0.,    RT::un8,   Unit::none,  "0=no, 1=yes",                     "", "ve/battery/bms_present",       "/Battery/BMSPresent" },
  { 0xEDE7, "Tail current",                   0.1,   RT::un16,  Unit::none,  "",                                "", "ve/battery/tail_current",      "/Battery/TailCurrent" },
  { 0xEDE6, "Low temperature charge current", 0.1,   RT::un16,  Unit::A,     "0xFFFF=use max",                  "", "ve/battery/lowtemp_charge_current", "/Battery/LowTemperatureChargeCurrent" },
  { 0xEDE5, "Auto equalise stop on voltage",  0.,    RT::un8,   Unit::none,  "0=no, 1=yes",                     "", "ve/battery/auto_equalise_stop", "/Battery/AutoEqualiseStopOnVoltage" },
  { 0xEDE4, "Equalisation current level",     1.,    RT::un8,   Unit::perc,  "(of 0xEDF0)",                     "", "ve/battery/equalisation_current", "/Battery/EqualisationCurrentLevel" },
  { 0xEDE3, "Equalisation duration",          0.01,  RT::un16,  Unit::hours, "",                                "", "ve/battery/equalisation_duration","/Battery/EqualisationDuration" },
  { 0xED2E, "Re-bulk voltage offset",         0.01,  RT::un16,  Unit::V,     "",                                "", "ve/battery/rebulk_voltage_offset", "/Battery/ReBulkVoltageOffset" },
  { 0xEDE0, "Battery low temperature level",  0.01,  RT::sn16,  Unit::C,     "",                                "", "ve/battery/low_temp_level",     "/Battery/LowTemperatureLevel" },
  { 0xEDCA, "Voltage compensation",           0.01,  RT::un16,  Unit::V,     "",                                "", "ve/battery/voltage_compensation","/Battery/VoltageCompensation" },

  // Charger data registers
  { 0xEDEC, "Battery temperature",           0.01,  RT::un16,  Unit::K,     "",                                "", "ve/charger/battery_temperature", "/Charger/BatteryTemperature" },
  { 0xEDDF, "Charger maximum current",       0.1,   RT::un16,  Unit::A,     "",                                "", "ve/charger/max_current",       "/Charger/MaximumCurrent" },
  { 0xEDDD, "System yield",                  0.01,  RT::un32,  Unit::kWh,  "",                                "", "ve/charger/system_yield",      "/Charger/SystemYield" },
  { 0xEDDC, "User yield (resettable)",       0.01,  RT::un32,  Unit::kWh,  "",                                "", "ve/charger/user_yield",        "/Charger/UserYield" },
  { 0xEDDB, "Charger internal temperature",  0.01,  RT::sn16,  Unit::C,    "",                                "", "ve/charger/internal_temp",     "/Charger/InternalTemperature" },
  { 0xEDDA, "Charger error code",            0.,    RT::un8,   Unit::none, "",                                "", "ve/charger/error_code",        "/Charger/ErrorCode" },
  { 0xEDD7, "Charger current",               0.1,   RT::un16,  Unit::A,    "",                                "", "ve/charger/current",           "/Charger/Current" },
  { 0xEDD5, "Charger voltage",               0.01,  RT::un16,  Unit::V,    "",                                "", "ve/charger/voltage",           "/Charger/Voltage" },
  { 0xEDD4, "Additional charger state info", 0.,    RT::un8,   Unit::none, "",                                "", "ve/charger/state_info",        "/Charger/AdditionalStateInfo" },
  { 0xEDD3, "Yield today",                   0.01,  RT::un16,  Unit::kWh,  "RT::un32 for version <= 1.12",   "", "ve/charger/yield_today",       "/Charger/YieldToday" },
  { 0xEDD2, "Maximum power today",           1.,    RT::un16,  Unit::W,    "",                                "", "ve/charger/max_power_today",   "/Charger/MaximumPowerToday" },
  { 0xEDD1, "Yield yesterday",               0.01,  RT::un16,  Unit::kWh,  "RT::un32 for version <= 1.12",   "", "ve/charger/yield_yesterday",   "/Charger/YieldYesterday" },
  { 0xEDD0, "Maximum power yesterday",       1.,    RT::un16,  Unit::W,    "",                                "", "ve/charger/max_power_yesterday","/Charger/MaximumPowerYesterday" },
  { 0xEDCE, "Voltage settings range",        0.,    RT::un16,  Unit::none, "",                                "", "ve/charger/voltage_settings_range","/Charger/VoltageSettingsRange" },
  { 0xEDCD, "History version",               0.,    RT::un8,   Unit::none, "",                                "", "ve/charger/history_version",   "/Charger/HistoryVersion" },
  { 0xEDCC, "Streetlight version",           0.,    RT::un8,   Unit::none, "",                                "", "ve/charger/streetlight_version","/Charger/StreetlightVersion" },
  { 0xEDC7, "Equalise current maximum",      1.,    RT::un8,   Unit::perc, "",                                "", "ve/charger/equalise_current_max","/Charger/EqualiseCurrentMax" },
  { 0xEDC6, "Equalise voltage maximum",      0.01,  RT::un16,  Unit::V,    "",                                "", "ve/charger/equalise_voltage_max","/Charger/EqualiseVoltageMax" },
  { 0x2211, "Adjustable voltage minimum",    0.01,  RT::un16,  Unit::V,    "",                                "", "ve/adjustable/voltage_min",    "/Adjustable/VoltageMinimum" },
  { 0x2212, "Adjustable voltage maximum",    0.01,  RT::un16,  Unit::V,    "",                                "", "ve/adjustable/voltage_max",    "/Adjustable/VoltageMaximum" },

  // DC channel registers - MPPT RS models only
  { 0xED8B, "Battery ripple voltage",        0.01,  RT::un16,  Unit::V,    "",                                "", "ve/battery/ripple_voltage",    "/Battery/RippleVoltage" },
  { 0xED8D, "Battery voltage",               0.01,  RT::sn16,  Unit::V,    "",                                "", "ve/battery/voltage_channel",   "/Battery/VoltageChannel" },
  { 0xED8F, "Battery current",               0.1,   RT::sn16,  Unit::A,    "",                                "", "ve/battery/current_channel",   "/Battery/CurrentChannel" },

  // Solar panel data registers
  { 0x0244, "Number of MPPT trackers",       0.,    RT::un8,   Unit::none, "",                                "", "ve/panel/mppt_trackers",       "/Panel/NumberOfMPPTTrackers" },
  { 0xEDBF, "Panel maximum current",         0.1,   RT::un16,  Unit::A,    "",                                "", "ve/panel/max_current",         "/Panel/MaximumCurrent" },
  { 0xEDBC, "Panel power",                   0.01,  RT::un32,  Unit::W,    "",                                "", "ve/panel/power",               "/Panel/Power" },
  { 0xEDBB, "Panel voltage",                 0.01,  RT::un16,  Unit::V,    "",                                "", "ve/panel/voltage",             "/Panel/Voltage" },
  { 0xEDBD, "Panel current",                 0.1,   RT::un16,  Unit::A,    "",                                "", "ve/panel/current",             "/Panel/Current" },
  { 0xEDB8, "Panel maximum voltage",         0.01,  RT::un16,  Unit::V,    "",                                "", "ve/panel/max_voltage",         "/Panel/MaximumVoltage" },
  { 0xEDB3, "Tracker mode",                  0.,    RT::un8,   Unit::none, "",                                "", "ve/panel/tracker_mode",        "/Panel/TrackerMode" },
  { 0xEDB2, "Panel starting voltage",        0.01,  RT::un16,  Unit::V,    "",                                "", "ve/panel/starting_voltage",    "/Panel/StartingVoltage" },
  { 0xEDB1, "Panel input resistance",        1.,    RT::un32,  Unit::Ohm,  "",                                "", "ve/panel/input_resistance",    "/Panel/InputResistance" },
  // Solar panel data individual MPPT trackers registers - MPPT RS models only
// Panel power (see 0xEDBC) 0xECCC 0xECDC 0xECEC 0xECFC
// Panel voltage (see 0xEDBB) 0xECCB 0xECDB 0xECEB 0xECFB
// Panel current (see 0xEDBD) 0xECCD 0xECDD 0xECED 0xECFD
// Tracker mode (see 0xEDB3) 0xECC3 0xECD3 0xECE3 0xECF3
  // Load output data/settings registers
  { 0xEDAD, "Load current",                  0.1,   RT::un16,  Unit::A,    "",                                "", "ve/load/current",              "/Load/Current" },
  { 0xEDAC, "Load offset voltage",           0.01,  RT::un8,   Unit::V,    "",                                "", "ve/load/offset_voltage",       "/Load/OffsetVoltage" },
  { 0xEDAB, "Load output control",           0.,    RT::un8,   Unit::none, "",                                "", "ve/load/output_control",       "/Load/OutputControl" },
  { 0xEDA9, "Load output voltage",           0.01,  RT::un16,  Unit::V,    "",                                "", "ve/load/output_voltage",       "/Load/OutputVoltage" },
  { 0xEDA8, "Load output state",             0.,    RT::un8,   Unit::none, "",                                "", "ve/load/output_state",         "/Load/OutputState" },
  { 0xED9D, "Load switch high level",        0.01,  RT::un16,  Unit::V,    "",                                "", "ve/load/switch_high_level",    "/Load/SwitchHighLevel" },
  { 0xED9C, "Load switch low level",         0.01,  RT::un16,  Unit::V,    "",                                "", "ve/load/switch_low_level",     "/Load/SwitchLowLevel" },
  { 0xED91, "Load output off reason",        0.,    RT::un8,   Unit::none, "",                                "", "ve/load/off_reason",           "/Load/OffReason" },
  { 0xED90, "Load AES timer",                1.,    RT::un16,  Unit::min,  "",                                "", "ve/load/aes_timer",            "/Load/AEStimer" },

  // Relay settings registers
  { 0xEDD9, "Relay operation mode",          0.,    RT::un8,   Unit::none, "",                                "", "ve/relay/operation_mode",      "/Relay/OperationMode" },
  { 0x0350, "Relay battery low voltage set", 0.01,  RT::un16,  Unit::V,    "",                                "", "ve/relay/batt_low_set",        "/Relay/BatteryLowVoltageSet" },
  { 0x0351, "Relay battery low voltage clear",0.01,  RT::un16,  Unit::V,    "",                                "", "ve/relay/batt_low_clear",      "/Relay/BatteryLowVoltageClear" },
  { 0x0352, "Relay battery high voltage set",0.01,  RT::un16,  Unit::V,    "",                                "", "ve/relay/batt_high_set",       "/Relay/BatteryHighVoltageSet" },
  { 0x0353, "Relay battery high voltage clear",0.01, RT::un16,  Unit::V,    "",                                "", "ve/relay/batt_high_clear",     "/Relay/BatteryHighVoltageClear" },
  { 0xEDBA, "Relay panel high voltage set",  0.01,  RT::un16,  Unit::V,    "",                                "", "ve/relay/panel_high_set",      "/Relay/PanelHighVoltageSet" },
  { 0xEDB9, "Relay panel high voltage clear",0.01,  RT::un16,  Unit::V,    "",                                "", "ve/relay/panel_high_clear",    "/Relay/PanelHighVoltageClear" },
  { 0x100A, "Relay minimum enabled time",    1.,    RT::un16,  Unit::min,  "",                                "", "ve/relay/min_enabled_time",    "/Relay/MinimumEnabledTime" },

  // Lighting controller timer
  { 0xEDA0, "Timer event 0",                 0.,    RT::un32,  Unit::none, "",                                "", "ve/timer/event0",             "/Timer/Event0" },
  { 0xEDA1, "Timer event 1",                 0.,    RT::un32,  Unit::none, "",                                "", "ve/timer/event1",             "/Timer/Event1" },
  { 0xEDA2, "Timer event 2",                 0.,    RT::un32,  Unit::none, "",                                "", "ve/timer/event2",             "/Timer/Event2" },
  { 0xEDA3, "Timer event 3",                 0.,    RT::un32,  Unit::none, "",                                "", "ve/timer/event3",             "/Timer/Event3" },
  { 0xEDA4, "Timer event 4",                 0.,    RT::un32,  Unit::none, "",                                "", "ve/timer/event4",             "/Timer/Event4" },
  { 0xEDA5, "Timer event 5",                 0.,    RT::un32,  Unit::none, "",                                "", "ve/timer/event5",             "/Timer/Event5" },
  { 0xEDA7, "Mid-point shift",               1.,    RT::sn16,  Unit::min,  "",                                "", "ve/timer/midpoint_shift",     "/Timer/MidPointShift" },
  { 0xED9B, "Gradual dim speed",             1.,    RT::un8,   Unit::sec,  "",                                "", "ve/timer/gradual_dim_speed",  "/Timer/GradualDimSpeed" },
  { 0xED9A, "Panel voltage night",           0.01,  RT::un16,  Unit::V,    "",                                "", "ve/panel/voltage_night",      "/Panel/VoltageNight" },
  { 0xED99, "Panel voltage day",             0.01,  RT::un16,  Unit::V,    "",                                "", "ve/panel/voltage_day",        "/Panel/VoltageDay" },
  { 0xED96, "Sunset delay",                  1.,    RT::un16,  Unit::min,  "",                                "", "ve/panel/sunset_delay",       "/Panel/SunsetDelay" },
  { 0xED97, "Sunrise delay",                 1.,    RT::un16,  Unit::min,  "",                                "", "ve/panel/sunrise_delay",      "/Panel/SunriseDelay" },
  { 0xED90, "AES Timer",                     1.,    RT::un16,  Unit::min,  "",                                "", "ve/load/aes_timer_dup",       "/Load/AESTimer" },
  { 0x2030, "Solar activity",                0.,    RT::un8,   Unit::none, "0=dark, 1=light",                "", "ve/solar/activity",          "/Solar/Activity" },
  { 0x2031, "Time-of-day",                   1.,    RT::un16,  Unit::min,  "0=mid-night",                    "", "ve/solar/time_of_day",        "/Solar/TimeOfDay" },

  // VE.Direct port functions
  { 0xED9E, "TX Port operation mode",        0.,    RT::un8,   Unit::none, "",                                "", "ve/vedirect/tx_port_mode",   "/VEDirect/TXPortOperationMode" },
  { 0xED98, "RX Port operation mode",        0.,    RT::un8,   Unit::none, "",                                "", "ve/vedirect/rx_port_mode",   "/VEDirect/RXPortOperationMode" },

  // Restore factory defaults
  { 0x0004, "Restore default",               0.,    RT::none,  Unit::none, "",                                "", "ve/device/restore_default",  "/Device/RestoreDefault" },

  // History data
  { 0x1030, "Clear history",                 0.,    RT::none,  Unit::none, "",                                "", "ve/history/clear",           "/History/Clear" },
// 0x104F Total history
// 0x1050 ... Daily history (0x1050=today, 0x1051=yesterday, ...) (*1)
  { 0x1050, "Daily history today",           0.,    RT::raw,   Unit::_hdr,  "",                                "", "ve/history/daily_today",     "/History/DailyToday" },
// 0x106E
// 0x10A0 ... Daily MPPT history (0x10A0=today, 0x10A1=yesterday, ...) (*1, *2)
// 0x10BE
  // Pluggable display settings
  { 0x0400, "Display backlight mode",        0.,    RT::un8,   Unit::none, "(0 = keypress, 1 = on, 2 = auto)", "", "ve/display/backlight_mode",   "/Display/BacklightMode" },
  { 0x0401, "Display backlight intensity",   0.,    RT::un8,   Unit::none, "(0 = always off, 1 = on)",       "", "ve/display/backlight_intensity","/Display/BacklightIntensity" },
  { 0x0402, "Display scroll text speed",     0.,    RT::un8,   Unit::none, "(1 = slow, 5 = fast)",            "", "ve/display/scroll_speed",     "/Display/ScrollTextSpeed" },
  { 0x0403, "Display setup lock",            0.,    RT::un8,   Unit::none, "(0 = unlocked, 1 = locked)",      "", "ve/display/setup_lock",       "/Display/SetupLock" },
  { 0x0404, "Display temperature unit",      0.,    RT::un8,   Unit::none, "(0 = Celsius, 1 = Fahrenheit)",   "", "ve/display/temp_unit",        "/Display/TemperatureUnit" },
  // Internal display settings
  { 0x0406, "Display contrast",              0.,    RT::un8,   Unit::none, "",                                "", "ve/display/contrast",         "/Display/Contrast" },
  { 0x0408, "Display backlight mode",        0.,    RT::un8,   Unit::none, "(0 = off, 1 = on, 2 = auto)",     "", "ve/display/backlight_mode_dup","/Display/BacklightModeAlt" },

  // Remote control registers
  { 0x2000, "Charge algorithm version",      0.,    RT::un8,   Unit::none, "",                                "", "ve/remote/charge_algo_version","/Remote/ChargeAlgorithmVersion" },
  { 0x2001, "Charge voltage set-point",      0.01,  RT::un16,  Unit::V,    "",                                "", "ve/remote/charge_voltage_setpoint", "/Remote/ChargeVoltageSetpoint" },
  { 0x2002, "Battery voltage sense",         0.01,  RT::un16,  Unit::V,    "",                                "", "ve/remote/battery_voltage_sense", "/Remote/BatteryVoltageSense" },
  { 0x2003, "Battery temperature sense",     0.01,  RT::sn16,  Unit::C,    "",                                "", "ve/remote/battery_temp_sense", "/Remote/BatteryTemperatureSense" },
  { 0x2004, "Remote command",                 0.,    RT::un8,   Unit::none, "",                                "", "ve/remote/command",           "/Remote/Command" },
  { 0x2007, "Charge state elapsed time",     1.,    RT::un32,  Unit::ms,   "",                                "", "ve/remote/charge_state_elapsed_ms", "/Remote/ChargeStateElapsedTime" },
  { 0x2008, "Absorption time",               0.01,  RT::un16,  Unit::hours, "",                               "", "ve/remote/absorption_time",   "/Remote/AbsorptionTime" },
  { 0x2009, "Error code",                    0.,    RT::un8,   Unit::none, "",                                "", "ve/remote/error_code",        "/Remote/ErrorCode" },
  { 0x200A, "Battery charge current",        0.001, RT::sn32,  Unit::A,    "",                                "", "ve/remote/charge_current",    "/Remote/BatteryChargeCurrent" },
  { 0x200B, "Battery idle voltage",          0.01,  RT::un16,  Unit::V,    "",                                "", "ve/remote/idle_voltage",      "/Remote/BatteryIdleVoltage" },
  { 0x200C, "Device state",                  0.,    RT::un8,   Unit::none, "",                                "", "ve/remote/device_state",      "/Remote/DeviceState" },
  { 0x200D, "Network info",                  0.,    RT::un8,   Unit::none, "",                                "", "ve/remote/network_info",      "/Remote/NetworkInfo" },
  { 0x200E, "Network mode",                  0.,    RT::un8,   Unit::none, "",                                "", "ve/remote/network_mode",      "/Remote/NetworkMode" },
  { 0x200F, "Network status register",       0.,    RT::un8,   Unit::none, "",                                "", "ve/remote/network_status",    "/Remote/NetworkStatusRegister" },
  { 0x2013, "Total charge current",          0.001, RT::sn32,  Unit::A,    "",                                "", "ve/remote/total_charge_current","/Remote/TotalChargeCurrent" },
  { 0x2014, "Charge current percentage",     1.,    RT::un8,   Unit::perc, "",                                "", "ve/remote/charge_current_pct", "/Remote/ChargeCurrentPercentage" },
  { 0x2015, "Charge current limit",          0.1,   RT::un16,  Unit::A,    "",                                "", "ve/remote/charge_current_limit","/Remote/ChargeCurrentLimit" },
  { 0x2018, "Manual equalisation pending",   0.,    RT::un8,   Unit::none, "",                                "", "ve/remote/manual_equalisation_pending","/Remote/ManualEqualisationPending" },
  { 0x2027, "Total DC input power",          0.01,  RT::un32,  Unit::W,    "",                                "", "ve/remote/total_dc_input_power","/Remote/TotalDCInputPower" },

  // terminator placeholder (undefined)
  { 0xFFFF, "Undefined",                     0.,    RT::un8,   Unit::none, "Undefined placeholder",           "", "ve/misc/undefined",           "/Misc/Undefined" },
};

/*
History day record (registers 0x1050..0x106E) - 34 bytes payload
Bytes Description Scale Type Unit
*/
struct __attribute__((packed)) HistoryDayRecord
{
  uint8_t Reserved; // [0]
  uint32_t Yield; // [1] Yield 0.01 un32 kWh
  uint32_t Consumed; // [5] Consumed (*1) 0.01 un32 kWh
  uint16_t UBatMax; // [9] Battery voltage maximum 0.01 un16 V
  uint16_t UBatMin; // [11] Battery voltage minimum 0.01 un16 V
  uint8_t ErrorDB; // [13] Error database (=0) - un8 -
  uint8_t Error0; // [14] Error 0 (most recent) (*2) - un8 -
  uint8_t Error1; // [15] Error 1 (*2) - un8 -
  uint8_t Error2; // [16] Error 1 (*2) - un8 -
  uint8_t Error3; // [17] Error 1 (*2) - un8 -
  uint16_t TimeBulk; // [18] Time bulk 1 un16 min
  uint16_t TimeAbs; // [20] Time absorption 1 un16 min
  uint16_t TimeFloat; // [22] Time float 1 un16 min
  uint32_t PowerMax; // [24] Power maximum 1 un32 W
  uint16_t BattCurrMax; // [28] Battery current maximum 0.1 un16 A
  uint16_t UPanelMax; // [30] Panel voltage maximum 0.01 un16 V
  uint16_t DaySeqNr; // [32] Day sequence number (*3) - un16 -
};

std::string HistoryDayRecordString(const uint8_t* pData, size_t len)
{
  static_assert(34u == sizeof(HistoryDayRecord), "Invalid HistoryDayRecord");
  if (sizeof(HistoryDayRecord) > len) return "";

  const auto& rec = *reinterpret_cast<const HistoryDayRecord*>(pData);
  std::string s;
  s.reserve(256); // grob vorreservieren

  s += "{\n";
  s += "  \"Yield\": "; s += std::to_string(rec.Yield); s += ",\n";
  s += "  \"Consumed\": "; s += std::to_string(rec.Consumed); s += ",\n";
  s += "  \"UBatMax\": "; s += std::to_string(rec.UBatMax); s += ",\n";
  s += "  \"UBatMin\": "; s += std::to_string(rec.UBatMin); s += ",\n";

  // Fehlerarray
  s += "  \"Errors\": [";
  bool first = true;
  auto appendError = [&](uint8_t err){
    if(err != 0){
      if(!first) s += ",";
      s += std::to_string(err);
      first = false;
    }
  };
  appendError(rec.ErrorDB);
  appendError(rec.Error0);
  appendError(rec.Error1);
  appendError(rec.Error2);
  appendError(rec.Error3);
  s += "],\n";

  s += "  \"TimeBulk\": "; s += std::to_string(rec.TimeBulk); s += ",\n";
  s += "  \"TimeAbs\": "; s += std::to_string(rec.TimeAbs); s += ",\n";
  s += "  \"TimeFloat\": "; s += std::to_string(rec.TimeFloat); s += ",\n";
  s += "  \"PowerMax\": "; s += std::to_string(rec.PowerMax); s += ",\n";
  s += "  \"BattCurrMax\": "; s += std::to_string(rec.BattCurrMax); s += ",\n";
  s += "  \"UPanelMax\": "; s += std::to_string(rec.UPanelMax); s += ",\n";
  s += "  \"DaySeqNr\": "; s += std::to_string(rec.DaySeqNr); s += "\n";
  s += "}";

  return s;
}

enum Capabilities : uint32_t
{
/*
//Bit Description
0 Load output present (0=no, 1=yes)
1 Rotary encoder present (0=no, 1=yes)
2 History support (0=no, 1=yes)
3 Batterysafe mode (0=no, 1=yes)
4 Adaptive mode (0=no, 1=yes)
5 Manual equalise (0=no, 1=yes)
6 Automatic equalise (0=no, 1=yes)
7 Storage mode (0=no, 1=yes)
8 Remote on/off via rx pin (0=no, 1=yes)
9 Solar timer/streetlighting (0=no, 1=yes)
10 Alternative VE.Direct TX pin function (0=no, 1=yes)
11 User defined load switch (0=no, 1=yes)
12 Load current in TEXT protocol (0=no, 1=yes)
13 Panel current (0=no, 1=yes)
14 BMS support (0=no, 1=yes)
15 External control support (0=no, 1=yes)
16 Synchronized charging support (0=no, 1=yes)
17 Alarm relay (0=no, 1=yes)
18 Alternative VE.Direct RX pin function (0=no, 1=yes)
19 Virtual load output (0=no, 1=yes)
20 Virtual relay (0=no, 1=yes)
21 Plugin display support (0=no, 1=yes)

25 Load Automatic Energy Selector (0=no, 1=yes)
26 Battery test (0=no, 1=yes)
27 PAYGO support (0=no, 1=yes)
*/
};

const VRegDefine* LookupRegDefs(uint16_t id)
{
  if ((0x1050u < id) && (0x106Eu >= id)) id = 0x1050u;
  else if ((0x10A0u < id) && (0x10BEu >= id)) id = 0x10A0u;
  for (auto* v = RegDefs; v->id != 0; ++v)
  {
    if (v->id == id) return v;
  }
  return nullptr;
}

std::string ValueString(const VRegDefine& def, const uint8_t* pData, size_t len)
{
  auto& regType = def.type;
  switch (regType)
  {
  case RT::un8: return (len < 1) ? "" : std::to_string(pData[0]);
  case RT::un16: return (len < 2) ? "" : std::to_string(pData[0] | (pData[1] << 8));
  case RT::un32: return (len < 4) ? "" : std::to_string(pData[0] | (pData[1] << 8) | (pData[2] << 16) | (pData[3] << 24));
  case RT::sn8: return (len < 1) ? "" : std::to_string(static_cast<int8_t>(pData[0]));
  case RT::sn16: return (len < 2) ? "" : std::to_string(static_cast<int16_t>(pData[0] | (pData[1] << 8)));
  case RT::sn32: return (len < 4) ? ""
      : std::to_string(static_cast<int32_t>(pData[0] | (pData[1] << 8) | (pData[2] << 16) | (pData[3] << 24)));
  case RT::string:
    return std::string(reinterpret_cast<const char*>(pData), len);
  case RT::raw:
    {
      switch (def.unit)
      {
        case Unit::_hdr: return HistoryDayRecordString(pData, len);
      }
    }
    return "";
  default: return "unknown type";
  }
}

double NormValue(const VRegDefine& def, const uint8_t* pData, size_t len)
{
  auto& regType = def.type;
  auto val = 0.;
  switch (regType)
  {
  case RT::un8: if (len >= 1) val = pData[0]; break;
  case RT::un16: if (len >= 2) val = pData[0] | (pData[1] << 8); break;
  case RT::un32: if (len >= 4) val = pData[0] | (pData[1] << 8) | (pData[2] << 16) | (pData[3] << 24); break;
  case RT::sn8: if (len >= 1) val = static_cast<int8_t>(pData[0]); break;
  case RT::sn16: if (len >= 2) val = static_cast<int16_t>(pData[0] | (pData[1] << 8)); break;
  case RT::sn32: if (len >= 4) val = static_cast<int32_t>(pData[0] | (pData[1] << 8) | (pData[2] << 16) | (pData[3] << 24)); break;
  default: return val;
  }
  if (0. != def.scale) val *= def.scale;
  return val;
}

}