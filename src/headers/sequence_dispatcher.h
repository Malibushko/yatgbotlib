#pragma once
#include "utility/traits.h"

namespace telegram {

template <class Event>
using checker_signature = typename traits::checked_callback<Event>::checker;

template <class Event, class Check = checker_signature<Event>>
class sequence : public std::enable_shared_from_this<sequence<Event>> {
  std::vector<std::pair<Event, std::optional<Check>>> transitions;
  std::optional<Check> common_check;
  size_t currentStep{0};

public:
  sequence() noexcept {}
  sequence(const sequence &) = default;
  sequence(sequence &&) = default;
  sequence &operator=(sequence &&) = default;
  sequence &operator=(const sequence &) = default;

  sequence(std::initializer_list<Event> &&trans) : transitions{trans} {}
  auto add_transition(const Event &step,
                      const std::optional<Check> &check = {}) {
    transitions.emplace_back(step, check);
    return this->shared_from_this();
  }
  Event &current_step() const { return transitions[currentStep]; }

  template <class Arg> void input(Arg &&arg) {
    static_assert(std::is_invocable_v<Event, Arg>,
                  "Not applicable function arguments");
    if (!finished()) {
      if (transitions[currentStep].second) {
        if (!std::invoke(transitions[currentStep].second.value(),
                         std::forward<Arg>(arg)))
          return;
      }
      if (common_check) {
        if (!std::invoke(common_check.value(), std::forward<Arg>(arg))) {
          return;
        }
      }
      std::invoke(transitions[currentStep++].first, std::forward<Arg>(arg));
    }
    return;
  }
  bool finished() const noexcept { return currentStep == transitions.size(); }
  auto add_check(const Check &e) {
    if (transitions.size())
      transitions.back().second = e;
    return this->shared_from_this();
  }
  void set_step(uint32_t val) {
    currentStep = std::clamp(val, 0, transitions.size());
  }
  void step_back() {
    if (--currentStep)
      currentStep = 0;
  }
  void finish() { currentStep = transitions.size(); }
  void reset() { currentStep = 0; }
  void add_check_common(const Check &e) { common_check = e; }
};
} // namespace telegram
