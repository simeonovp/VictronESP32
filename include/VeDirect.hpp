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

  bool AddParameter(const String& s);
  bool AddHex(const String& s);
  bool ProcessParameter();

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

void VeDirect::ReadTask(void* pInstance)
{
  log_d("ReadTask");
  auto pVeDirect = static_cast<VeDirect*>(pInstance);
  Serial1.begin(19200, SERIAL_8N1, VEDIRECT_RX, VEDIRECT_TX);
  String line;
  auto isAscii = true;
  for (;;) 
  {
    while (Serial1.available()) 
    {
      auto c = static_cast<char>(Serial1.read());

#ifdef ONLY_LOGGER
      char buf[3];
      if ((' ' > c) || (127 < c))
      {
        sprintf(buf, "\\x%02X", c);
        line += buf;
        if (0x0a == c)
        {
          Serial.println(line);
          line.clear();
        }
      }
      else line += c;
#else // ONLY_LOGGER
      switch (c)
      {
      case '\n':
        //line.trim(); // remove CR, LF, Whitespace
        if (!line.isEmpty())
        {
          if (isAscii) pVeDirect->AddParameter(line);
          else pVeDirect->AddHex(line);
          line.clear();
          //log_d("Stack free: %5d", uxTaskGetStackHighWaterMark(nullptr));
        }
        isAscii = true;
        break; 
      case ' ':
      case '\r':
        //skip
        break;
      case ':':
        if (!line.isEmpty())
        {
          if (isAscii) pVeDirect->AddParameter(line);
          else pVeDirect->AddHex(line);
          line.clear();
        }
        isAscii = false;
        break;
      default:
        line += c;
      }
    }
    vTaskDelay(pdMS_TO_TICKS(1)); // clean sleep
#endif // ONLY_LOGGER
  }
}

bool VeDirect::AddParameter(const String& s)
{
  taskENTER_CRITICAL(&mLinesMutex);
  auto result = mLines.enqueue(s);
  taskEXIT_CRITICAL(&mLinesMutex);
  if (!result) log_e("Queue voll! skip: \"%s\"", s.c_str());
  return result;
}

bool VeDirect::AddHex(const String& s)
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
  auto value = sp.getItemAtIndex(1);
  auto it = parameterMap.find(key);
  if (key == "Checksum")
  {
#ifdef SUPPORT_VEDIRECT_BLOCKS
    //??
#endif // SUPPORT_VEDIRECT_BLOCKS
    return true;
  }

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
      AddParameter(line.c_str());
      vTaskDelay(pdMS_TO_TICKS(1));
    }
  }
}

#endif // VEDIRECT_HPP
