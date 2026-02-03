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
struct App;
using AppP = std::unique_ptr<App>;
using AppPromise = std::promise<AppP>;
using AppFuture = std::future<AppP>;
using AppPtr = AppFuture;

struct App {
    virtual ~App() = default;

    const Context& context;
    std::string title;
    std::string prompt;
    std::string defaultInput;

    App(const Context& webGpu, std::string title, std::string prompt, std::string defaultInput)
        : context(webGpu),
          title(std::move(title)),
          prompt(std::move(prompt)),
          defaultInput(std::move(defaultInput)) {
    }

    App() = delete;


    virtual AppPtr run(std::string input) =0;
};

template <class T, class... Args> requires std::is_base_of_v<App, T>
AppPtr create_app_immediately(Args&&... args) {
    AppPromise promise;
    promise.set_value(std::unique_ptr<T>(new T(std::forward<Args>(args)...)));
    return promise.get_future();
}

inline AppPtr immediate_null_app() {
    AppPromise promise;
    promise.set_value(nullptr);
    return promise.get_future();
}
#endif //NVRHI_UNIT_TEST_APP_H