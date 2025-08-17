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
  String name;
  String type;
  float scale;
  String unit;
  String mqttPath;  // Topic-Suffix
  String lastValue;
};

// Ve.Direct → MQTT Topic Mapping
std::map<String, VeDirectParameter> parameterMap =
{
  // Elektrical basic data
  {"V",    {"V",    "float", 0.001, "V",   "Dc/0/Voltage"}},
  {"I",    {"I",    "float", 0.001, "A",   "Dc/0/Current"}},
  {"P",    {"P",    "float", 1.0,   "W",   "Dc/0/Power"}},
  {"VPV",  {"VPV",  "float", 0.001, "V",   "Pv/0/Voltage"}},
  {"PPV",  {"PPV",  "float", 1.0,   "W",   "Pv/0/Power"}},
  {"IL",   {"IL",   "float", 0.001, "A",   "Load/0/Current"}},
  {"LOAD", {"LOAD", "bool",  1.0,   "",    "Load/0/State"}},

  // Battery infos
  {"SOC",  {"SOC",  "float", 0.1,   "%",   "Battery/Soc"}},
  {"TTG",  {"TTG",  "float", 60.0,  "s",   "Battery/TimeToGo"}},
  {"AR",   {"AR",   "bool",  1.0,   "",    "Battery/AlarmActive"}},
  {"CE",   {"CE",   "float", 1.0,   "Ah",  "Battery/ConsumedAh"}},
  {"H19",  {"H19",  "float", 0.001, "kWh", "History/ChargedEnergy"}},
  {"H20",  {"H20",  "float", 0.001, "kWh", "History/DischargedEnergy"}},
  {"H21",  {"H21",  "float", 0.001, "kWh", "History/Solar/YieldToday"}},
  {"H22",  {"H22",  "float", 0.001, "kWh", "History/Solar/YieldYesterday"}},

  // Temperatur
  {"T",    {"T",    "float", 0.1,   "°C",  "Battery/Temperature"}},

  // Relais & Status
  {"Relay", {"Relay", "bool", 1.0,  "",    "Relay/0/State"}},
  {"Alarm", {"Alarm", "bool", 1.0,  "",    "Alarms/0/Active"}},
  {"ERR",   {"ERR",   "int",  1.0,  "",    "Error/Code"}},
  {"CS",    {"CS",    "int",  1.0,  "",    "Charger/State"}},
  {"MPPT",  {"MPPT",  "int",  1.0,  "",    "Charger/MpptMode"}},
  {"FW",    {"FW",    "string", 1.0, "",   "Device/Firmware"}},
  {"PID",   {"PID",   "string", 1.0, "",   "Device/ProductId"}},

  // History
  {"H1",   {"H1",   "float", 0.1,   "A",   "History/MaxCurrent"}},
  {"H3",   {"H3",   "float", 0.1,   "A",   "History/MaxDischargeCurrent"}},
  {"H4",   {"H4",   "float", 0.1,   "A",   "History/MaxChargeCurrent"}},

  // Additional infos
  {"SER#", {"SER#", "string", 1.0, "",     "Device/Serial"}},
  {"MODE", {"MODE", "int",    1.0, "",     "Charger/Mode"}},
  {"BS",   {"BS",   "int",    1.0, "",     "Battery/Starter/Detected"}},

  // Unknown
  {"OR",   {"OR",   "string", 1.0,   "",    ""}},
  {"H23",  {"H23",  "float",  0.001, "kWh", "History/Solar/YieldXxx"}},
  {"HSDS", {"HSDS", "float",  0.001, "kWh", "History/Solar/YieldYyy"}},
  // {"OR",   {"OR",   "int",    1.0, "",     "OffReason"}},
  // {"H23",  {"H23",  "int",    1.0, "",     "History/LowVoltageEvents"}},
  // {"HSDS", {"HSDS", "int",    1.0, "",     "HistoryStatus"}},

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
  if (!parameterMap.count(key))
  {
    Serial.println("Unbekannter Parameter: " + key);
    return;
  }

  VeDirectParameter param = parameterMap[key];
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