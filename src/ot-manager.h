#ifndef EMP_OT_WRAPPER_MANAGER
#define EMP_OT_WRAPPER_MANAGER

#include <memory>
#include <mutex>
#include <stdint.h>

#include <cryptoTools/Common/Defines.h>
#include <cryptoTools/Crypto/PRNG.h>
#include <coproto/Socket/AsioSocket.h>
#include <libOTe/TwoChooseOne/SoftSpokenOT/SoftSpokenMalOtExt.h>

class OTManager {
public:
  explicit OTManager(bool isServer, bool isQuiet = true);
  ~OTManager();

  void connect(const char* address);
  void disconnect();

  bool isConnected() const {
    return m_connected;
  }

  bool isServer() const {
    return m_server;
  }

  uint8_t* requestData(uint8_t * const choiceBits, uint32_t numbBlocks);
  void respondWithData(uint8_t * const data, uint32_t numBytes);

  OTManager(const OTManager&) = delete;
  OTManager& operator=(const OTManager&) = delete;
  OTManager(OTManager&& other) = delete;

private:
  coproto::AsioSocket m_io;
  std::shared_ptr<osuCrypto::PRNG> m_prng;
  std::unique_ptr<osuCrypto::SoftSpokenMalOtSender> m_ots;
  std::unique_ptr<osuCrypto::SoftSpokenMalOtReceiver> m_otr;
  std::mutex m_mut;

  bool m_server;
  bool m_quiet;
  bool m_connected = false;
};

#endif // EMP_OT_WRAPPER_MANAGER
