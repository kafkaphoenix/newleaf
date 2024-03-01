#pragma once

#include "core/application.h"
#include "utils/exception.h"

namespace engine = potatoengine;

extern engine::Application* engine::Create(engine::CLArgs&& args);

int main(int argc, char** argv) {
  try {
    engine::LogManager::init();

    std::vector<const char*> args(argv, argv + argc);
    engine::CLArgs clargs{std::span<const char*>{args}};
    engine::Application* app = engine::Create(std::move(clargs));

    app->run();

    delete app;
  } catch (const engine::EngineException& e) {
    ENGINE_CRITICAL(e.what());
    engine::LogManager::dump_backtrace();
    std::exit(EXIT_FAILURE);
  } catch (const engine::AppException& e) {
    APP_CRITICAL(e.what());
    engine::LogManager::dump_backtrace();
    std::exit(EXIT_FAILURE);
  } catch (const std::exception& e) {
    // unknow source, we assume from the app
    APP_CRITICAL(e.what());
    engine::LogManager::dump_backtrace();
    std::exit(EXIT_FAILURE);
  }
}