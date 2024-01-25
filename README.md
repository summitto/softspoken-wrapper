# libOTe Softspoken OT wrapper for Node.js and Golang

## Prerequisites

- Clang 9 / GCC 9
- CMake 3.16
- Node.js 16.14 (for building Node.js addon)
- Swig 4.1 (for building Golang library)

## Build instructions

1. Build and install Release version of Boost 1.77 with -fPIC flag

> Example B2 command: `sudo ./b2 runtime-debugging=off cxxflags=-fPIC --build-type=minimal install`

2. Git checkout submodules: `git submodule update --init --recursive`

3. Apply `libOTe.patch` to libOTe `cd libOTe && git apply ../libOTe.patch`

### Building Node.js addon (and optionally Go wrapper)

1. Run `npm install`
2. Run `npm run configure` - this will configure the CMake project. This can fail if you don't have all the required dependencies or something goes wrong in the configuration
3. Build Node.js addon with `npm run build-node`

**If you want to build Golang wrapper is well, I recommend using the same cmake-js configuration**

4. Build Golang wrapper with `npm run build-go`.

You can execute any cmake-js command using `npx cmake-js <cmake-js command>`. See https://github.com/cmake-js/cmake-js for more information on commands and options

You can test the Node.js addon by running `node client.js` and `node server.js` using 2 different terminals.

### Building Golang wrapper without Node.js

1. Create a build directory `mkdir build` and cd to it
2. Configure with `cmake ..`. **You will not be able to build Node.js addon using this configuration**.
To change the compiler used by CMake, pass `-DCMAKE_CXX_COMPILER=<c++ compiler> -DCMAKE_C_COMPILER=<c compiler>` options to the cmake command.
3. Build Golang wrapper with `make -j$(nproc) libOtGo`

You can test the Golang wrapper by running:
1. Run `go build`

> Running with `go run server.go` doesn't work because it's built in a temporary directory, which messes up Golang wrapper library search path. You
could fix it by passing `LD_LIBRARY_PATH` environment variable before the command and pointing it to `pkg` directory like this: `LD_LIBRARY_PATH=./pkg go run server.go`.

2. Run the resulting executable `./ot-wrapper`. This will start a server first, you can pair it with the Node.js client (see [Node.js addon section](#building-nodejs-addon-and-optionally-go-wrapper)).
Then it will start a client, you can pair with the Node.js server.
