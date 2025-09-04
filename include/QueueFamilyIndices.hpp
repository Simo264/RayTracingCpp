#pragma once

#include <optional>

struct QueueFamilyIndices
{
  std::optional<uint32_t> compute_family_index;
  std::optional<uint32_t> graphics_family_index;
  std::optional<uint32_t> present_family_index;

  bool isComplete() const
  {
    return graphics_family_index.has_value() &&
      compute_family_index.has_value() &&
      present_family_index.has_value();
  }
};