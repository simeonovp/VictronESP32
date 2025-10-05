#pragma once

#include <OneWire.h>
#include <DallasTemperature.h>
#include <string>

#define MAX_DS18SENSORS 3
#define MAX_MEASSUREMENTS 3

OneWire oneWire(ONEWIRE_PIN);
DallasTemperature sensors(&oneWire);
int deviceCount = 0;
bool onewire_good_values = false;

std::string addr2String(DeviceAddress addr)
{
  std::string s;
  for (int i = 0; i < 7; i++)
  {
    s += std::to_string(addr[i]) + ":";
  }
  return s += addr[7];
}

bool meassureOneWire()
{
  log_d("Start");
  int m_count = MAX_MEASSUREMENTS;
  do
  {
    m_count --;
    onewire_good_values = true;
    sensors.begin();
    sensors.setWaitForConversion(true);
    delay(1000);
    sensors.requestTemperatures();
    delay(1000);
    deviceCount = sensors.getDeviceCount();
    log_d("Found %d devices", deviceCount);
    for (int i = 0; i < deviceCount; i++)
    {
      float temp = sensors.getTempCByIndex(i);
      if (() temp > 200 || temp < -80 )
      {
        onewire_good_values = false;
      }
    }
  }
  while (m_count >= 0 && !onewire_good_values);
  log_d("End");
  return true;
}

bool sendOneWireMQTT()
{
  log_d("Start");
  meassureOneWire();

  log_d("Found %d One wire temp sensors", deviceCount);
  float c[deviceCount];

  if ( !onewire_good_values || deviceCount <= 0)
  {
    log_d("No ONE Wire temp sensors found; END");
    return false;
  }

  for (int i = 0; i < deviceCount; i++)
  {
    DeviceAddress addr;
    sensors.getAddress((uint8_t *)&addr, (uint8_t) i);
    c[i] = sensors.getTempCByIndex(i);
    log_d("DS18: %s, Temp: %f", addr2String(addr).c_str(), c[i]);
  }

  if (!victronMQTT.connected())
  {
    MQTTStart();
  }

  StaticJsonDocument<300> doc;
  // Add values in the document
  //
  int count = deviceCount > MAX_DS18SENSORS ? MAX_DS18SENSORS : deviceCount;
  log_d("Sending: %d Devices", count);
  for (uint8_t i = 0u; i < count; i++)
  {
    DeviceAddress addr;
    sensors.getAddress(reinterpret_cast<uint8_t*>(&addr), i);
    auto s = addr2String(addr);
    doc[s] = c[i];
  }
  char s[300];
  serializeJson(doc, s);
  if (victronMQTT.publish(MQTT_ONEWIRE.c_str(), s))
  {
    log_d("Sending OneWire Data: %s - OK", s);
  }
  else
  {
    log_d("Sending OneWire %s Data: %s - ERROR", MQTT_ONEWIRE.c_str(), s);
  }
  victronMQTT.loop();
  if (mqtt_param_rec)
  {
    // avoid loops by sending only if we received a valid parameter
    log_i("Removing parameter from Queue: %s", MQTT_PARAMETER);
    victronMQTT.publish(MQTT_PARAMETER, "", true);
  }
  log_d("End");
  return true;
}
