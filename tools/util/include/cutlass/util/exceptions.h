/******************************************************************************
 * Copyright (c) 2017 - 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

#pragma once

/**
 * \file
 * \brief C++ exception semantics for CUDA error codes
 */

#include <hip/hip_runtime.h>
#include <iosfwd>
#include <stdexcept>

#include "cutlass/platform/platform.h"

namespace cutlass {

/// C++ exception wrapper for CUDA \p hipError_t
class cuda_exception : public std::exception {
 public:
  /// Constructor
  cuda_exception(const char* msg = "", hipError_t err = hipErrorUnknown) : msg(msg), err(err) {}

  /// Returns the underlying CUDA \p hipError_t
  hipError_t hipError_t() const { return err; }

 protected:
  /// Explanatory string
  const char* msg;

  /// Underlying CUDA \p hipError_t
  hipError_t err;
};

/// Writes a cuda_exception instance to an output stream
inline std::ostream& operator<<(std::ostream& out, cuda_exception const& e) {
  return out << e.what() << ": " << hipGetErrorString(e.hipError_t());
}

}  // namespace cutlass
