#include "async-ot-manager.h"

#include "logger.h"

Napi::Object AsyncOtManager::Init(Napi::Env env, Napi::Object exports) {
  // This method is used to hook the accessor and method callbacks
  Napi::Function func = DefineClass(env, "OTManager", {
    InstanceMethod<&AsyncOtManager::isConnected>("isConnected", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
    InstanceMethod<&AsyncOtManager::isServer>("isServer", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
    InstanceMethod<&AsyncOtManager::connect>("connect", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
    InstanceMethod<&AsyncOtManager::disconnect>("disconnect", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
    InstanceMethod<&AsyncOtManager::requestData>("requestData", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
    InstanceMethod<&AsyncOtManager::respondWithData>("respondWithData", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
  });

  Napi::FunctionReference* constructor = new Napi::FunctionReference();

  // Create a persistent reference to the class constructor. This will allow
  // a function called on a class prototype and a function
  // called on instance of a class to be distinguished from each other.
  *constructor = Napi::Persistent(func);
  exports.Set("OTManager", func);

  // Store the constructor as the add-on instance data. This will allow this
  // add-on to support multiple instances of itself running on multiple worker
  // threads, as well as multiple instances of itself running in different
  // contexts on the same thread.
  //
  // By default, the value set on the environment here will be destroyed when
  // the add-on is unloaded using the `delete` operator, but it is also
  // possible to supply a custom deleter.
  env.SetInstanceData<Napi::FunctionReference>(constructor);

  return exports;
}

AsyncOtManager::AsyncOtManager(const Napi::CallbackInfo& info) : Napi::ObjectWrap<AsyncOtManager>(info) {
  Napi::Env env = info.Env();

  // read constructor arguments from info
  if (info.Length() != 2) {
    throw Napi::Error::New(env, "OTManager: expected 2 arguments in constructor");
  }

  if (!info[0].IsBoolean()) {
    throw Napi::Error::New(env, "OTManager: expected 1st argument (isServer) in constructor to be boolean");
  }

  if (!info[1].IsBoolean()) {
    throw Napi::Error::New(env, "OTManager: expected 2nd argument (isQuiet) in constructor to be boolean");
  }

  Napi::Boolean isServer = info[0].As<Napi::Boolean>();
  Napi::Boolean isQuiet = info[1].As<Napi::Boolean>();

  if (!isQuiet.Value()) {
    auto logFunction = [](const char* str) { std::cout << "(NATIVE LOG) " << str << std::endl; };
    Logger::init(logFunction);
  }

  m_ot = std::make_shared<OTManager>(isServer.Value(), isQuiet.Value());
}

// bool isConnected()
Napi::Value AsyncOtManager::isConnected(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  return Napi::Boolean::New(env, m_ot->isConnected());
}

// bool isServer()
Napi::Value AsyncOtManager::isServer(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  return Napi::Boolean::New(env, m_ot->isServer());
}

// void connect(const char* address);
Napi::Value AsyncOtManager::connect(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() != 1) {
    throw Napi::Error::New(env, "OTManager::connect(): expected 1 argument (address)");
  }

  Napi::String address = info[0].As<Napi::String>();

  try {
    // TODO: use AsyncWorker/AsyncContext/Promise to make it async
    m_ot->connect(address.Utf8Value().c_str());
  } catch (const std::exception& e) {
    throw Napi::Error::New(env, e.what());
  }

  return env.Null();
}

Napi::Value AsyncOtManager::disconnect(const Napi::CallbackInfo& info) {
  m_ot->disconnect();
  return info.Env().Null();
}

// uint8_t* requestData(uint8_t * const choiceBits, size_t numbBlocks);
Napi::Value AsyncOtManager::requestData(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() != 2) {
    throw Napi::Error::New(env, "OTManager::requestData(): expected 2 arguments (choice bits as Uint8Array and number of blocks as number)");
  }

  Napi::Uint8Array choiceBits = info[0].As<Napi::Uint8Array>();
  Napi::Number numBlocks = info[1].As<Napi::Number>();
  size_t numBytes = numBlocks.Uint32Value() * 16;

  try {
    // TODO: use AsyncWorker/AsyncContext/Promise to make it async
    auto response = m_ot->requestData(choiceBits.Data(), numBlocks.Uint32Value());

    auto buf = Napi::Uint8Array::New(env, numBytes);
    memcpy(buf.ArrayBuffer().Data(), response, numBytes);
    delete[] response;

    return buf;
  } catch (const std::exception& e) {
    throw Napi::Error::New(env, e.what());
  }
}

// void respondWithData(uint8_t * const data, size_t numBytes);
Napi::Value AsyncOtManager::respondWithData(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() != 1) {
    throw Napi::Error::New(env, "OTManager::(): expected 1 argument (data to send as Uint8Array)");
  }

  Napi::Uint8Array data = info[0].As<Napi::Uint8Array>();

  try {
    // TODO: use AsyncWorker/AsyncContext/Promise to make it async
    m_ot->respondWithData(data.Data(), data.ElementLength());

    return env.Null();
  } catch (const std::exception& e) {
    throw Napi::Error::New(env, e.what());
  }
}

