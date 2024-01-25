#include "ot-manager.h"
#include "logger.h"

#include <cryptoTools/Common/Aligned.h>
#include <cryptoTools/Common/BitVector.h>

#include <array>
#include <sstream>

OTManager::OTManager(bool isServer, bool isQuiet) {
  this->m_server = isServer;
  this->m_quiet = isQuiet;
  this->m_prng = std::make_shared<osuCrypto::PRNG>(osuCrypto::sysRandomSeed());

  this->m_ots = std::make_unique<osuCrypto::SoftSpokenMalOtSender>();
  this->m_otr = std::make_unique<osuCrypto::SoftSpokenMalOtReceiver>();

  if (!this->m_quiet) Logger::log("OTManager: initialized");
}

OTManager::~OTManager() {
  if (!this->m_quiet) Logger::log("OTManager: destroying itself...");

  delete this->m_ots.release();
  delete this->m_otr.release();

  if (this->m_connected) {
    this->disconnect();
  }
}

void OTManager::connect(const char* address) {
  try {
    if (this->m_server) {
      auto acceptor = coproto::AsioAcceptor(address, coproto::global_io_context(), 1);
      acceptor.mAcceptor.set_option(boost::asio::ip::tcp::acceptor::keep_alive(true));
      auto acceptorTask = macoro::make_task(std::move(acceptor));
      this->m_io = macoro::sync_wait(acceptorTask);
    } else {
      auto connector = coproto::AsioConnect(address, coproto::global_io_context());
      auto connectorTask = macoro::make_task(std::move(connector));
      this->m_io = macoro::sync_wait(connectorTask);
    }
    this->m_connected = true;
    this->m_otr->init();
    this->m_ots->init();
    if (!this->m_quiet) Logger::log("OTManager::connect(): created connection for OT");
  } catch (const std::exception& e) {
    if (!this->m_quiet) Logger::log(std::string("OTManager::connect(): failed to start connection for OT: ") + e.what());
    throw e;
  }
}

void OTManager::disconnect() {
  if (!this->m_connected) {
    return;
  }

  if (!this->m_quiet) {
    std::stringstream ss;
    ss << "OTManager stats: sent " << this->m_io.bytesSent() << " bytes, received " << this->m_io.bytesReceived() << " bytes" << std::endl;
    Logger::log(ss.str());
    Logger::log("OTManager::disconnect(): closing connection");
  }
  macoro::sync_wait(this->m_io.flush());
  this->m_io.close();
  this->m_connected = false;
}

uint8_t* OTManager::requestData(uint8_t * const choiceBits, uint32_t numBlocks) {
  // libOTe/SoftSpokenOT cannot OT 1 out of 2 blocks for some reason, it needs at least 2 out of 4 blocks
  if (choiceBits == nullptr || numBlocks < 2) {
    throw std::runtime_error("OTManager::requestData(): empty choice or number of blocks less than 2");
  }

  if (!this->isConnected()) {
    throw std::runtime_error("OTManager::requestData(): requesting data without an established connection, call connect() first");
  }

  if (!this->m_quiet) Logger::log(std::string("OTManager::requestData(): requesting " + std::to_string(numBlocks) + " blocks"));

  // SoftSpoken OT requires memory addresses to be aligned to 32 bytes
  osuCrypto::AlignedVector<osuCrypto::block> recvData { numBlocks };
  osuCrypto::BitVector choice { choiceBits, numBlocks };

  {
    std::lock_guard<std::mutex> lock { this->m_mut };
    coproto::sync_wait(m_otr->receiveChosen(std::move(choice), recvData, *m_prng.get(), m_io));
    coproto::sync_wait(m_io.flush());
  }

  if (!this->m_quiet) Logger::log(std::string("OTManager::requestData(): received " + std::to_string(numBlocks) + " blocks"));

  uint8_t* result = new uint8_t[numBlocks * 16];
  memcpy(result, recvData.data(), numBlocks * 16);
  recvData.clear();

  return result;
}

void OTManager::respondWithData(uint8_t * const data, uint32_t numBytes) {
  // libOTe/SoftSpokenOT cannot OT 1 out of 2 blocks for some reason, it needs at least 2 out of 4 blocks
  if (data == nullptr || numBytes < 4 * 16) {
    throw std::runtime_error("OTManager::respondWithData(): empty choice or number of blocks less than 4");
  }

  if (!this->isConnected()) {
    throw std::runtime_error("OTManager::respondWithData(): sending data without an established connection, call connect() first");
  }

  if (numBytes % 16 != 0) {
    throw std::runtime_error("OTManager::respondWithData(): data for sending must be aligned to 16 bytes");
  }

  // Every block is 16 bytes
  const size_t numBlocks = numBytes / 16;
  // Client selects one of 2 blocks out of a pair, depending on the choice bit
  const size_t tableSize = numBlocks / 2;

  // SoftSpoken OT requires memory addresses to be aligned to 32 bytes
  osuCrypto::AlignedVector<std::array<osuCrypto::block, 2>> sendData { tableSize };

  for (size_t i = 0; i < numBlocks; i += 2) {
    sendData[i/2][0] = osuCrypto::toBlock(data + (i * 16));
    sendData[i/2][1] = osuCrypto::toBlock(data + ((i + 1) * 16));
  }

  {
    std::lock_guard<std::mutex> lock { this->m_mut };
    coproto::sync_wait(m_ots->sendChosen(sendData, *m_prng.get(), m_io));
    coproto::sync_wait(m_io.flush());
  }
  sendData.clear();

  if (!this->m_quiet) Logger::log(std::string("OTManager::respondWithData(): sending " + std::to_string(numBlocks / 2) + " blocks"));
}
