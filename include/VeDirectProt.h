#pragma once
#include <inttypes.h>

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
};

static const VRegDefine RegDefs[] =
{
  // Product information registers
  { 0x0100, "Product Id", 0., RT::un32 }, // ve/product/id
  { 0x0104, "Group Id", 0., RT::un8 }, // ve/product/groupid
  { 0x010A, "Serial number", 0., RT::string }, // ve/product/serial
  { 0x010B, "Model name", 0., RT::string }, // ve/product/model
  { 0x0140, "Capabilities", 0., RT::un32 }, // ve/product/capabilities
  // Generic device control registers
  { 0x0200, "Device mode", 0., RT::un8 }, // ve/device/mode
  { 0x0201, "Device state", 0., RT::un8 }, // ve/device/state
  { 0x0202, "Remote control used", 0., RT::un32 }, // ve/device/remotecontrol
  { 0x0205, "Device off reason", 0., RT::un8 }, // ve/device/remotecontrol
  { 0x0207, "Device off reason", 0., RT::un32 }, // ve/device/offreason2
  // Battery settings registers
  { 0xEDFF, "Batterysafe mode", 0., RT::un8, Unit::none, "0=off, 1=on" },
  { 0xEDFE, "Adaptive mode", 0., RT::un8, Unit::none, "0=off, 1=on" },
  { 0xEDFD, "Automatic equalisation mode", 0., RT::un8, Unit::none, "0=off, 1..250" },
  { 0xEDFC, "Battery bulk time limit", 0.01, RT::un16, Unit::hours },
  { 0xEDFB, "Battery absorption time limit", 0.01, RT::un16, Unit::hours },
  { 0xEDF7, "Battery absorption voltage", 0.01, RT::un16, Unit::V },
  { 0xEDF6, "Battery float voltage", 0.01, RT::un16, Unit::V },
  { 0xEDF4, "Battery equalisation voltage", 0.01, RT::un16, Unit::V },
  { 0xEDF2, "Battery temp. compensation", 0.01, RT::sn16, Unit::mV_K },
  { 0xEDF1, "Battery type", 1., RT::un8, Unit::none, "0xFF = user" },
  { 0xEDF0, "Battery maximum current", 0.1, RT::un16, Unit::A },
  { 0xEDEF, "Battery voltage", 1., RT::un8, Unit::V },
  { 0xEDEC, "Battery temperature", 0.01, RT::un16, Unit::K, "0xFFFF=N/A" },
  { 0xEDEA, "Battery voltage setting", 1., RT::un8, Unit::V },
  { 0xEDE8, "BMS present", 0., RT::un8, Unit::none, "0=no, 1=yes" },
  { 0xEDE7, "Tail current", 0.1, RT::un16 },
  { 0xEDE6, "Low temperature charge current", 0.1, RT::un16, Unit::A, "0xFFFF=use max" },
  { 0xEDE5, "Auto equalise stop on voltage", 0., RT::un8, Unit::none, "0=no, 1=yes" },
  { 0xEDE4, "Equalisation current level", 1., RT::un8, Unit::perc, "(of 0xEDF0)"},
  { 0xEDE3, "Equalisation duration", 0.01, RT::un16, Unit::hours },
  { 0xED2E, "Re-bulk voltage offset", 0.01, RT::un16, Unit::V },
  { 0xEDE0, "Battery low temperature level", 0.01, RT::sn16, Unit::C },
  { 0xEDCA, "Voltage compensation", 0.01, RT::un16, Unit::V },
  // Charger data registers
  { 0xEDEC, "Battery temperature", 0.01, RT::un16, Unit::K },
  { 0xEDDF, "Charger maximum current", 0.1, RT::un16, Unit::A },
  { 0xEDDD, "System yield", 0.01, RT::un32, Unit::kWh },
  { 0xEDDC, "User yield (resettable)", 0.01, RT::un32, Unit::kWh },
  { 0xEDDB, "Charger internal temperature", 0.01, RT::sn16, Unit::C },
  { 0xEDDA, "Charger error code", 0., RT::un8 },
  { 0xEDD7, "Charger current", 0.1, RT::un16, Unit::A },
  { 0xEDD5, "Charger voltage", 0.01, RT::un16, Unit::V },
  { 0xEDD4, "Additional charger state info", 0., RT::un8 },
  { 0xEDD3, "Yield today", 0.01, RT::un16, Unit::kWh }, //RT::un32 for version <= 1.12
  { 0xEDD2, "Maximum power today", 1., RT::un16, Unit::W },
  { 0xEDD1, "Yield yesterday", 0.01, RT::un16, Unit::kWh }, //RT::un32 for version <= 1.12
  { 0xEDD0, "Maximum power yesterday", 1., RT::un16, Unit::W },
  { 0xEDCE, "Voltage settings range", 0., RT::un16 },
  { 0xEDCD, "History version", 0., RT::un8 },
  { 0xEDCC, "Streetlight version", 0., RT::un8 },
  { 0xEDC7, "Equalise current maximum", 1., RT::un8, Unit::perc },
  { 0xEDC6, "Equalise voltage maximum", 0.01, RT::un16, Unit::V },
  { 0x2211, "Adjustable voltage minimum", 0.01, RT::un16, Unit::V },
  { 0x2212, "Adjustable voltage maximum", 0.01, RT::un16, Unit::V },
  // DC channel registers - MPPT RS models only
  { 0xED8B, "Battery ripple voltage", 0.01, RT::un16, Unit::V },
  { 0xED8D, "Battery voltage", 0.01, RT::sn16, Unit::V },
  { 0xED8F, "Battery current", 0.1, RT::sn16, Unit::A },
  // Solar panel data registers
  { 0x0244, "Number of MPPT trackers", 0., RT::un8 },
  { 0xEDBF, "Panel maximum current", 0.1, RT::un16, Unit::A },
  { 0xEDBC, "Panel power", 0.01, RT::un32, Unit::W },
  { 0xEDBB, "Panel voltage", 0.01, RT::un16, Unit::V },
  { 0xEDBD, "Panel current", 0.1, RT::un16, Unit::A },
  { 0xEDB8, "Panel maximum voltage", 0.01, RT::un16, Unit::V },
  { 0xEDB3, "Tracker mode", 0., RT::un8 },
  { 0xEDB2, "Panel starting voltage", 0.01, RT::un16, Unit::V },
  { 0xEDB1, "Panel input resistance", 1., RT::un32, Unit::Ohm },
  // Solar panel data individual MPPT trackers registers - MPPT RS models only
// Panel power (see 0xEDBC) 0xECCC 0xECDC 0xECEC 0xECFC
// Panel voltage (see 0xEDBB) 0xECCB 0xECDB 0xECEB 0xECFB
// Panel current (see 0xEDBD) 0xECCD 0xECDD 0xECED 0xECFD
// Tracker mode (see 0xEDB3) 0xECC3 0xECD3 0xECE3 0xECF3
  // Load output data/settings registers
  { 0xEDAD, "Load current", 0.1, RT::un16, Unit::A },
  { 0xEDAC, "Load offset voltage", 0.01, RT::un8, Unit::V },
  { 0xEDAB, "Load output control", 0., RT::un8 },
  { 0xEDA9, "Load output voltage", 0.01, RT::un16, Unit::V },
  { 0xEDA8, "Load output state", 0., RT::un8 },
  { 0xED9D, "Load switch high level", 0.01, RT::un16, Unit::V },
  { 0xED9C, "Load switch low level", 0.01, RT::un16, Unit::V },
  { 0xED91, "Load output off reason", 0., RT::un8 },
  { 0xED90, "Load AES timer", 1., RT::un16, Unit::min },
  // Relay settings registers
  { 0xEDD9, "Relay operation mode", 0., RT::un8 },
  { 0x0350, "Relay battery low voltage set", 0.01, RT::un16, Unit::V },
  { 0x0351, "Relay battery low voltage clear", 0.01, RT::un16, Unit::V },
  { 0x0352, "Relay battery high voltage set", 0.01, RT::un16, Unit::V },
  { 0x0353, "Relay battery high voltage clear", 0.01, RT::un16, Unit::V },
  { 0xEDBA, "Relay panel high voltage set", 0.01, RT::un16, Unit::V },
  { 0xEDB9, "Relay panel high voltage clear", 0.01, RT::un16, Unit::V },
  { 0x100A, "Relay minimum enabled time", 1., RT::un16, Unit::min },
  // Lighting controller timer
  { 0xEDA0, "Timer event 0", 0., RT::un32 },
  { 0xEDA1, "Timer event 1", 0., RT::un32 },
  { 0xEDA2, "Timer event 2", 0., RT::un32 },
  { 0xEDA3, "Timer event 3", 0., RT::un32 },
  { 0xEDA4, "Timer event 4", 0., RT::un32 },
  { 0xEDA5, "Timer event 5", 0., RT::un32 },
  { 0xEDA7, "Mid-point shift", 1., RT::sn16, Unit::min },
  { 0xED9B, "Gradual dim speed", 1., RT::un8, Unit::sec },
  { 0xED9A, "Panel voltage night", 0.01, RT::un16, Unit::V },
  { 0xED99, "Panel voltage day", 0.01, RT::un16, Unit::V },
  { 0xED96, "Sunset delay", 1., RT::un16, Unit::min },
  { 0xED97, "Sunrise delay", 1., RT::un16, Unit::min },
  { 0xED90, "AES Timer", 1., RT::un16, Unit::min },
  { 0x2030, "Solar activity", 0., RT::un8, Unit::none, "0=dark, 1=light" },
  { 0x2031, "Time-of-day", 1., RT::un16, Unit::min, "0=mid-night" },
  // VE.Direct port functions
  { 0xED9E, "TX Port operation mode", 0., RT::un8 },
  { 0xED98, "RX Port operation mode", 0., RT::un8 },
  // Restore factory defaults
  { 0x0004, "Restore default" },
  // History data
  { 0x1030, "Clear history" },
// 0x104F Total history
// 0x1050 ... Daily history (0x1050=today, 0x1051=yesterday, ...) (*1)
  { 0x1050, "Daily history today", 0., RT::raw, Unit::_hdr },
// 0x106E
// 0x10A0 ... Daily MPPT history (0x10A0=today, 0x10A1=yesterday, ...) (*1, *2)
// 0x10BE
  // Pluggable display settings
  { 0x0400, "Display backlight mode", 0., RT::un8, Unit::none, "(0 = keypress, 1 = on, 2 = auto)" },
  { 0x0401, "Display backlight intensity", 0., RT::un8, Unit::none, "(0 = always off, 1 = on)" },
  { 0x0402, "Display scroll text speed", 0., RT::un8, Unit::none, "(1 = slow, 5 = fast)" },
  { 0x0403, "Display setup lock", 0., RT::un8, Unit::none, "(0 = unlocked, 1 = locked)" },
  { 0x0404, "Display temperature unit", 0., RT::un8, Unit::none, "(0 = Celsius, 1 = Fahrenheit)" },
  // Internal display settings
  { 0x0401, "Display backlight intensity", 0., RT::un8, Unit::none, "(0 = always off, 1 = on)" },
  { 0x0402, "Display scroll text speed", 0., RT::un8, Unit::none, "(1 = slow, 5 = fast)" },
  { 0x0404, "Display temperature unit", 0., RT::un8, Unit::none, "(0 = Celsius, 1 = Fahrenheit)" },
  { 0x0406, "Display contrast", 0., RT::un8 },
  { 0x0408, "Display backlight mode", 0., RT::un8, Unit::none, "(0 = off, 1 = on, 2 = auto)" },
  // Remote control registers
  { 0x2000, "Charge algorithm version", 0., RT::un8 },
  { 0x2001, "Charge voltage set-point", 0.01, RT::un16, Unit::V },
  { 0x2002, "Battery voltage sense", 0.01, RT::un16, Unit::V },
  { 0x2003, "Battery temperature sense", 0.01, RT::sn16, Unit::C },
  { 0x2004, "Remote command", 0., RT::un8 },
  { 0x2007, "Charge state elapsed time", 1., RT::un32, Unit::ms },
  { 0x2008, "Absorption time", 0.01, RT::un16, Unit::hours },
  { 0x2009, "Error code", 0., RT::un8 },
  { 0x200A, "Battery charge current", 0.001, RT::sn32, Unit::A },
  { 0x200B, "Battery idle voltage", 0.01, RT::un16, Unit::V },
  { 0x200C, "Device state", 0., RT::un8 },
  { 0x200D, "Network info", 0., RT::un8 },
  { 0x200E, "Network mode", 0., RT::un8 },
  { 0x200F, "Network status register", 0., RT::un8 },
  { 0x2013, "Total charge current", 0.001, RT::sn32, Unit::A },
  { 0x2014, "Charge current percentage", 1., RT::un8, Unit::perc },
  { 0x2015, "Charge current limit", 0.1, RT::un16, Unit::A },
  { 0x2018, "Manual equalisation pending", 0., RT::un8 },
  { 0x2027, "Total DC input power", 0.01, RT::un32, Unit::W },
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

String HistoryDayRecordString(const uint8_t* pData, size_t len)
{
  static_assert(34u == sizeof(HistoryDayRecord), "Invalid HistoryDayRecord");
  if (sizeof(HistoryDayRecord) > len) return "";
  auto& rec = *reinterpret_cast<const HistoryDayRecord*>(pData);
  String s;
  s += "\n\tYield: "; s += rec.Yield;
  s += "\n\tConsumed: "; s += rec.Consumed;
  s += "\n\tUBatMax: "; s += rec.UBatMax;
  s += "\n\tUBatMin: "; s += rec.UBatMin;
  s += "\n\tErrors: "; s += rec.ErrorDB;
  if (0u != rec.Error0) s += ", "; s += rec.Error0;
  if (0u != rec.Error1) s += ", "; s += rec.Error1;
  if (0u != rec.Error2) s += ", "; s += rec.Error2;
  if (0u != rec.Error3) s += ", "; s += rec.Error3;
  s += "\n\tTimeBulk: "; s += rec.TimeBulk;
  s += "\n\tTimeAbs: "; s += rec.TimeAbs;
  s += "\n\tTimeFloat: "; s += rec.TimeFloat;
  s += "\n\tPowerMax: "; s += rec.PowerMax;
  s += "\n\tBattCurrMax: "; s += rec.BattCurrMax;
  s += "\n\tUPanelMax: "; s += rec.UPanelMax;
  s += "\n\tDaySeqNr: "; s += rec.DaySeqNr;
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

String ValueString(const VRegDefine& def, const uint8_t* pData, size_t len)
{
  auto& regType = def.type;
  switch (regType)
  {
  case RT::un8: return (len < 1) ? "" : String(pData[0]);
  case RT::un16: return (len < 2) ? "" : String(pData[0] | (pData[1] << 8));
  case RT::un32: return (len < 4) ? "" : String(pData[0] | (pData[1] << 8) | (pData[2] << 16) | (pData[3] << 24));
  case RT::sn8: return (len < 1) ? "" : String(static_cast<int8_t>(pData[0]));
  case RT::sn16: return (len < 2) ? "" : String(static_cast<int16_t>(pData[0] | (pData[1] << 8)));
  case RT::sn32: return (len < 4) ? ""
      : String(static_cast<int32_t>(pData[0] | (pData[1] << 8) | (pData[2] << 16) | (pData[3] << 24)));
  case RT::string:
    return String(reinterpret_cast<const char*>(pData), len);
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