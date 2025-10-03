#ifndef VICTRON_MQTT_H
#define VICTRON_MQTT_H

#include <PubSubClient.h>
#include <ArduinoJson.h>

//Windows: mosquitto_sub.exe -h 192.168.169.227 -p 1883 -u admin -P 888888 -t "#" -v

volatile bool mqtt_param_rec = false;    // we received a parameter via MQTT; remove it or it will be received over and over again

PubSubClient victronMQTT(espClient);

bool MQTTReconnect(const char* server, uint16_t port, const char* id, const char* user, const char* pw)
{
  victronMQTT.setServer(server, port);
  // Loop until we're reconnected
  int j = MQTT_MAX_RETRIES;
  while (!victronMQTT.connected() && (j-- > 0))
  {
    log_i("Attempting MQTT connection to server: %s, try: %d", server, j);
    // Attempt to connect
    if (victronMQTT.connect(id, user, pw))
    {
      log_i("connected");
      log_i("Subscribing to: %s", MQTT_PARAMETER.c_str());
      victronMQTT.subscribe(MQTT_PARAMETER.c_str(), 1);
      return true;
    }
    else
    {
      log_e("failed, rc= %d", victronMQTT.state());
      log_e(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  return false;
}

void OnMQTTData(const char* topic, const uint8_t* payload, unsigned int length)
{
  mqtt_param_rec = false;    // did we receive a parameter?
  char s[length + 1];
  strncpy(s, reinterpret_cast<const char*>(payload), length);
  s[length] = '\0';
  log_d("MQTT OnMQTTData received: %s - \"%s\"", topic, s);
  String t = String(topic);
  if (t.equals(MQTT_PARAMETER))
  {
    log_d("MQTT received new parameters");

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, s, length);
    if ( err )
    {
      log_d("ERROR: deserializing MQTT message: %s", err.c_str());
      return;
    }
    JsonObject obj = doc.as<JsonObject>();
#ifdef DEBUG
    for ( JsonPair kv : obj)
    {
      log_d("key: %s, Value %s", kv.key().c_str(), kv.value().as<String>().c_str());
    }
#endif

    JsonVariant sleeptime = obj["VE_WAIT_TIME"];
    if (sleeptime)
    {
      mqtt_param_rec = true; // this will cause the parameter to be erased from queue
      int st = sleeptime.as<int>();
      log_d("Found member VEDIRECT_WAIT_TIME: %d", st);
      if (VE_WAIT_TIME != st)
      {
        VE_WAIT_TIME = st;
        // store in EEPROM
        pref.setInt("VE_WAIT_TIME", st);
      }
    }
#ifdef USE_V_OTA
    JsonVariant ota_sleeptime = obj["OTA_WAIT_TIME"];
    if (ota_sleeptime)
    {
      mqtt_param_rec = true; // this will cause the parameter to be erased from queue
      int st = ota_sleeptime.as<int>();
      log_d("Found member OTA_WAIT_TIME: %d", st);
      if ( OTA_WAIT_TIME != st)
      {
        OTA_WAIT_TIME = st;
        // store in EEPROM
        pref.setInt("OTA_WAIT_TIME", st);
      }
    }
#endif
  }
}

bool MQTTStart()
{
#ifdef USE_SSL
  espClient.setCACert(rootCACertificate);
#endif
  // receive parameter via MQTT
  log_d("MQTT OnMQTTData setting");
  victronMQTT.setCallback(OnMQTTData);
  for (int i = 0; i < mqtt_server_count; i++)
  {
    if (!victronMQTT.connected())
    {
      log_d("Connecting to MQTT server: %s port: %d", mqtt_server[i], mqtt_port[i]);
      if (MQTTReconnect(mqtt_server[i], mqtt_port[i], mqtt_clientID[i], mqtt_username[i], mqtt_pw[i]))
      {
        return true; // done got the server
      }
      else log_w("MQTTReconnect failed");
    }
  }
  return false; // no server available
}

bool MQTTEnd()
{
  victronMQTT.loop();
  log_d("MQTT disconnect");
  victronMQTT.disconnect();
  return true;
}

void MQTTPublish(const std::string& key, const std::string& value)
{
  log_d("MQTTPublish \"%s\" = %s", key.c_str(), value.c_str());
  if (!victronMQTT.connected())
  {
    MQTTStart();
  }
  victronMQTT.loop();
  auto topic = MQTT_PREFIX + "/" + key.c_str();
  topic.replace("#", ""); // # in a topic is a no go for MQTT

  String payload = "{\"value\":";
  payload += value.c_str();
  payload += '}';
  payload.replace("\r\n", "");

  if (victronMQTT.publish(topic.c_str(), payload.c_str()))
  {
    //sip++ log_i("MQTT message sent succesfully: %s: \"%s\"", topic.c_str(), payload.c_str());
  }
  else
  {
    log_e("Sending MQTT message failed: %s: %s", topic.c_str(), payload.c_str());
  }

  if (mqtt_param_rec)
  {
    // avoid loops by sending only if we received a valid parameter
    log_i("Removing parameter from Queue: %s", MQTT_PARAMETER.c_str());
    victronMQTT.publish(MQTT_PARAMETER.c_str(), "", true);
  }
}

bool MQTTSendOPInfo()
{
  if (!victronMQTT.connected())
  {
    MQTTStart();
  }
  victronMQTT.loop();
  String topic = MQTT_PREFIX + "/" + "UTCBootTime";
  if (victronMQTT.publish(topic.c_str(), asctime(localtime(&last_boot))))
  {
    //sip++ log_i("MQTT message sent succesfully: %s: \"%s\"", topic.c_str(), asctime(localtime(&last_boot)));
  }
  else
  {
    log_e("Sending MQTT message failed: %s: %s", topic.c_str(), asctime(localtime(&last_boot)));
  }
  return true;
}

#endif //VICTRON_MQTT_H
