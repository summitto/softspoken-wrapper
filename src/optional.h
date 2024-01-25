#ifndef EMP_OT_WRAPPER_OPTIONAL
#define EMP_OT_WRAPPER_OPTIONAL

#include <utility>
#include <initializer_list>
#include <stdexcept>

// Helper type trait to check if a type is callable
template <typename T, typename... Args>
struct is_callable {
private:
  template <typename U>
  static auto test(int) -> decltype(std::declval<U>()(std::declval<Args>()...), std::true_type());

  template <typename>
  static std::false_type test(...);

public:
  static constexpr bool value = decltype(test<T>(0))::value;
};

template <typename T>
class optional {
public:
  optional() : hasValue(false) {}

  optional(T&& value) : hasValue(true), storedValue(std::forward<T>(value)) {}

  optional(std::initializer_list<typename std::remove_reference<T>::type> initList)
    : hasValue(true), storedValue(initList) {}

  void operator=(const T& value) {
    hasValue = true;
    storedValue = value;
  }

  bool has_value() const {
    return hasValue;
  }

  T& value() {
    if (!hasValue) {
      throw std::runtime_error("optional does not contain a value");
    }
    return storedValue;
  }

  const T& value() const {
    if (!hasValue) {
      throw std::runtime_error("optional does not contain a value");
    }
    return storedValue;
  }

  // Enable operator() only if T is callable
  template <typename U = T, typename... Args, typename = std::enable_if<is_callable<U, Args...>::value>>
  void operator()(Args&&... args) {
    if (hasValue) {
      storedValue(std::forward<Args>(args)...);
    }
  }

private:
  bool hasValue;
  T storedValue;
};

#endif // EMP_OT_WRAPPER_OPTIONAL
