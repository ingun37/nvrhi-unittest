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
using AppPromise = std::promise<std::unique_ptr<App> >;
using AppPtr = std::unique_ptr<AppPromise>;
template <typename AppT> requires std::is_base_of_v<App, AppT>
using AppPtrT = std::unique_ptr<std::promise<std::unique_ptr<AppT> > >;

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

template <typename T> requires std::is_base_of_v<App, T>
AppPtr immediate_app(std::unique_ptr<T> app) {
    AppPtr promise = std::make_unique<AppPromise>();
    promise->set_value(std::move(app));
    return promise;
}

inline AppPtr immediate_null_app() {
    AppPtr promise = std::make_unique<AppPromise>();
    promise->set_value(nullptr);
    return promise;
}
#endif //NVRHI_UNIT_TEST_APP_H