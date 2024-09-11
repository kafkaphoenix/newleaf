#pragma once

#include <span>
#include <vector>

#include "../utils/exception.h"
#include "application.h"
#include "logging/log_manager.h"

extern nl::Application* nl::Create(nl::CLArgs&& args);

int main(int argc, char** argv) {
  try {
    nl::LogManager::init();

    std::vector<const char*> args(argv, argv + argc);
    nl::CLArgs clargs{std::span<const char*>{args}};
    nl::Application* app = nl::Create(std::move(clargs));

    app->run();

    delete app;
  } catch (const nl::EngineException& e) {
    ENGINE_CRITICAL(e.what());
    nl::LogManager::dump_backtrace();
    std::exit(EXIT_FAILURE);
  } catch (const nl::AppException& e) {
    APP_CRITICAL(e.what());
    nl::LogManager::dump_backtrace();
    std::exit(EXIT_FAILURE);
  } catch (const std::exception& e) {
    // unknow source, we assume from the app
    APP_CRITICAL(e.what());
    nl::LogManager::dump_backtrace();
    std::exit(EXIT_FAILURE);
  }
}