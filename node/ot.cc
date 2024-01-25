#include "async-ot-manager.h"

#include <napi.h>

// void logFunction(const char* str) {
//   std::cout << str << std::endl;
// }

// Napi::Boolean runTestClient(const Napi::CallbackInfo& info) {
//   Napi::Env env = info.Env();
//   Logger::init(logFunction);

//   try {
//     auto ot = new OTManager(false, false);
//     ot->connect("127.0.0.1:12345");

//     // choices are bits encoded in bytes, so to have 2 choices we need 1 byte with 2 bits set as choices (little endian)
//     uint8_t choices[1] = {2}; // equivalent to bits "01"
//     auto data = ot->requestData(choices, 2);

//     std::cout << "Received bytes: " << std::setw(2) << std::setfill('0');
//     for (auto i = 0; i < 2 * 16; ++i) {
//       std::cout << (int) data[i] << " ";
//     }
//     std::cout << std::endl;

//     return Napi::Boolean::New(env, true);
//   } catch (const std::exception& e) {
//     std::cout << "Client exception: " << e.what() << std::endl;
//     return Napi::Boolean::New(env, false);
//   }
// }

// Napi::Boolean runTestServer(const Napi::CallbackInfo& info) {
//   Napi::Env env = info.Env();
//   Logger::init(logFunction);

//   try {
//     auto ot = new OTManager(true, false);
//     ot->connect("127.0.0.1:12345");

//     uint8_t data[4*16] = {
//       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//       0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
//       0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb,
//       0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc,
//     };

//     ot->respondWithData(data, 4*16);
//     return Napi::Boolean::New(env, true);
//   } catch (const std::exception& e) {
//     std::cout << "Server exception: " << e.what() << std::endl;
//     return Napi::Boolean::New(env, false);
//   }
// }

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  // exports.Set(Napi::String::New(env, "runTestClient"),
  //             Napi::Function::New(env, runTestClient));

  // exports.Set(Napi::String::New(env, "runTestServer"),
  //             Napi::Function::New(env, runTestServer));

  AsyncOtManager::Init(env, exports);

  return exports;
}

NODE_API_MODULE(addon, Init)
