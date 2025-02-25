/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd. All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __ONERT_BACKEND_TRAIN_OPS_CONVOLUTIONLAYER_H__
#define __ONERT_BACKEND_TRAIN_OPS_CONVOLUTIONLAYER_H__

#include <ops/ConvolutionLayer.h>

#include <exec/train/ITrainableFunction.h>

namespace onert
{
namespace backend
{
namespace train
{
namespace ops
{

class ConvolutionLayer : public ::onert::exec::train::ITrainableFunction,
                         public cpu::ops::ConvolutionLayer
{
public:
  ConvolutionLayer();
  ~ConvolutionLayer();

  void configure(const IPortableTensor *input, const IPortableTensor *kernel,
                 const IPortableTensor *bias, ir::PaddingType _paddingType,
                 const uint32_t paddingLeft, const uint32_t paddingRight, const uint32_t paddingTop,
                 const uint32_t paddingBottom, const uint32_t strideWidth,
                 const uint32_t strideHeight, const uint32_t dilationWidthFactor,
                 const uint32_t dilationHeightFactor, const ir::Activation activation,
                 IPortableTensor *output);
  void forward(bool training) override;
  void backward(uint32_t training_step) override;
};

} // namespace ops
} // namespace train
} // namespace backend
} // namespace onert

#endif // __ONERT_BACKEND_TRAIN_OPS_CONVOLUTIONLAYER_H__
