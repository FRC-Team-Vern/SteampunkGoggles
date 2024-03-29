/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>
#include <vector>

namespace frc {

template <typename T>
inline cs::AxisCamera CameraServerCustom::AddAxisCamera(
    std::initializer_list<T> hosts) {
  return AddAxisCamera("Axis Camera", hosts);
}

template <typename T>
inline cs::AxisCamera CameraServerCustom::AddAxisCamera(
    llvm::StringRef name, std::initializer_list<T> hosts) {
  std::vector<std::string> vec;
  vec.reserve(hosts.size());
  for (const auto& host : hosts) vec.emplace_back(host);
  return AddAxisCamera(name, vec);
}

}  // namespace frc
