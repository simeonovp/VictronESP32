#ifndef VEDIRECTPARAMS_H
#define VEDIRECTPARAMS_H

#ifdef VEDIRECT_DEMO
#include <WiFi.h>
#include <PubSubClient.h>
#endif // VEDIRECT_DEMO
#include <WString.h>
#include <map>

#ifdef VEDIRECT_DEMO
const char* ssid = "DEIN_WIFI";
const char* password = "DEIN_PASS";
const char* mqtt_server = "192.168.193.231"; // Venus IP

WiFiClient espClient;
PubSubClient client(espClient);
#endif // VEDIRECT_DEMO

// Mappingstruktur
struct VeDirectParameter
{
  const char* name;      // Label from VE.Direct (e.g. "PID", "V", "I")
  const char* type;      // "string", "float", "bool"
  float scale;           // Scaling factor
  const char* unit;      // "V", "A", "W", ...
  const char* descr;     // Human-readable description
  const char* mqttPath;  // Path for publishing
  String lastValue;
};

// Label Units Description BMV 60x BMV 70x BMV71x SmartShunt MPPT1 Phoenix Inverter Phoenix Charger Smart BuckBoost
std::map<String, VeDirectParameter> parameterMap =
{
  // --- Allgemein / Geräte-Infos ---
  { "PID",   { "PID",   "string", 1.0f, "",   "Product ID",                           "Device/ProductId" }},
  { "FW",    { "FW",    "string", 1.0f, "",   "Firmware version (16 bit)",            "Device/Firmware" }},
  { "FWE",   { "FWE",   "string", 1.0f, "",   "Firmware version (24 bit)",            "Device/FirmwareExtended" }},
  { "SER#",  { "SER#",  "string", 1.0f, "",   "Serial number",                        "Device/Serial" }},
  { "BMV",   { "BMV",   "string", 1.0f, "",   "Model description (deprecated)",       "Device/Model" }},
  { "MODE",  { "MODE",  "float",  1.0f, "",   "Device mode",                          "Charger/Mode" }},

  // --- DC Seite (Batterie, Kanäle) ---
  { "V",     { "V",     "float",  1.0f, "mV", "Main / channel 1 battery voltage",     "Dc/0/Voltage" }},
  { "V2",    { "V2",    "float",  1.0f, "mV", "Channel 2 battery voltage",            "Dc/1/Voltage" }},
  { "V3",    { "V3",    "float",  1.0f, "mV", "Channel 3 battery voltage",            "Dc/2/Voltage" }},
  { "VS",    { "VS",    "float",  1.0f, "mV", "Auxiliary (starter) voltage",          "Starter/Voltage" }},
  { "VM",    { "VM",    "float",  1.0f, "mV", "Battery bank mid-point voltage",       "Battery/MidVoltage" }},
  { "DM",    { "DM",    "float",  1.0f, "‰",  "Mid-point deviation",                  "Battery/MidDeviation" }},

  { "I",     { "I",     "float",  1.0f, "mA", "Main / channel 1 battery current",     "Dc/0/Current" }},
  { "I2",    { "I2",    "float",  1.0f, "mA", "Channel 2 battery current",            "Dc/1/Current" }},
  { "I3",    { "I3",    "float",  1.0f, "mA", "Channel 3 battery current",            "Dc/2/Current" }},
  { "P",     { "P",     "float",  1.0f, "W",  "Instantaneous power",                  "Dc/0/Power" }},

  // --- PV Site (SmartSolar MPPT) ---
  { "VPV",   { "VPV",   "float",  1.0f, "mV", "Panel voltage",                        "Pv/0/Voltage" }},
  { "PPV",   { "PPV",   "float",  1.0f, "W",  "Panel power",                          "Pv/0/Power" }},

  // --- Load-Output (MPPT with load output) ---
  { "IL",    { "IL",    "float",  1.0f, "mA", "Load current",                         "Load/0/Current" }},
  { "LOAD",  { "LOAD",  "bool",   1.0f, "",   "Load output state (ON/OFF)",           "Load/0/State" }},

  // --- Battery-State (BMV/SmartShunt) ---
  { "T",     { "T",     "float",  0.1f, "°C", "Battery temperature (0.1°C)",          "Battery/Temperature" }},
  { "CE",    { "CE",    "float",  1.0f, "Ah", "Consumed Amp Hours",                   "Battery/ConsumedAh" }},
  { "SOC",   { "SOC",   "float",  0.1f, "‰",  "State of charge (per mille)",          "Battery/Soc" }},
  { "TTG",   { "TTG",   "float",  1.0f, "Min","Time to go (minutes)",                 "Battery/TimeToGo" }},

  // --- Alarm/State ---
  { "Alarm", { "Alarm", "bool",   1.0f, "",   "Alarm condition active",               "Alarms/0/Active" }},
  { "Relay", { "Relay", "bool",   1.0f, "",   "Relay state",                          "Relay/0/State" }},
  { "AR",    { "AR",    "float",  1.0f, "",   "Alarm reason (bitmask)",               "Alarms/0/Reason" }},
  { "OR",    { "OR",    "string", 1.0f, "",   "Off reason (bitmask, hex)",            "OffReason" }},
  { "ERR",   { "ERR",   "float",  1.0f, "",   "Error code",                           "Error/Code" }},
  { "WARN",  { "WARN",  "float",  1.0f, "",   "Warning reason (bitmask)",             "Warning/Reason" }},

  // --- Loader- / Tracker-State (MPPT) ---
  { "CS",    { "CS",    "float",  1.0f, "",   "State of operation (charger state)",   "Charger/State" }},
  { "MPPT",  { "MPPT",  "float",  1.0f, "",   "Tracker operation mode",               "Charger/MpptMode" }},

  // --- History (BMV / DC Monitor / MPPT) ---
  { "H1",    { "H1",    "float",  1.0f, "mAh","Deepest discharge",                    "History/DeepestDischarge" }},
  { "H2",    { "H2",    "float",  1.0f, "mAh","Last discharge",                       "History/LastDischarge" }},
  { "H3",    { "H3",    "float",  1.0f, "mAh","Average discharge",                    "History/AverageDischarge" }},
  { "H4",    { "H4",    "float",  1.0f, "",   "Charge cycles",                        "History/ChargeCycles" }},
  { "H5",    { "H5",    "float",  1.0f, "",   "Full discharges",                      "History/FullDischarges" }},
  { "H6",    { "H6",    "float",  1.0f, "mAh","Cumulative Ah drawn",                  "History/CumulativeAhDrawn" }},
  { "H7",    { "H7",    "float",  1.0f, "mV", "Minimum main voltage",                 "History/MinMainVoltage" }},
  { "H8",    { "H8",    "float",  1.0f, "mV", "Maximum main voltage",                 "History/MaxMainVoltage" }},
  { "H9",    { "H9",    "float",  1.0f, "s",  "Seconds since last full charge",       "History/SecondsSinceFull" }},
  { "H10",   { "H10",   "float",  1.0f, "",   "Automatic synchronizations",           "History/AutoSyncs" }},
  { "H11",   { "H11",   "float",  1.0f, "",   "Low voltage alarms",                   "History/LowVoltAlarms" }},
  { "H12",   { "H12",   "float",  1.0f, "",   "High voltage alarms",                  "History/HighVoltAlarms" }},
  { "H13",   { "H13",   "float",  1.0f, "",   "Low aux voltage alarms",               "History/LowAuxVoltAlarms" }},
  { "H14",   { "H14",   "float",  1.0f, "",   "High aux voltage alarms",              "History/HighAuxVoltAlarms" }},
  { "H15",   { "H15",   "float",  1.0f, "mV", "Minimum aux voltage",                  "History/MinAuxVoltage" }},
  { "H16",   { "H16",   "float",  1.0f, "mV", "Maximum aux voltage",                  "History/MaxAuxVoltage" }},
  { "H17",   { "H17",   "float",  0.01f,"kWh","Discharged energy (BMV) / Produced (DC monitor)", "History/EnergyOutTotal" }},
  { "H18",   { "H18",   "float",  0.01f,"kWh","Charged energy (BMV) / Consumed (DC monitor)",    "History/EnergyInTotal" }},

  // MPPT-specific History:
  { "H19",   { "H19",   "float",  0.01f,"kWh","Yield total (user resettable)",        "History/Solar/YieldTotal" }},
  { "H20",   { "H20",   "float",  0.01f,"kWh","Yield today",                           "History/Solar/YieldToday" }},
  { "H21",   { "H21",   "float",  1.0f, "W",  "Maximum power today",                   "History/Solar/MaxPowerToday" }},
  { "H22",   { "H22",   "float",  0.01f,"kWh","Yield yesterday",                       "History/Solar/YieldYesterday" }},
  { "H23",   { "H23",   "float",  1.0f, "W",  "Maximum power yesterday",               "History/Solar/MaxPowerYesterday" }},
  { "HSDS",  { "HSDS",  "float",  1.0f, "",   "Day sequence (0..364)",                 "History/Solar/DaySequence" }},

  // --- Phoenix Inverter (AC Out) ---
  { "AC_OUT_V",{ "AC_OUT_V","float",0.01f,"V","AC output voltage",                    "Ac/Out/L1/V" }},
  { "AC_OUT_I",{ "AC_OUT_I","float",0.1f, "A","AC output current",                    "Ac/Out/L1/I" }},
  { "AC_OUT_S",{ "AC_OUT_S","float",1.0f, "VA","AC output apparent power",            "Ac/Out/L1/S" }},

  // --- DC Monitor / Phoenix Charger (DC In) ---
  { "MON",     { "MON",     "float",1.0f, "",  "DC monitor mode",                     "Monitor/Mode" }},
  { "DC_IN_V", { "DC_IN_V", "float",0.01f,"V", "DC input voltage",                    "Dc/In/Voltage" }},
  { "DC_IN_I", { "DC_IN_I", "float",0.1f, "A", "DC input current",                    "Dc/In/Current" }},
  { "DC_IN_P", { "DC_IN_P", "float",1.0f, "W", "DC input power",                      "Dc/In/Power" }}
};

/*
TODO:
Bedeutungen von Load/State (VE.Direct LOAD)
Wert (ASCII)	Bedeutung (englisch)	Bedeutung (deutsch)
ON	Load output on	Lastausgang ein
OFF	Load output off	Lastausgang aus

Bedeutungen von Charger/State (VE.Direct „CS“)
Wert	Bedeutung (englisch)	Bedeutung (deutsch)
0	Off	Aus
1	Low power	Niedriglast / Bereitschaft
2	Fault	Fehler
3	Bulk	Konstantstrom (Bulk-Ladung)
4	Absorption	Konstantspannung (Absorption)
5	Float	Erhaltungsladung (Float)
6	Storage	Lagerungsladung
7	Equalize (manual)	Ausgleichsladung (manuell)
8	Inverting (nur MultiPlus/Quattro, bei Ladegeräten i.d.R. nicht genutzt)	Wechselrichter aktiv
9	Power supply	Netzteilmodus
10	Equalize (auto)	Automatische Ausgleichsladung
11	External control	Steuerung von extern

Bedeutungen von Charger/MpptMode (VE.Direct „MPPT“)
Wert	Bedeutung (englisch)	Bedeutung (deutsch)
0	Off	Aus
1	Voltage or current limited	Spannungs- oder Strombegrenzung
2	MPPT active	MPPT arbeitet (Tracking aktiv)

Bedeutungen von Charger/Mode (VE.Direct „MODE“)
Wert	Bedeutung (englisch)	Bedeutung (deutsch)
1	Off	Aus
2	Voltage at battery terminals	Spannung an Batterieanschlüssen messen
3	Battery charger	Batterieladegerät aktiv
4	Ext. control	Externe Steuerung

*/

#ifdef VEDIRECT_DEMO
// ⬇ MQTT verbinden
void reconnect()
{
  while (!client.connected())
  {
    if (client.connect("vedirect-bridge"))
    {
      Serial.println("MQTT verbunden");
    }
    else
    {
      delay(2000);
    }
  }
}

// ⬇ Wert parsen und MQTT publizieren
void processVedirectValue(String key, String rawValue)
{
  auto it = parMap.find(key);
  if (it == parMap.end())
  {
    Serial.println("Unknown parameter: " + key);
    return;
  }

  auto& param = it->second;
  String topic = "N/<portalId>/battery/vedirect/" + param.mqttPath;
  String payload;

  if (param.type == "float")
  {
    float val = rawValue.toFloat() * param.scale;
    payload = "{\"value\":" + String(val, 2) + "}";
  }
  else if (param.type == "int")
  {
    int val = rawValue.toInt();
    payload = "{\"value\":" + String(val) + "}";
  }
  else if (param.type == "bool")
  {
    bool val = (rawValue == "1");
    payload = "{\"value\":" + String(val ? "true" : "false") + "}";
  }
  else if (param.type == "string")
  {
    payload = "{\"value\":\"" + rawValue + "\"}";
  }

  Serial.printf("Publish: %s → %s\n", topic.c_str(), payload.c_str());
  client.publish(topic.c_str(), payload.c_str());
}

void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) delay(500);
  client.setServer(mqtt_server, 1883);

  reconnect();

  // Beispielaufrufe
  processVedirectValue("V", "13050");    // 13.05 V
  processVedirectValue("SOC", "815");    // 81.5%
  processVedirectValue("Relay", "1");    // true
}

void loop()
{
  if (!client.connected()) reconnect();
  client.loop();
}
#endif // VEDIRECT_DEMO

#endif // VEDIRECTPARAMS_H