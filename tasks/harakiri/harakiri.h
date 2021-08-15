#pragma once

#include <string>
#include <utility>

// Should not allow reuse and yell under sanitizers.
// Fix the interface and implementation.
// AwesomeCallback should add "awesomeness".

class OneTimeCallback {
 public:
  virtual ~OneTimeCallback() { }
  virtual std::string operator()() = 0;
};

// Implement ctor, operator(), maybe something else...
class AwesomeCallback : public OneTimeCallback {
public:
private:
};

