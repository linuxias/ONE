/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd. All Rights Reserved
 * Copyright 2017 The TensorFlow Authors. All Rights Reserved.
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

#ifndef LUCI_INTERPRETER_PAL_UTILS_H
#define LUCI_INTERPRETER_PAL_UTILS_H

#include <cassert>

namespace luci_interpreter_pal
{

// Table of sigmoid(i/24) at 0.16 format - 256 elements.
// We use combined sigmoid and tanh look-up table, since
// tanh(x) = 2*sigmoid(2*x) -1.
// Both functions are symmetric, so the LUT table is only needed
// for the absolute value of the input.
static const uint16_t sigmoid_table_uint16[256] = {
  32768, 33451, 34133, 34813, 35493, 36169, 36843, 37513, 38180, 38841, 39498, 40149, 40794, 41432,
  42064, 42688, 43304, 43912, 44511, 45102, 45683, 46255, 46817, 47369, 47911, 48443, 48964, 49475,
  49975, 50464, 50942, 51409, 51865, 52311, 52745, 53169, 53581, 53983, 54374, 54755, 55125, 55485,
  55834, 56174, 56503, 56823, 57133, 57433, 57724, 58007, 58280, 58544, 58800, 59048, 59288, 59519,
  59743, 59959, 60168, 60370, 60565, 60753, 60935, 61110, 61279, 61441, 61599, 61750, 61896, 62036,
  62172, 62302, 62428, 62549, 62666, 62778, 62886, 62990, 63090, 63186, 63279, 63368, 63454, 63536,
  63615, 63691, 63765, 63835, 63903, 63968, 64030, 64090, 64148, 64204, 64257, 64308, 64357, 64405,
  64450, 64494, 64536, 64576, 64614, 64652, 64687, 64721, 64754, 64786, 64816, 64845, 64873, 64900,
  64926, 64950, 64974, 64997, 65019, 65039, 65060, 65079, 65097, 65115, 65132, 65149, 65164, 65179,
  65194, 65208, 65221, 65234, 65246, 65258, 65269, 65280, 65291, 65301, 65310, 65319, 65328, 65337,
  65345, 65352, 65360, 65367, 65374, 65381, 65387, 65393, 65399, 65404, 65410, 65415, 65420, 65425,
  65429, 65433, 65438, 65442, 65445, 65449, 65453, 65456, 65459, 65462, 65465, 65468, 65471, 65474,
  65476, 65479, 65481, 65483, 65485, 65488, 65489, 65491, 65493, 65495, 65497, 65498, 65500, 65501,
  65503, 65504, 65505, 65507, 65508, 65509, 65510, 65511, 65512, 65513, 65514, 65515, 65516, 65517,
  65517, 65518, 65519, 65520, 65520, 65521, 65522, 65522, 65523, 65523, 65524, 65524, 65525, 65525,
  65526, 65526, 65526, 65527, 65527, 65528, 65528, 65528, 65529, 65529, 65529, 65529, 65530, 65530,
  65530, 65530, 65531, 65531, 65531, 65531, 65531, 65532, 65532, 65532, 65532, 65532, 65532, 65533,
  65533, 65533, 65533, 65533, 65533, 65533, 65533, 65534, 65534, 65534, 65534, 65534, 65534, 65534,
  65534, 65534, 65534, 65535};

inline std::int32_t saturatingRoundingDoublingHighMul(std::int32_t a, std::int32_t b)
{
  bool overflow = a == b && a == std::numeric_limits<std::int32_t>::min();
  std::int64_t a_64(a);
  std::int64_t b_64(b);
  std::int64_t ab_64 = a_64 * b_64;
  std::int32_t nudge = ab_64 >= 0 ? (1 << 30) : (1 - (1 << 30));
  std::int32_t ab_x2_high32 = static_cast<std::int32_t>((ab_64 + nudge) / (1ll << 31));
  return overflow ? std::numeric_limits<std::int32_t>::max() : ab_x2_high32;
}

// Correctly-rounded-to-nearest division by a power-of-two.
// Also known as a rounding arithmetic right shift.
inline int32_t roundingDivideByPOT(int32_t x, int32_t exponent)
{
  assert(exponent >= 0);
  assert(exponent <= 31);
  const int32_t mask = int32_t((1ll << exponent) - 1);
  const int32_t zero = int32_t(0);
  const int32_t one = int32_t(1);
  const int32_t remainder = x & mask;
  const int32_t threshold = (mask >> 1) + ((x < zero ? one : zero) & one);
  return (x >> exponent) + ((remainder > threshold ? one : zero) & one);
}

inline int32_t multiplyByQuantizedMultiplier(int32_t x, int32_t quantized_multiplier, int shift)
{
  int left_shift = shift > 0 ? shift : 0;
  int right_shift = shift > 0 ? 0 : -shift;
  return roundingDivideByPOT(
    saturatingRoundingDoublingHighMul(x * (1 << left_shift), quantized_multiplier), right_shift);
}

inline int32_t multiplyByQuantizedMultiplierSmallerThanOneExp(int32_t x,
                                                              int32_t quantized_multiplier,
                                                              int left_shift)
{
  return roundingDivideByPOT(saturatingRoundingDoublingHighMul(x, quantized_multiplier),
                             -left_shift);
}

template <typename P> inline void getActivationParams(const P &params, int32_t *min, int32_t *max)
{
  *min = params.quantized_activation_min;
  *max = params.quantized_activation_max;
}

template <typename P> inline void getActivationParams(const P &params, float *min, float *max)
{
  *min = params.float_activation_min;
  *max = params.float_activation_max;
}

template <typename P> inline void getActivationParams(const P &params, int64_t *min, int64_t *max)
{
  *min = params.int64_activation_min;
  *max = params.int64_activation_max;
}

// Gets offset of index if reducing on axis. When reducing, the flattened offset
// will not change, if the input index changes on the given axis. For example,
// if you have a 3D tensor and you are reducing to 2D by eliminating axis 0,
// then index (0, 1, 2) and index (1, 1, 2) will map to the same flattened
// offset.
inline size_t reducedOutputOffset(const int num_dims, const int *dims, const int *index,
                                  const int num_axis, const int *axis)
{
  if (num_dims == 0)
  {
    return 0;
  }
  size_t offset = 0;
  for (int idx = 0; idx < num_dims; ++idx)
  {
    // if we need to skip this axis
    bool is_axis = false;
    if (axis != nullptr)
    {
      for (int axis_idx = 0; axis_idx < num_axis; ++axis_idx)
      {
        if (idx == axis[axis_idx])
        {
          is_axis = true;
          break;
        }
      }
    }
    if (!is_axis)
    {
      offset = offset * static_cast<size_t>(dims[idx]) + static_cast<size_t>(index[idx]);
    }
  }
  return offset;
}

// Gets next index to iterate through a multidimensional array.
inline bool nextIndex(const int num_dims, const int *dims, int *current)
{
  if (num_dims == 0)
  {
    return false;
  }
  int carry = 1;
  for (int idx = num_dims - 1; idx >= 0; --idx)
  {
    int current_val = current[idx] + carry;
    if (dims[idx] == current_val)
    {
      current[idx] = 0;
    }
    else
    {
      current[idx] = current_val;
      carry = 0;
      break;
    }
  }
  return (carry == 0);
}

} // namespace luci_interpreter_pal

#endif // LUCI_INTERPRETER_PAL_UTILS_H
