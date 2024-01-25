#ifndef GO_INTERFACE_WRAPPER
#define GO_INTERFACE_WRAPPER

#include "ot-manager.h"
#include "logger.h"

#include <vector>
#include <iostream>

class OTManagerGo : private OTManager {
public:
  explicit OTManagerGo(bool isServer, bool isQuiet = true) : OTManager(isServer, isQuiet) {
    if (!isQuiet) {
      auto logFunction = [](const char* str) { std::cout << "(NATIVE LOG) " << str << std::endl; };
      Logger::init(logFunction);
    }
  };

  void connect(const char* address) { OTManager::connect(address); }

  void disconnect() { OTManager::disconnect(); }

  bool isConnected() const { return OTManager::isConnected(); }

  bool isServer() const { return OTManager::isServer(); }

  std::vector<uint8_t> requestData(std::vector<uint8_t>& choiceBits, size_t numBlocks) {
    auto result = OTManager::requestData(choiceBits.data(), numBlocks);
    std::vector<uint8_t> buf { result, result + (numBlocks * 16) };
    delete[] result;

    return buf;
  }

  void respondWithData(std::vector<uint8_t>& data) {
    OTManager::respondWithData(data.data(), data.size());
  }

  OTManagerGo(const OTManagerGo&) = delete;
  OTManagerGo& operator=(const OTManagerGo&) = delete;
  OTManagerGo(OTManagerGo&& other) = delete;
};

#endif // GO_INTERFACE_WRAPPER
