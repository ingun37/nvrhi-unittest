//
// Created by Ingun Jon on 12/23/25.
//

#ifndef NVRHI_UNIT_TEST_APP_H
#define NVRHI_UNIT_TEST_APP_H
#include <memory>
#include <string>
#include <utility>
#include <scenario/Context.h>
#include <future>
struct Step;
using StepPtr = std::unique_ptr<Step>;
using StepPromise = std::promise<StepPtr>;
using StepFuture = std::future<StepPtr>;

struct Step {
    virtual ~Step() = default;

    const Context& context;
    std::string title;
    std::string prompt;
    std::string defaultInput;

    Step(const Context& webGpu, std::string title, std::string prompt, std::string defaultInput)
        : context(webGpu),
          title(std::move(title)),
          prompt(std::move(prompt)),
          defaultInput(std::move(defaultInput)) {
    }

    Step() = delete;


    virtual StepFuture run(std::string input) =0;
};

template <class T, class... Args> requires std::is_base_of_v<Step, T>
StepFuture create_step_immediately(Args&&... args) {
    StepPromise promise;
    promise.set_value(std::unique_ptr<T>(new T(std::forward<Args>(args)...)));
    return promise.get_future();
}

template <class T> requires std::is_base_of_v<Step, T>
StepFuture create_step_immediately(T&& app) {
    StepPromise promise;
    promise.set_value(std::make_unique<T>(app));
    return promise.get_future();
}

inline StepFuture create_step_immediately(StepPtr&& app) {
    StepPromise promise;
    promise.set_value(std::move(app));
    return promise.get_future();
}

inline StepFuture create_null_step() {
    StepPromise promise;
    promise.set_value(nullptr);
    return promise.get_future();
}

#endif //NVRHI_UNIT_TEST_APP_H