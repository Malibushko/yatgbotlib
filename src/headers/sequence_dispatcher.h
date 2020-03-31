#pragma once
#include "utility/traits.h"

namespace telegram {

template <class Event>
using checker_signature = typename traits::checked_callback<Event>::checker;

template <class Event, class Check = checker_signature<Event>>
class Sequence : public std::enable_shared_from_this<Sequence<Event>> {
  std::vector<std::pair<Event, std::optional<Check>>> transitions;
  std::optional<Check> commonCheck;
  size_t m_currentStep{0};

public:
  Sequence() noexcept {}
  Sequence(const Sequence &) = default;
  Sequence(Sequence &&) = default;
  Sequence &operator=(Sequence &&) = default;
  Sequence &operator=(const Sequence &) = default;

  Sequence(std::initializer_list<Event> &&trans) : transitions{trans} {}
  auto addTransition(const Event &step,
                      const std::optional<Check> &check = {}) {
    transitions.emplace_back(step, check);
    return this->shared_from_this();
  }
  Event &currentStep() const { return transitions[m_currentStep]; }

  template <class Arg> void input(Arg &&arg) {
    static_assert(std::is_invocable_v<Event, Arg>,
                  "Not applicable function arguments");
    if (!finished()) {
      if (transitions[m_currentStep].second) {
        if (!std::invoke(transitions[m_currentStep].second.value(),
                         std::forward<Arg>(arg)))
          return;
      }
      if (commonCheck) {
        if (!std::invoke(commonCheck.value(), std::forward<Arg>(arg))) {
          return;
        }
      }
      std::invoke(transitions[m_currentStep++].first, std::forward<Arg>(arg));
    }
    return;
  }
  bool finished() const noexcept { return m_currentStep == transitions.size(); }
  auto addCheck(const Check &e) {
    if (transitions.size())
      transitions.back().second = e;
    return this->shared_from_this();
  }
  void addCommonCheck(const Check &e) { commonCheck = e; }
  void setStep(uint32_t val) {
    m_currentStep = std::clamp(val, 0, transitions.size());
  }
  void stepBack() {
    if (--m_currentStep)
      m_currentStep = 0;
  }
  void finish() { m_currentStep = transitions.size(); }
  void reset() { m_currentStep = 0; }
};
} // namespace telegram
