#ifndef VEDIRECT_HPP
#define VEDIRECT_HPP

#include "LockFreeStringQueue.h"
#include "VeDirectParameters.h"
#include "VeDirectRegister.h"
#include <StringSplitter.h>
#include <list>
#include <string>
#include <sstream>
#include <regex>

#define SUPPORT_VEDIRECT_BLOCKS

class VeDirect
{
public:
  using HookFunction = std::function<void(const String& key, const String& value)>;

  void Init();
  void SetOnChangeHook(HookFunction f) { mOnChange = f; }
  void SetOnDataHook(HookFunction f) { mOnData = f; }
  void ReadLog(const std::string& log);

private:
  struct Parameter
  {
    const VeDirectParameter* pDef;
    String value;
  };

  struct Packet
  {
    std::list<Parameter> parameters;
  };

  static void ReadTask(void* pInstance);
  static void ParseTask(void* pInstance);

  bool AddStringParameter(const String& s);
  bool AddHexParameter(const String& s);
  bool ProcessParameter();
  bool ProcessHexParameter(const String& line);

  HookFunction mOnChange{ nullptr };
  HookFunction mOnData{ nullptr };
  TaskHandle_t mReadTask{ nullptr };
  portMUX_TYPE mLinesMutex;
  LockFreeStringQueue mLines;
#ifdef SUPPORT_VEDIRECT_BLOCKS
  TaskHandle_t mParseTask{ nullptr };
  portMUX_TYPE mPacketsMutex;
  LockFreeStringQueue mPackets;
#endif // SUPPORT_VEDIRECT_BLOCKS
};

void VeDirect::Init()
{
  mLinesMutex = portMUX_INITIALIZER_UNLOCKED;
  // handler, name, size, instance, priority, (out) handle
  xTaskCreate(VeDirect::ReadTask,  "ReadTask",  10000,  this, configMAX_PRIORITIES - 3,  &mReadTask);

#ifdef SUPPORT_VEDIRECT_BLOCKS
  mPacketsMutex = portMUX_INITIALIZER_UNLOCKED;
  xTaskCreate(VeDirect::ParseTask,  "ParseTask",  10000,  this, 2, &mParseTask);
#endif // SUPPORT_VEDIRECT_BLOCKS
}

uint8_t HexCharsToByte(char hi, char lo)
{
  uint8_t high = (hi >= '0' && hi <= '9') ? hi - '0' :
                 (hi >= 'A' && hi <= 'F') ? hi - 'A' + 10 :
                 (hi >= 'a' && hi <= 'f') ? hi - 'a' + 10 : 0;
  uint8_t low  = (lo >= '0' && lo <= '9') ? lo - '0' :
                 (lo >= 'A' && lo <= 'F') ? lo - 'A' + 10 :
                 (lo >= 'a' && lo <= 'f') ? lo - 'a' + 10 : 0;
  return (high << 4) | low;
}

void VeDirect::ReadTask(void* pInstance)
{
  log_d("ReadTask");
  auto pVeDirect = static_cast<VeDirect*>(pInstance);
  Serial1.begin(19200, SERIAL_8N1, VEDIRECT_RX, VEDIRECT_TX);
  String asciiBuf;
  String hexBuf;
  String nibbleBuf;
  int hexExpectedBytes = 0;
  uint8_t hexChecksum = 0u;
  uint8_t hexCount = 0u;
  

  for (;;) 
  {
    while (Serial1.available()) 
    {
      auto c = static_cast<char>(Serial1.read());

      if (hexExpectedBytes == 0)
      {
        if (('\n' == c) || ('\r' == c))
        {
          if (!asciiBuf.isEmpty())
          {
            pVeDirect->AddStringParameter(asciiBuf);
            asciiBuf.clear();
          }
        }
        else if (asciiBuf.isEmpty() && (':' == c))
        {
          // HEX start detected
          hexBuf = ":";
          hexExpectedBytes = -1;
          hexChecksum = 0;
        }
        else
        {
          asciiBuf += c;
        }
      }

      // HEX Mode
      else
      {
        // auto ParserResetHex = [&]()
        // {
        //   inHex = false;
        //   hexCount = 0;
        //   hexExpected = -1;
        //   hexChecksum = 0;
        // };

        hexBuf += c;

        // HEX mode: decode nibble stream into bytes
        static int nibble = -1;

        int val = -1;
        if (c >= '0' && c <= '9') val = c - '0';
        else if (c >= 'A' && c <= 'F') val = 10 + (c - 'A');
        else if (c >= 'a' && c <= 'f') val = 10 + (c - 'a');
        else continue;  // ignore whitespace etc.

        if (nibble < 0)
        {
          nibble = val;
        }
        else
        {
          uint8_t byteVal = (nibble << 4) | val;
          nibble = -1;

          hexCount++;
          hexChecksum += byteVal;

          if ((3 == hexCount) && (0 > hexExpectedBytes))
          {
            hexExpectedBytes = 3 + byteVal + 1; // ID+LEN+Payload+Checksum
          }

          if ((0 < hexExpectedBytes) && (hexCount >= hexExpectedBytes))
          {
            if (hexChecksum == 0)
            {
              pVeDirect->AddStringParameter(hexBuf);
            }
            hexBuf.clear();
            hexExpectedBytes = 0u; // back to ASCII
            hexChecksum = 0u;
            hexCount = 0u;
          }
        }

        // If field set, wait for first 3-Bytes ID+LEN
        if ((-1 == hexExpectedBytes) && (6 <= hexBuf.length()))
        {
          uint8_t lenByte = HexCharsToByte(hexBuf[4], hexBuf[5]);
          hexExpectedBytes = static_cast<int>(lenByte);
        }

        // Check, if HEX-frame receive completed
        if ((0 < hexExpectedBytes) && (hexBuf.length() >= (hexExpectedBytes*2 + 6)))
        {
          pVeDirect->AddStringParameter(hexBuf);
          hexBuf.clear();
          hexExpectedBytes = 0; // back to ASCII
        }
      }
    }
    vTaskDelay(pdMS_TO_TICKS(1)); // clean sleep
  }
}

bool VeDirect::AddStringParameter(const String& s)
{
  taskENTER_CRITICAL(&mLinesMutex);
  auto result = mLines.enqueue(s);
  taskEXIT_CRITICAL(&mLinesMutex);
  if (!result) log_e("Queue voll! skip: \"%s\"", s.c_str());
  return result;
}

bool VeDirect::AddHexParameter(const String& s)
{
  //sip TODO
  return true;
}

void VeDirect::ParseTask(void* pInstance)
{
  log_d("ReadTask");
  auto pVeDirect = static_cast<VeDirect*>(pInstance);
  for(;;)
  {
    // process all available parameter from buffer
    while (pVeDirect->ProcessParameter()) yield();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

bool VeDirect::ProcessParameter()
{
  String s;
  taskENTER_CRITICAL(&mLinesMutex);
  auto result = mLines.dequeue(s);
  taskEXIT_CRITICAL(&mLinesMutex);
  if (!result) return false;

  if (s.isEmpty()) return true;
  if (':' == s[0]) return ProcessHexParameter(s);

  StringSplitter sp = StringSplitter(s, '\t', 2);
  if (sp.getItemCount() != 2)
  {
    if ((sp.getItemCount() != 1) || (sp.getItemAtIndex(0) != "Checksum"))
    {
      log_e("Receviced data not correct. Item count %u: \"%s\"", sp.getItemCount(), s.c_str());
    }
    return true;
  }

  auto key = sp.getItemAtIndex(0);
  if (key == "Checksum")
  {
#ifdef SUPPORT_VEDIRECT_BLOCKS
    //??
#endif // SUPPORT_VEDIRECT_BLOCKS
    return true;
  }
  auto value = sp.getItemAtIndex(1);
  auto it = parameterMap.find(key);

  if (parameterMap.end() == it)
  {
    log_e("Receviced unknown parameter: \"%s\" = %s", key.c_str(), value.c_str());
    return true;
  }

  auto& param = it->second;
  auto& topic = param.mqttPath;
  log_d("ProcessParameter \"%s\" = %s", key.c_str(), value.c_str());
  if (nullptr != mOnData) mOnData(topic, value);
  if (value != param.lastValue)
  {
    param.lastValue = value;
    if (nullptr != mOnChange) mOnChange(topic, value);
  }
  return true;
}

const VRegDef* LookupReg(uint16_t id)
{
  for (auto* v = vregDefs; v->id != 0; ++v)
  {
    if (v->id == id) return v;
  }
  return nullptr;
}

bool VeDirect::ProcessHexParameter(const String& line)
{
  // if (!line.startsWith(":")) return;
  uint16_t id = strtol(line.substring(1,5).c_str(), nullptr, 16);
  //uint8_t len = strtol(line.substring(5,7).c_str(), nullptr, 16);
  String data = line.substring(7);

  auto v = LookupReg(id);
  if (!v)
  {
    Serial.printf("Unknown VREG 0x%04X raw=%s\n", id, data.c_str());
    return true;
  }

  long value = strtol(data.c_str(), nullptr, 16);
  float scaled = value * v->scale;
  Serial.printf("%s = %.2f %s\n", v->name, scaled, v->unit);
  return false; //TODO
}

void VeDirect::ReadLog(const std::string& log)
{
  log_d("VeDirect ReadLog");
  std::istringstream stream(log);
  std::string line;

  while (std::getline(stream, line))
  {
    //line.trim(); // remove CR, LF, Whitespace
    if (line.length() > 0)
    {
      log_d("VeDirect ReadLog line: %s", line.c_str());
      line = std::regex_replace(line, std::regex(" +"), "\t");
      AddStringParameter(line.c_str());
      vTaskDelay(pdMS_TO_TICKS(1));
    }
  }
}

#endif // VEDIRECT_HPP
