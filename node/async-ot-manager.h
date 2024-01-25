#include <napi.h>

#include "ot-manager.h"

#include <memory>

class AsyncOtManager : public Napi::ObjectWrap<AsyncOtManager> {
public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports); // should be called in the addon Init
  AsyncOtManager(const Napi::CallbackInfo& info); // equivalent to js class constructor

private:
  // bool isConnected()
  Napi::Value isConnected(const Napi::CallbackInfo& info);
  // bool isServer()
  Napi::Value isServer(const Napi::CallbackInfo& info);
  // void connect(const char* address);
  Napi::Value connect(const Napi::CallbackInfo& info);
  // void disconnect();
  Napi::Value disconnect(const Napi::CallbackInfo& info);
  // uint8_t* requestData(uint8_t * const choiceBits, size_t numbBlocks);
  Napi::Value requestData(const Napi::CallbackInfo& info);
  // void respondWithData(uint8_t * const data, size_t numBytes);
  Napi::Value respondWithData(const Napi::CallbackInfo& info);

  std::shared_ptr<OTManager> m_ot;
};
