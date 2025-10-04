#pragma once

#include "LockFreeStringQueue.h"
#include "VeDirectParameters.h"
#include "VeDirectRegister.h"
#include "VeDirectProt.h"
#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include <mutex>
#include <queue>

#define V_CACHE

class VeDirect
{
public:
  using HookFunction = std::function<void(const std::string& key, const std::string& value)>;

  void Init();
  void Stop();
  void SetOnChangeHook(HookFunction f) { mOnChange = f; }
  void SetOnDataHook(HookFunction f) { mOnData = f; }
  void ReadLog(const std::string& log);

private:
  struct VRegRecord
  {
    uint32_t timestamp{ 0u };
    std::string lastValue;
    const VeDirectProt::VRegDefine* pDef { nullptr };
  };
  using VQueueItem = std::pair<uint32_t, std::string>;
  static uint8_t HexCharsToByte(char hi, char lo);
  static void ReadTask(void* pInstance);
  static void ParseTask(void* pInstance);

  bool ProcessParameter();
  void ProcessHexParameter(const std::string& s, uint32_t timestamp);
  void ProcessStringParameter(const std::string& s, uint32_t timestamp);
  void Enqueue(const std::string& line);
  bool Dequeue(VQueueItem& ev);
  std::vector<std::string> Split(const std::string& s, char delimiter, size_t limit);

  uint64_t mBootOffsetMs{ 0u };
  HookFunction mOnChange{ nullptr };
  HookFunction mOnData{ nullptr };
  TaskHandle_t mReadTask{ nullptr };
  TaskHandle_t mParseTask{ nullptr };
  volatile bool mStopRequested{ false };
  std::mutex mQueueMutex;  // ESP32 FreeRTOS: std::mutex oder portMUX_TYPE
  std::queue<VQueueItem> mQueue;
  std::map<uint16_t, VRegRecord> mRegisters;
};

void VeDirect::Init()
{
  time_t now;
  time(&now);
  mBootOffsetMs = static_cast<int64_t>(now) * 1000 - millis();
  // handler, name, size, instance, priority, (out) handle
  xTaskCreate(VeDirect::ReadTask,  "ReadTask",  10000,  this, configMAX_PRIORITIES - 3,  &mReadTask);
  xTaskCreate(VeDirect::ParseTask,  "ParseTask",  10000,  this, 2, &mParseTask);
}

void VeDirect::Stop()
{
  mStopRequested = true;
  // if (nullptr != mReadTask)
  // {
  //   vTaskDelete(mReadTask);
  //   mReadTask = nullptr;
  // }
  // if (nullptr != mParseTask)
  // {
  //   vTaskDelete(mParseTask);
  //   mParseTask = nullptr;
  // }
}

uint8_t VeDirect::HexCharsToByte(char hi, char lo)
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
  std::string line;
  line.reserve(128);
  auto isAscii = true;
  while (!pVeDirect->mStopRequested) 
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
        if (!line.empty())
        {
          pVeDirect->Enqueue(line);
          line.clear();
          xTaskNotifyGive(pVeDirect->mParseTask);
          //log_d("Stack free: %5d", uxTaskGetStackHighWaterMark(nullptr));
          vTaskDelay(pdMS_TO_TICKS(1)); // clean sleep
        }
        break; 
      case ' ':
      case '\r':
        //skip
        break;
      default:
        line += c;
      }
    }
    vTaskDelay(pdMS_TO_TICKS(1)); // clean sleep
#endif // ONLY_LOGGER
  }
  vTaskDelete(nullptr);
}

void VeDirect::ParseTask(void* pInstance)
{
  log_d("ReadTask");
  auto pVeDirect = static_cast<VeDirect*>(pInstance);
  while (!pVeDirect->mStopRequested) 
  {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    // process all available parameter from buffer
    while (pVeDirect->ProcessParameter()) yield();
    // vTaskDelay(pdMS_TO_TICKS(10));
  }
  vTaskDelete(nullptr);
}

bool VeDirect::ProcessParameter()
{
  VQueueItem ev;
  auto result = Dequeue(ev);
  if (!result) return false;
  uint32_t timestamp = ev.first;
  std::string& s = ev.second;
  if (!s.empty())
  if (':' == s[0]) ProcessHexParameter(s, timestamp);
  else ProcessStringParameter(s, timestamp);
  return true;
}

void VeDirect::ProcessHexParameter(const std::string& s, uint32_t timestamp)
{
  uint8_t cs = 0u;
  std::vector<uint8_t> bytes;
  for (auto idx = 0u; (idx + 1) < s.length(); idx += 2)
  {
    auto b = HexCharsToByte((0u == idx) ? '0' : s[idx], s[idx + 1]);
    cs += b;
    bytes.push_back(b);
  }
  if (bytes.empty()) return;
  if (0x55u != cs)
  {
    log_w("! Hex checksum error, bytes:%u, calculated:%02X, (%s)",
      bytes.size(), cs, s.c_str());
    return;
  }
  auto comm = "";
  switch (bytes[0])
  {
  case static_cast<uint8_t>(VeDirectProt::Command::Async):
  case static_cast<uint8_t>(VeDirectProt::Response::Get):
    comm = (0xAu == bytes[0]) ? "Async" : "Get";
    if (5u > bytes.size())
    {
      log_w("Length error, received:%u, expected >= 5", bytes.size());
      break;
    }
    else
    {
      auto reg = *reinterpret_cast<const uint16_t*>(&bytes[1]);
      auto& rec = mRegisters[reg];

#ifdef V_CACHE
      if (0u == rec.timestamp)
      {
        rec.timestamp = timestamp;
        rec.pDef = VeDirectProt::LookupRegDefs(reg);
        log_i("Register reg:%04X: [%p] %s", reg, rec.pDef, (nullptr != rec.pDef) ? rec.pDef->name : "");
      }
      auto pDef = rec.pDef;
#else // V_CACHE
      auto pDef = VeDirectProt::LookupRegDefs(reg);
#endif // V_CACHE

      std::string value;
      if (nullptr != pDef)
      {
        value = std::string(ValueString(*pDef, &bytes[3u], (bytes.size() - 5u)).c_str());
        log_i("[%s, reg:%04X] %s: (%s) %s", comm, reg, pDef->name, to_string(pDef->type), value.c_str());
      }
      else
      {
        value = s.substr(6u, (bytes.size() << 1) - 10u);
        log_i("Command %s, reg:%04X, data len:%u, %p", comm, reg, bytes.size() - 5u, pDef);
      }

      // auto flags = bytes[bytes.size() - 2u];
      // auto cs = bytes[bytes.size() - 1u];

      //TODO
      std::string topic;

      if (!topic.empty())
      {
        if (nullptr != mOnData) mOnData(topic, value);
        if (value != rec.lastValue)
        {
          rec.lastValue = value;
          if (nullptr != mOnChange) mOnChange(topic, value);
        }
      }
    }
    break;
  case static_cast<uint8_t>(VeDirectProt::Response::Ping):
    //TODO
    break;
  }

/*
How to Use HEX-mode
- Initial Connection: Connect your device to the VE.Direct port. 
- Send a HEX Message: Send a valid HEX message to the device to switch it from Text-mode to HEX-mode. 
- Receive Data: Process the HEX messages received from the device. 
- Switch Back: If no more HEX messages are received for a period, the device will automatically revert to Text-mode. 

#www.victronenergy.com/upload/documents/BlueSolar-HEX-protocol.pdf
*/
}

void VeDirect::ProcessStringParameter(const std::string& s, uint32_t timestamp)
{
  auto sp = Split(s, '\t', 2);
  if (sp.size() != 2)
  {
    if ((sp.size() != 1) || (sp.at(0) != "Checksum"))
    {
      log_e("Receviced data not correct. Item count %u: \"%s\"", sp.size(), s.c_str());
    }
    return;
  }

  auto key = sp.at(0);
  auto value = sp.at(1);
  auto it = parameterMap.find(key);
  if (key == "Checksum")
  {
    //??
    return;
  }

  if (parameterMap.end() == it)
  {
    log_e("Receviced unknown parameter: \"%s\" = %s, (%s)", key.c_str(), value.c_str(), s.c_str());
    return;
  }

  auto& param = it->second;
  auto& topic = param.mqttPath;
  log_d("ProcessStringParameter \"%s\" = %s", key.c_str(), value.c_str());
  if (nullptr != mOnData) mOnData(topic, value);
  if (value != param.lastValue)
  {
    param.lastValue = value;
    if (nullptr != mOnChange) mOnChange(topic, value);
  }
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
      Enqueue(line);
      vTaskDelay(pdMS_TO_TICKS(1));
    }
  }
}

void VeDirect::Enqueue(const std::string& line)
{
  std::lock_guard<std::mutex> lock(mQueueMutex);
  mQueue.push(VQueueItem(millis(), line));
}

bool VeDirect::Dequeue(VQueueItem& ev)
{
  std::lock_guard<std::mutex> lock(mQueueMutex);
  if (mQueue.empty()) return false;
  ev = mQueue.front();
  mQueue.pop();
  return true;
}

std::vector<std::string> VeDirect::Split(const std::string& s, char delimiter, size_t limit)
{
  std::vector<std::string> result;
  size_t start = 0;
  size_t end = 0;
  size_t count = 0;

  while ((end = s.find(delimiter, start)) != std::string::npos)
  {
    if (count + 1 == limit)
    {
      result.push_back(s.substr(start));
      return result;
    }

    result.push_back(s.substr(start, end - start));
    start = end + 1;
    count++;
  }

  result.push_back(s.substr(start));
  return result;
}
