/*
   VE.Direct config file.
   Cconfiguration parameters for
   VE.Direct2MQTT gateway

   GITHUB Link

   MIT License

   Copyright (c) 2020 Ralf Lehmann


   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#ifndef CONFIG_H
#define CONFIG_H
/*
   Defines that activate features like OTA (over the air update)
   or Acitve/Passive mode
*/
// use SSL to connect to MQTT Server or OTA Server
// it is strongly recommended to use SSL if you send any password over the net
// connectiong to MQTT might need a password; the same for OTA
// if you do not have SSL activated on your servers rename it to NO_USE_SSL
#define NO_USE_SSL

// Activate Over The Air Update of firmware
// rename to NO_USE_OTA if you do not have a webserver that can server new firmware
//#define USE_OTA

#define USE_VICTRON_FORMAT

#ifdef USE_SSL
/*
   SSL certificate
   This is good for all let's encrypt certificates for MQTT or OTA servers
*/
/*
  This is lets-encrypt-x3-cross-signed.pem
*/
const char* rootCACertificate = \
                                "-----BEGIN CERTIFICATE-----\n" \
                                "MIIEkjCCA3qgAwIBAgIQCgFBQgAAAVOFc2oLheynCDANBgkqhkiG9w0BAQsFADA/\n" \
                                "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \
                                "DkRTVCBSb290IENBIFgzMB4XDTE2MDMxNzE2NDA0NloXDTIxMDMxNzE2NDA0Nlow\n" \
                                "SjELMAkGA1UEBhMCVVMxFjAUBgNVBAoTDUxldCdzIEVuY3J5cHQxIzAhBgNVBAMT\n" \
                                "GkxldCdzIEVuY3J5cHQgQXV0aG9yaXR5IFgzMIIBIjANBgkqhkiG9w0BAQEFAAOC\n" \
                                "AQ8AMIIBCgKCAQEAnNMM8FrlLke3cl03g7NoYzDq1zUmGSXhvb418XCSL7e4S0EF\n" \
                                "q6meNQhY7LEqxGiHC6PjdeTm86dicbp5gWAf15Gan/PQeGdxyGkOlZHP/uaZ6WA8\n" \
                                "SMx+yk13EiSdRxta67nsHjcAHJyse6cF6s5K671B5TaYucv9bTyWaN8jKkKQDIZ0\n" \
                                "Z8h/pZq4UmEUEz9l6YKHy9v6Dlb2honzhT+Xhq+w3Brvaw2VFn3EK6BlspkENnWA\n" \
                                "a6xK8xuQSXgvopZPKiAlKQTGdMDQMc2PMTiVFrqoM7hD8bEfwzB/onkxEz0tNvjj\n" \
                                "/PIzark5McWvxI0NHWQWM6r6hCm21AvA2H3DkwIDAQABo4IBfTCCAXkwEgYDVR0T\n" \
                                "AQH/BAgwBgEB/wIBADAOBgNVHQ8BAf8EBAMCAYYwfwYIKwYBBQUHAQEEczBxMDIG\n" \
                                "CCsGAQUFBzABhiZodHRwOi8vaXNyZy50cnVzdGlkLm9jc3AuaWRlbnRydXN0LmNv\n" \
                                "bTA7BggrBgEFBQcwAoYvaHR0cDovL2FwcHMuaWRlbnRydXN0LmNvbS9yb290cy9k\n" \
                                "c3Ryb290Y2F4My5wN2MwHwYDVR0jBBgwFoAUxKexpHsscfrb4UuQdf/EFWCFiRAw\n" \
                                "VAYDVR0gBE0wSzAIBgZngQwBAgEwPwYLKwYBBAGC3xMBAQEwMDAuBggrBgEFBQcC\n" \
                                "ARYiaHR0cDovL2Nwcy5yb290LXgxLmxldHNlbmNyeXB0Lm9yZzA8BgNVHR8ENTAz\n" \
                                "MDGgL6AthitodHRwOi8vY3JsLmlkZW50cnVzdC5jb20vRFNUUk9PVENBWDNDUkwu\n" \
                                "Y3JsMB0GA1UdDgQWBBSoSmpjBH3duubRObemRWXv86jsoTANBgkqhkiG9w0BAQsF\n" \
                                "AAOCAQEA3TPXEfNjWDjdGBX7CVW+dla5cEilaUcne8IkCJLxWh9KEik3JHRRHGJo\n" \
                                "uM2VcGfl96S8TihRzZvoroed6ti6WqEBmtzw3Wodatg+VyOeph4EYpr/1wXKtx8/\n" \
                                "wApIvJSwtmVi4MFU5aMqrSDE6ea73Mj2tcMyo5jMd6jmeWUHK8so/joWUoHOUgwu\n" \
                                "X4Po1QYz+3dszkDqMp4fklxBwXRsW10KXzPMTZ+sOPAveyxindmjkW8lGy+QsRlG\n" \
                                "PfZ+G6Z6h7mjem0Y+iWlkYcV4PIWL1iwBi8saCbGS5jN2p8M+X+Q7UNKEkROb3N6\n" \
                                "KOqkqm57TH2H3eDJAkSnh6/DNFu0Qg==\n" \
                                "-----END CERTIFICATE-----\n";

#endif


/*
   WiFi parameters
*/

// WiFi SSID'S and passwords
// the strongest WiFi station will be used
const char* ssid[] = {"Camper", "GOWLAN", "Caravan"};
const char* pw[] = {"1107199104041993", "0404199311071991", "1107199104041993"};

/*
   MQTT parameters
   you can have more than one MQTT server, the first one that answers will have the connection
   it is strongly recommended to use SSL if you send a username and password over the internet
   ATTENTION: use a unique client id to connect to MQTT or you will be kicked out by another device
   using your id
*/
#define MQTT_MAX_RETRIES 3   // maximum retires to reach a MQTT broker
const char* mqtt_server[] = {"192.168.169.227", "192.168.193.231", "192.168.68.223"};
// no SSL ports
const uint16_t mqtt_port[] = {1883, 1883, 1883};
// SSL ports
//const uint16_t mqtt_port[] = {8883, 8883, 8883};
const char* mqtt_clientID[] = {"victronESP32_1", "victronESP32_1", "victronESP32_1"};
const char* mqtt_username[] = {"admin", NULL, NULL};
const char* mqtt_pw[] = {"888888", NULL, NULL};
int mqtt_server_count = sizeof(mqtt_server) / sizeof(mqtt_server[0]);

// this is the MQTT prefix; below that we use the string from VE.Direct
// e.g. /MPPT75-15/PID  for Product ID
// e.g. /N/<VRM ID>/solarcharger/0/Pv/0/V
String MQTT_PREFIX = "N/c0619ab5b2ba/vedirect/0";
String MQTT_PARAMETER = MQTT_PREFIX + "/Parameter"; 

#ifdef USE_OTA
/*
   the binary file to look for
*/
#define SKETCH_NAME "VictronESP32.ino.esp32.bin"  // sketch name and binary
/*
   the URL of he update
   Note: the php script file.php returns a 302 "not modified" if the checksum of the current sketch and the binary
   is the same
   URL:
   http://username:pw@servername/bin/file.php?
*/
const char* ota_server_string[] = {"http://user:pw@serverIP/bin/file.php?", "http://user:pw@serverName/bin/file.php?"};
int ota_server_count = sizeof(ota_server_string) / sizeof(ota_server_string[0]);

/*
   define the wait time between 2 attempts to update the firmware
   300000 = every 5 minutes
*/
int OTA_WAIT_TIME = 300; // in s
time_t last_ota;
#endif

/*
   Software serial parameter
   These are the pins for the VE.Direct connection
   WARNING: if your VE.Direct device uses 5V please use a 1kOhm/2kOhm divider for the receive line
   The sending line does not need any modification. The ESP uses 3.3V and that's it. A 5V device
   should be able to read that voltage as input
*/
/*
//Pinleiste
GND   IO25
IO32  IO33
IO05  IO12
IO34  IO35
IO18  VDD
GND   VDD

UART0 → Standard, verbunden mit USB (TX=GPIO1, RX=GPIO3) → für Upload & Serial Monitor
UART1 → frei verfügbar (Standard-TX=GPIO10, RX=GPIO9, aber kann beliebig gemappt werden)
UART2 → frei verfügbar (Standard-TX=GPIO17, RX=GPIO16, aber kann beliebig gemappt werden)

RS485 auf dem TTGO RS485-CAN Board

Der MAX485-Chip ist mit dem ESP32 verdrahtet:
UART2 TX → GPIO17 (TX2)
UART2 RX → GPIO16 (RX2)
RE/DE (Sendesteuerung) hängt an GPIO21

Der SN65HVD230 CAN Transceiver ist verbunden mit:
TX (CANTX) → GPIO25
RX (CANRX) → GPIO26


Lösung für LILYGO XY_32_CAN_RS485 v1.1:
UART0 (GPIO1/3): bleibt für USB/PC (Programmieren + Logs).
UART1 (GPIO16/17): bleibt für RS485/VE.Bus.
CAN (GPIO25/26): wird über TWAI für VE.Can genutzt.
UART2: kannst du auf zwei freie Pins routen, z. B.: RX=GPIO33, TX=GPIO32
*/

#ifndef VEDIRECT_RX
#define VEDIRECT_RX 33  // connected to TX of the VE.Direct device; ATTENTION divider may be needed, see abowe
#endif
#ifndef VEDIRECT_TX
#define VEDIRECT_TX 32 // connected to RX of the VE:DIRECT device
#endif

/**
  Depending on the DE.Direct device (see VeDirectParameters.h) there will be several Key/Value pairs
  Define the maximum count of key/value pairs
*/
#define MAX_KEY_VALUE_COUNT 30

/**
  Number of Key-Value blocks we can buffer
  MQTT may be slower than one second, especially when we have to reconnect
  this is the number of buffers we can keep
*/
#define MAX_BLOCK_COUNT 8

/**
  Wait time in Loop
  this determines how many frames are send to MQTT
  if wait time is e.g. 10 minutes, we will send only every 10 minutes to MQTT
  Note: only the last incoming block will be send; all previous blocks will be discarded
  Wait time is in seconds
  Waittime of 1 or 0 means every received packet will be transmitted to MQTT
  Packets during OTA or OneWire will be discarded
*/
int VE_WAIT_TIME = 1; // in s

#endif // CONFIG_H
