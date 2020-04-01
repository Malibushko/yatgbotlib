#pragma once
#include "utility/traits.h"

namespace telegram {

template <class Event>
using checker_signature = typename traits::checked_callback<Event>::checker;

template <class Event, class Check = checker_signature<Event>>
class Sequence : public std::enable_shared_from_this<Sequence<Event>> {
  using TransitionPair = std::pair<Event, std::optional<Check>>;
  std::vector<TransitionPair> transitions;
  Event exitEvent;
  Event enterEvent;

  std::optional<Check> commonCheck;
  size_t m_currentStep{0};

public:
  using EventType = Event;
  using CheckType = Check;

  Sequence() noexcept;
  Sequence(const Sequence &) = default;
  Sequence(Sequence &&) = default;
  Sequence &operator=(Sequence &&) = default;
  Sequence &operator=(const Sequence &) = default;
  Sequence(std::initializer_list<Event> &&trans);

  template <class Arg> void input(Arg &&arg);

  std::shared_ptr<Sequence<Event, Check>> addCheck(const Check &e);
  std::shared_ptr<Sequence<Event, Check>> addCommonCheck(const Check &e);
  std::shared_ptr<Sequence<Event, Check>>
  addTransition(const Event &step, const std::optional<Check> &check = {});

  void setStep(uint32_t val);
  void stepBack();
  void finish();
  void reset();

  std::shared_ptr<Sequence<Event,Check>> onExit(const Event& e) {
      exitEvent = e;
      return this->shared_from_this();
  }
  std::shared_ptr<Sequence<Event,Check>> onEnter(const Event& e) {
      enterEvent = e;
      return this->shared_from_this();
  }

  std::size_t size() const noexcept {
      return transitions.size();
  }
  bool finished() const noexcept;
};

template <class Event, class Check>
template <class Arg>
void Sequence<Event, Check>::input(Arg &&arg) {
    static_assert(std::is_invocable_v<Event, Arg>,
                  "Not applicable function arguments");
    if (!finished()) {
      if (auto step = transitions[m_currentStep].second;step &&
              !std::invoke(step.value(),std::forward<Arg>(arg))) {
          return;
      }
      if (commonCheck && !std::invoke(commonCheck.value(), std::forward<Arg>(arg))) {
            return;
      }
      if (!m_currentStep && enterEvent)
          enterEvent(std::forward<Arg>(arg));

      std::invoke(transitions[m_currentStep++].first, std::forward<Arg>(arg));

      if (finished() && exitEvent)
          exitEvent(std::forward<Arg>(arg));
    }
    return;
  }


template <class Event, class Check>
Sequence<Event, Check>::Sequence() noexcept {}

template <class Event, class Check>
Sequence<Event, Check>::Sequence(std::initializer_list<Event> &&trans)
    : transitions{trans} {}



template <class Event, class Check>
bool Sequence<Event, Check>::finished() const noexcept {
  return m_currentStep == transitions.size();
}

template <class Event, class Check>
std::shared_ptr<Sequence<Event, Check>>
Sequence<Event, Check>::addCheck(const Check &e) {
  if (transitions.size())
    transitions.back().second = e;
  return this->shared_from_this();
}

template <class Event, class Check>
std::shared_ptr<Sequence<Event, Check>>
Sequence<Event, Check>::addCommonCheck(const Check &e) {
  commonCheck = e;
  return this->shared_from_this();
}

template <class Event, class Check>
std::shared_ptr<Sequence<Event, Check>>
Sequence<Event, Check>::addTransition(const Event &step,
                                      const std::optional<Check> &check) {
  transitions.emplace_back(step, check);
  return this->shared_from_this();
}

template <class Event, class Check>
void Sequence<Event, Check>::setStep(uint32_t val) {
  m_currentStep = std::clamp(val, 0, transitions.size());
}

template <class Event, class Check> void Sequence<Event, Check>::stepBack() {
  if (--m_currentStep)
    m_currentStep = 0;
}

template <class Event, class Check> void Sequence<Event, Check>::finish() {
  m_currentStep = transitions.size();
}

template <class Event, class Check> void Sequence<Event, Check>::reset() {
  m_currentStep = 0;
}
} // namespace telegram
