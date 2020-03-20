#pragma once
#include <memory>
#include "utility/traits.h"

namespace telegram {

template <class Event>
using seq_signatures = traits::checked_callback<Event>;

template <class Event,class Check = typename seq_signatures<Event>::checker>
class Sequence : public std::enable_shared_from_this<Sequence<Event>> {
    std::vector<std::pair<Event,std::optional<Check>>> transitions;
    std::optional<Check> common_check;
    size_t currentStep {0};
public:
    Sequence() noexcept {}
    Sequence(const Sequence&) = default;
    Sequence(Sequence&&) = default;
    Sequence& operator=(Sequence&&) = default;
    Sequence& operator=(const Sequence&) = default;

    Sequence(std::initializer_list<Event>&& trans) : transitions{trans} {
    }
    auto addTransition(const Event& step,const std::optional<Check>& check = {}) {
        transitions.emplace_back(step,check);
        return this->shared_from_this();
    }
    Event& getCurrentStep() const {
        return transitions[currentStep];
    }
    template<class Arg>
    void input(Arg&& arg) {
        static_assert (std::is_invocable_v<Event,Arg>, "Not applicable DSM function arguments");
        if (!isFinished()) {
            std::cout << __func__ << " " << this << std::endl;
            if (transitions[currentStep].second) {
                if (!std::invoke(transitions[currentStep].second.value(),std::forward<Arg>(arg)))
                    return;
            }
            if (common_check) {
                if (!std::invoke(common_check.value(),std::forward<Arg>(arg))) {
                    return;
                }
            }
             std::invoke(transitions[currentStep++].first,std::forward<Arg>(arg));
        }
        return;
    }
    bool isFinished() const {
        return currentStep == transitions.size();
    }
    auto addCheck(const Check& e) {
        if (transitions.size())
            transitions.back().second = e;
        return this->shared_from_this();
    }
    void setStep(uint32_t val) {
        currentStep = std::clamp(val,0,transitions.size());
    }
    void stepBack() {
        if (--currentStep)
            currentStep = 0;
    }
    void finish() {
        std::cout << __func__ << " " << this << std::endl;
        currentStep = transitions.size();
    }
    void reset() {
        currentStep = 0;
    }
    void addCommonCheck(const Check& e) {
        common_check = e;
    }
};
}
