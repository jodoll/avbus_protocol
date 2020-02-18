#include "Command.hpp"
#include "Timings.hpp"

const std::map<std::string, Device*> Device::deviceByName = {
    {"NONE", nullptr},       //
    {"TUNER", new Tuner()},  //
    {"TV", new Tv()},        //
    {"AMP", new Amp()},      //
    {"TAPE", new Tape()},    //
    {"VCR", new Vcr()},      //
    {"PHONO", new Phono()},  //
    {"CD", new Cd()},        //
    {"SYSTEM", nullptr},     // TODO
};

const Device* Device::getDeviceByName(std::string key) { return getOrDefault(deviceByName, key, (Device*)nullptr); }

const uint16_t Device::operator[](std::string key) const {
  return getOrDefault(getCommandMap(), key, (uint16_t)Device::Command::UNDEFINED);
}

const std::map<std::string, uint16_t> Tuner::commandByName = {
    {"FM", Tuner::Command::FM},
    {"AM", Tuner::Command::AM},
    {"TUNE_DOWN", Tuner::Command::TUNE_DOWN},
    {"TUNE_UP", Tuner::Command::TUNE_UP},
    {"MEMORY_1", Tuner::Command::MEMORY_1},
    {"MEMORY_2", Tuner::Command::MEMORY_2},
    {"MEMORY_3", Tuner::Command::MEMORY_3},
    {"MEMORY_4", Tuner::Command::MEMORY_4},
    {"MEMORY_5", Tuner::Command::MEMORY_5},
    {"MEMORY_6", Tuner::Command::MEMORY_6},
    {"MEMORY_7", Tuner::Command::MEMORY_7},
    {"MEMORY_8", Tuner::Command::MEMORY_8},
};

const std::map<std::string, uint16_t> Amp::commandByName = {
    {"VOLUME_DOWN_FRONT", Amp::Command::VOLUME_DOWN_FRONT},
    {"VOLUME_UP_FRONT", Amp::Command::VOLUME_UP_FRONT},
    {"VOLUME_DOWN_REAR", Amp::Command::VOLUME_DOWN_REAR},
    {"VOLUME_UP_REAR", Amp::Command::VOLUME_UP_REAR},
    {"MUTE", Amp::Command::MUTE},
    {"INPUT_TUNER", Amp::Command::INPUT_TUNER},
    {"INPUT_PHONO", Amp::Command::INPUT_PHONO},
    {"INPUT_CD", Amp::Command::INPUT_CD},
    {"INPUT_TAPE_MON", Amp::Command::INPUT_TAPE_MON},
    {"INPUT_TV", Amp::Command::INPUT_TV},
    {"INPUT_VIDEO", Amp::Command::INPUT_VIDEO},
    {"INPUT_VCR", Amp::Command::INPUT_VCR},
};

const std::map<std::string, uint16_t> Tv::commandByName = {};

const std::map<std::string, uint16_t> Tape::commandByName = {
    {"PLAY_RPT", Tape::Command::PLAY_RPT},
    {"STOP_C", Tape::Command::STOP_C},
    {"REW", Tape::Command::REW},
    {"FF", Tape::Command::FF},
    {"PAUSE", Tape::Command::PAUSE},
    {"DIRECTION", Tape::Command::DIRECTION},
    {"INDEX_SCAN", Tape::Command::INDEX_SCAN},
    {"QMS", Tape::Command::QMS},
    {"MEMO", Tape::Command::MEMO},
    {"REC", Tape::Command::REC},
    {"REC_MUTE", Tape::Command::REC_MUTE},
};

const std::map<std::string, uint16_t> Vcr::commandByName = {
    {"VCR_DIRECTION", Vcr::Command::DIRECTION},
    {"VCR_MEMO", Vcr::Command::MEMO},
    {"VCR_REC_MUTE", Vcr::Command::REC_MUTE},
};

const std::map<std::string, uint16_t> Phono::commandByName = {
    {"PLAY", Phono::Command::PLAY},
    {"CUT", Phono::Command::CUT},
    {"CUE", Phono::Command::CUE},
    {"REPEAT", Phono::Command::REPEAT},
};

const std::map<std::string, uint16_t> Cd::commandByName = {
    {"PLAY_NEXT", Cd::Command::PLAY_NEXT},
    {"STOP_C", Cd::Command::STOP_C},
    {"PAUSE", Cd::Command::PAUSE},
    {"REPEAT", Cd::Command::REPEAT},
};

Command::Command(const Device& device, const uint16_t& command) {
  deviceCode = device.getDeviceCode();
  commandCode = command;
  combinedCommand = (deviceCode << 9) | command;
  timings = calculateTimings();
}

Command::~Command() {}

const std::vector<uint16_t> Command::getTimings() { return timings; }

std::vector<uint16_t> Command::calculateTimings() {
  std::vector<uint16_t> timings;
  // INIT
  timings.push_back(INIT_DURATION_US);
  // Command
  for (int i = 0; i < 15; i++) {
    bool isHigh = (combinedCommand >> (14 - i)) & 0b1;
    if (isHigh) {
      timings.push_back(HIGH_DURATION_US);
    } else {
      timings.push_back(LOW_DURATION_US);
    }
  }
  // HOLD
  timings.push_back(HOLD_DURATION_US);

  return timings;
}
