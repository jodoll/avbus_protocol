#ifndef COMMAND_H
#define COMMAND_H

#include <inttypes.h>
#include <map>
#include <vector>

template <template <class, class, class...> class C, typename K, typename V, typename... Args>
V getOrDefault(const C<K, V, Args...>& m, K const& key, const V& defval) {
  typename C<K, V, Args...>::const_iterator it = m.find(key);
  if (it == m.end()) return defval;
  return it->second;
}

class Device {
 public:
  enum Command { UNDEFINED = 0 };
  enum Code {
    NONE = UNDEFINED,
    TUNER = 0b010110,
    TV = UNDEFINED,  // No signal on bus
    AMP = 0b011001,
    TAPE = 0b100101,
    VCR = 0b100110,
    PHONO = 0b010110,
    CD = 0b010101,
    SYSTEM = 0b101001
  };

  static const Device* getDeviceByName(std::string key);
  virtual const std::string getDeviceName() const;
  virtual const Device::Code getDeviceCode() const;
  const uint16_t operator[](std::string key) const;
  virtual const std::map<std::string, uint16_t> getCommandMap() const;

 private:
  static const std::map<std::string, Device*> deviceByName;
};

class Tuner : public Device {
 public:
  enum Command {
    UNDEFINED = Device::Command::UNDEFINED,
    AM = 0b101001100,
    FM = 0b101001101,
    TUNE_DOWN = 0b101010101,
    TUNE_UP = 0b101010100,
    MEMORY_1 = 0b100101011,
    MEMORY_2 = 0b100101100,
    MEMORY_3 = 0b100101101,
    MEMORY_4 = 0b100110010,
    MEMORY_5 = 0b100110011,
    MEMORY_6 = 0b100110100,
    MEMORY_7 = 0b100110101,
    MEMORY_8 = 0b101001010
  };
  
  const std::string getDeviceName() const override { return "Tuner"; }
  const Device::Code getDeviceCode() const override { return Device::TUNER; }
  const std::map<std::string, uint16_t> getCommandMap() const override { return commandByName; }

 private:
  static const std::map<std::string, uint16_t> commandByName;
};

class Amp : public Device {
 public:
  enum Command {
    UNDEFINED = Device::Command::UNDEFINED,
    VOLUME_DOWN_FRONT = 0b010101100,
    VOLUME_UP_FRONT = 0b010101011,
    VOLUME_DOWN_REAR = 0b101010011,
    VOLUME_UP_REAR = 0b011010011,
    MUTE = 0b011010010,
    INPUT_TUNER = 0b100110011,
    INPUT_PHONO = 0b100110101,
    INPUT_CD = 0b011010100,
    INPUT_TAPE_MON = 0b010110101,
    INPUT_TV = 0b100110100,
    INPUT_VIDEO = 0b101001010,
    INPUT_VCR = 0b101001011,
  };

  const std::string getDeviceName() const override { return "Amp"; }
  const Device::Code getDeviceCode() const override { return Device::AMP; }
  const std::map<std::string, uint16_t> getCommandMap() const override { return commandByName; }

 private:
  static const std::map<std::string, uint16_t> commandByName;
};

class Tv : public Device {
 public:
  enum Command {
    // No signal on bus
  };
  const std::string getDeviceName() const override { return "Tv"; }
  const Device::Code getDeviceCode() const override { return Device::TV; }
  const std::map<std::string, uint16_t> getCommandMap() const override { return commandByName; }

 private:
  static const std::map<std::string, uint16_t> commandByName;
};

class Tape : public Device {
 public:
  enum Command {
    UNDEFINED = Device::Command::UNDEFINED,
    PLAY_RPT = 0b010101011,
    STOP_C = 0b010110010,
    REW = 0b010101100,
    FF = 0b010101101,
    PAUSE = 0b010110011,
    DIRECTION = 0b011001100,
    INDEX_SCAN = 0b010110100,
    QMS = 0b011001101,
    MEMO = 0b011001010,
    REC = 0b011010011,
    REC_MUTE = 0b010110101,
  };

  const std::string getDeviceName() const override { return "Tape"; }
  const Device::Code getDeviceCode() const override { return Device::TAPE; }
  const std::map<std::string, uint16_t> getCommandMap() const override { return commandByName; }

 private:
  static const std::map<std::string, uint16_t> commandByName;
};

class Vcr : public Device {
 public:
  enum Command {
    UNDEFINED = Device::Command::UNDEFINED,
    DIRECTION = 0b011001100,
    MEMO = 0b011001010,
    REC_MUTE = 0b010110101,
  };

  const std::string getDeviceName() const override { return "Vcr"; }
  const Device::Code getDeviceCode() const override { return Device::VCR; }
  const std::map<std::string, uint16_t> getCommandMap() const override { return commandByName; }

 private:
  static const std::map<std::string, uint16_t> commandByName;
};

class Phono : public Device {
  enum Command {
    UNDEFINED = Device::Command::UNDEFINED,
    PLAY = 0b010101011,
    CUT = 0b010110010,
    CUE = 0b010110011,
    REPEAT = 0b011001011,
  };

  const std::string getDeviceName() const override { return "Phono"; }
  const Device::Code getDeviceCode() const override { return Device::PHONO; }
  const std::map<std::string, uint16_t> getCommandMap() const override { return commandByName; }

 private:
  static const std::map<std::string, uint16_t> commandByName;
};

class Cd : public Device {
  enum Command {  //
    UNDEFINED = Device::Command::UNDEFINED,
    PLAY_NEXT = 0b010101011,
    STOP_C = 0b010110010,
    PAUSE = 0b010110011,
    REPEAT = 0b011001011,
  };

  const std::string getDeviceName() const override { return "Cd"; }
  const Device::Code getDeviceCode() const override { return Device::CD; }
  const std::map<std::string, uint16_t> getCommandMap() const override { return commandByName; }

 private:
  static const std::map<std::string, uint16_t> commandByName;
};

class Command {
 public:
  Command(const Device& device, const uint16_t& command);
  ~Command();

  const std::vector<uint16_t> getTimings();

 private:
  Device::Code deviceCode = Device::Code::NONE;
  uint16_t commandCode = Device::Command::UNDEFINED;
  uint16_t combinedCommand = 0;
  std::vector<uint16_t> timings;

  std::vector<uint16_t> calculateTimings();
};

#endif  // COMMAND_H