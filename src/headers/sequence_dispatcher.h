#pragma once
#include "utility/traits.h"

namespace telegram {

/**
 * This class represens sequence of Events
 * Sequences are simillar to DFA (https://en.wikipedia.org/wiki/Deterministic_finite_automaton)
 *
 * Transition is an event that happens then current step is changed
 *
 * Check is a function that must return boolean value that represents if there
 * must be transition to next or not. Every Transition can have its own Check
 *
 * commonCheck is a special Check that is called at every transition
 *
 * Sequence has two special events - exitEvent and enterEvent
 * enterEvent happens BEFORE the first transition call
 * exitEvent happens AFTER the last transition call
 *
 */

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
  /// for other classes
  using EventType = Event;
  using CheckType = Check;

  Sequence() noexcept;
  Sequence(const Sequence &) = default;
  Sequence(Sequence &&) = default;
  Sequence &operator=(Sequence &&) = default;
  Sequence &operator=(const Sequence &) = default;
  Sequence(std::initializer_list<Event> &&trans);

  /// accepts a value and performs (or not performs) transition to next step.
  template <class Arg> void input(Arg &&arg);

  /**
   * Add a check to last transition.
   * Check signature must repeat Transition signature except it must return boolean type.
   * If Check function returns 'false' - current step is not changed and transition Event is not called.
   */
  std::shared_ptr<Sequence<Event, Check>> addCheck(const Check &e);
  /**
    * And special Check that will be called before every Transition
    * CommonCheck is called before transition Check
   */
  std::shared_ptr<Sequence<Event, Check>> addCommonCheck(const Check &e);
  /**
    Add next transition for this sequence
   */
  std::shared_ptr<Sequence<Event, Check>>
  addTransition(const Event &step, const std::optional<Check> &check = {});

  /*  Currently not working

      void setStep(uint32_t val);
      void stepBack();
      void finish();
      void reset();
  */
  /// Set event that will be called after last transition
  std::shared_ptr<Sequence<Event,Check>> onExit(const Event& e) {
      exitEvent = e;
      return this->shared_from_this();
  }
  /// Set event that will be called before first transition
  std::shared_ptr<Sequence<Event,Check>> onEnter(const Event& e) {
      enterEvent = e;
      return this->shared_from_this();
  }
  /// returns the number of transition
  std::size_t size() const noexcept {
      return transitions.size();
  }

  /// check if sequence has reached last transition
  bool finished() const noexcept;
};

template <class Event, class Check>
template <class Arg>
void Sequence<Event, Check>::input(Arg &&arg) {
    static_assert(std::is_invocable_v<Event, Arg>,
                  "Not applicable function arguments");
    if (!finished()) {
        // invoke commonCheck if it present
        if (commonCheck && !std::invoke(commonCheck.value(), std::forward<Arg>(arg))) {
              return;
        }
        // if current transition has Check invoke it
        if (auto step = transitions[m_currentStep].second;step &&
              !std::invoke(step.value(),std::forward<Arg>(arg))) {
          return;
      }
      // call enterEvent if it present
      if (!m_currentStep && enterEvent)
          enterEvent(std::forward<Arg>(arg));
      // invoke transition event
      std::invoke(transitions[m_currentStep++].first, std::forward<Arg>(arg));

      // call exitEvent if it present
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
/* Currently unavailable
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
*/
} // namespace telegram
