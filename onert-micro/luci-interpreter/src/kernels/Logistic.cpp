/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd. All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Builders.h"
#include "kernels/Utils.h"
#include "PALLogistic.h"

namespace luci_interpreter
{

void configure_kernel_CircleLogistic(const circle::Operator *cur_op,
                                     BaseRuntimeGraph *runtime_graph)
{
  const auto input_index = cur_op->inputs()->operator[](0);
  const auto output_index = cur_op->outputs()->operator[](0);

  assert(input_index != -1);
  assert(output_index != -1);

  const auto input = runtime_graph->getCircleTensorByIndex(input_index);
  auto output = runtime_graph->getCircleTensorByIndex(output_index);

  assert(input != nullptr);
  assert(output != nullptr);

  LUCI_INTERPRETER_CHECK(Tensor::element_type(input) == Tensor::element_type(output));

#ifndef DIS_QUANT
  if (Tensor::element_type(input) == DataType::U8)
  {
    LUCI_INTERPRETER_CHECK(Tensor::scale(output) == 1. / 256);
  }
#endif // DIS_QUANT
}

void execute_kernel_CircleLogistic(const circle::Operator *cur_op, BaseRuntimeGraph *runtime_graph)
{
  const auto input_index = cur_op->inputs()->operator[](0);
  const auto output_index = cur_op->outputs()->operator[](0);

  assert(input_index != -1);
  assert(output_index != -1);

  const auto input = runtime_graph->getCircleTensorByIndex(input_index);
  auto output = runtime_graph->getCircleTensorByIndex(output_index);

  assert(input != nullptr);
  assert(output != nullptr);

  bool is_inplace = runtime_graph->is_inplace_op(cur_op);

  const uint8_t *input_data = runtime_graph->getDataByTensor(input);
  uint8_t *output_data = runtime_graph->getDataByTensor(output);

  if (is_inplace)
  {
    output_data = const_cast<uint8_t *>(input_data);
  }

  assert(input_data != nullptr);
  assert(output_data != nullptr);

  const int flat_size = kernels::getTensorRuntimeShape(input, runtime_graph).flatSize();

  switch (Tensor::element_type(input))
  {
#ifndef DIS_FLOAT
    case DataType::FLOAT32:
      luci_interpreter_pal::Logistic(flat_size, kernels::getTensorData<float>(input_data),
                                     kernels::getTensorData<float>(output_data));
      break;
#endif // DIS_FLOAT
#ifndef DIS_QUANT
    case DataType::S8:
      luci_interpreter_pal::Logistic(flat_size, kernels::getTensorData<int8_t>(input_data),
                                     Tensor::scale(input), Tensor::zero_point(input),
                                     kernels::getTensorData<int8_t>(output_data),
                                     Tensor::scale(output), Tensor::zero_point(output));
      break;
#endif // DIS_QUANT
    default:
      assert(false && "Unsupported type.");
  }

  if (is_inplace)
  {
    runtime_graph->makeInplaceOperation(input, output);
  }
}

} // namespace luci_interpreter
