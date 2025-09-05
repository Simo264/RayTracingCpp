#pragma once

#include <optional>

struct QueueFamilyIndices
{
  std::optional<uint32_t> compute_family_index;

  bool isComplete() const
  {
    return compute_family_index.has_value();
  }
};