
#pragma once

#include <WiFi.h>
#include <WiFiMulti.h>

#ifdef USE_SSL
#include <WiFiClientSecure.h>
#else
#include <WiFiClient.h>
#endif

WiFiMulti gMultiWiFi;
#ifdef USE_SSL
WiFiClientSecure espClient;
#else
WiFiClient espClient;
#endif

int ap_count = sizeof(gAPs) / sizeof(gAPs[0]);

inline bool NullOrEmpty(const char* s)
{
  return (nullptr == s) || ('\0' == *s );
}

// Set time via NTP, as required for x.509 validation
void setClock()
{
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");  // UTC

  log_d("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2)
  {
    yield();
    delay(500);
    now = time(nullptr);
  }

  log_d("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  log_d("NTP time %s", asctime(&timeinfo));
}

bool startWiFi()
{
  log_d("Number of ap's: %d", ap_count);
  // add all AP's to gMultiWiFi
  for (int i = 0; i < ap_count; i++)
  {
    log_i("Adding AP: %s", gAPs[i].ssid);
    gMultiWiFi.addAP(gAPs[i].ssid, gAPs[i].pw);
  }

  log_i("Connecting to WiFi...");

  auto startAttemptTime = millis();

  while (gMultiWiFi.run() != WL_CONNECTED)
  {
    if (millis() - startAttemptTime > 10000)  // 10s Timeout
    {
      log_e("WiFi connection timeout");
      return false;
    }
    delay(500);
  }

  log_i("Connected to: %s", WiFi.SSID().c_str());
  log_i("IP before static config: %s", WiFi.localIP().toString().c_str());

  // Suche nach passendem AP in gAPs
  for (int i = 0; i < ap_count; i++)
  {
    if ((WiFi.SSID() == gAPs[i].ssid) && !NullOrEmpty(gAPs[i].ip))
    {
      IPAddress ip, gw, mask;
      ip.fromString(gAPs[i].ip);
      gw.fromString(gAPs[i].gw ? gAPs[i].gw : "192.168.1.1");
      mask.fromString(gAPs[i].mask ? gAPs[i].mask : "255.255.255.0");

      WiFi.config(ip, gw, mask);
      log_i("Applied static IP: %s", ip.toString().c_str());
      break;
    }
  }

#ifdef USE_SSL
  //WiFiClientSecure client;
  espClient.setCACert(rootCACertificate);
#endif

  espClient.setTimeout(24);  // Sekunden
  return true;
}

// check if WiFi is still connected 
// if not, try to reconnect
bool checkWiFi()
{
  if ( WiFi.status() != WL_CONNECTED ){
    return startWiFi();
  }
  return true;
}

bool endWiFi()
{
  log_d("Stopping WiFi ... ");
  bool r = WiFi.disconnect();
  delay(1000);
  if (WiFi.status() == WL_CONNECTED)
  {
    log_e("ERROR: unable to disconnect");
    return false;
  }
  log_i("disconnected successfully");
  return true;
}
