%module(directors="1") otWrapper

%insert(cgo_comment) %{
#cgo LDFLAGS: -L${SRCDIR}/ -Wl,-rpath,$ORIGIN/pkg -lOtGo
%}

%include "typemaps.i"
%include "stdint.i"

%{
#include <stdexcept>

#include "interface-wrapper.h"
%}

%include "exception.i"

// The %exception directive will catch any exception thrown by the C++ library and
// panic() with the same message.
%exception {
  try {
    $action;
  } catch (std::exception &e) {
    _swig_gopanic(e.what());
  }
}

%include <std_vector.i>
%template(UInt8Vector) std::vector<uint8_t>;

%include "../go/interface-wrapper.h"
