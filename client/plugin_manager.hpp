#pragma once

namespace manager::plugins {
  auto load(const char * file) -> bool;
  auto unload(const char * file) -> bool;
}
