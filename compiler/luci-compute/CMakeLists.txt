nnas_find_package(TensorFlowSource EXACT 2.8.0 QUIET)
nnas_find_package(TensorFlowGEMMLowpSource EXACT 2.8.0 QUIET)
nnas_find_package(TensorFlowRuySource EXACT 2.8.0 QUIET)

if(NOT TensorFlowSource_FOUND)
  message(STATUS "Build luci-compute: FAILED (missing TensorFlowSource 2.8.0)")
  return()
endif(NOT TensorFlowSource_FOUND)

if(NOT TensorFlowGEMMLowpSource_FOUND)
  message(STATUS "Build luci-compute: FAILED (missing TensorFlowGEMMLowpSource 2.8.0)")
  return()
endif(NOT TensorFlowGEMMLowpSource_FOUND)

if(NOT TensorFlowRuySource_FOUND)
  message(STATUS "Build luci-compute: FAILED (missing TensorFlowRuySource 2.8.0)")
  return()
endif(NOT TensorFlowRuySource_FOUND)

add_library(luci_compute INTERFACE)
target_include_directories(luci_compute SYSTEM INTERFACE "${TensorFlowSource_DIR}")
target_include_directories(luci_compute SYSTEM INTERFACE "${TensorFlowGEMMLowpSource_DIR}")
target_include_directories(luci_compute SYSTEM INTERFACE "${TensorFlowRuySource_DIR}")
