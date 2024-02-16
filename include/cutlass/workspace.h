/***************************************************************************************************
 * Copyright (c) 2023 - 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 **************************************************************************************************/
/*! \file
    \brief Utilities for initializing workspaces
*/

/*
  Note:  CUTLASS 3x increases the host compiler requirements to C++17. However, certain
         existing integrations of CUTLASS require C++11 host compilers.

         Until this requirement can be lifted, certain headers with this annotation are required
         to be remain consistent with C++11 syntax.

         C++11 compatibility is enforced by this unit test: `cutlass_test_unit_core_cpp11`.
*/

#pragma once

#if !defined(__CUDACC_RTC__)
#include "hip/hip_runtime.h"
#include "hip/hip_runtime.h"

#include "cutlass/trace.h"
#endif

#include "cutlass.h"
#include "cutlass/cuda_host_adapter.hpp"
namespace cutlass {

/////////////////////////////////////////////////////////////////////////////////////////////////

static constexpr int MinWorkspaceAlignment = 16;

#if !defined(__CUDACC_RTC__)
static Status
zero_workspace(void* workspace, size_t workspace_size, hipStream_t stream = nullptr) {
  if (workspace_size > 0) {
    if (workspace == nullptr) {
      CUTLASS_TRACE_HOST("  error: device workspace must not be null");
      return Status::kErrorWorkspaceNull;
    }

    CUTLASS_TRACE_HOST("  clearing workspace");
    hipError_t result = hipMemsetAsync(workspace, 0, workspace_size, stream);
    if (hipSuccess != result) {
      result = hipGetLastError(); // to clear the error bit
      CUTLASS_TRACE_HOST("  hipMemsetAsync() returned error " << hipGetErrorString(result));
      return Status::kErrorInternal;
    }
  }

  return Status::kSuccess;
}
#endif

#if !defined(__CUDACC_RTC__)
template <typename T>
Status
<<<<<<< HEAD
fill_workspace(void* workspace, T fill_value, size_t fill_count, cudaStream_t stream = nullptr, CudaHostAdapter *cuda_adapter = nullptr) {
=======
fill_workspace(void* workspace, T fill_value, size_t fill_count, hipStream_t stream = nullptr) {
>>>>>>> dfa93ce8 (hipify other headers)
  static_assert(sizeof(T) == 4 || sizeof(T) == 2 || sizeof(T) == 1, "Unsupported fill type");
  if (fill_count > 0) {
    if (workspace == nullptr) {
      CUTLASS_TRACE_HOST("  error: device workspace must not be null");
      return Status::kErrorWorkspaceNull;
    }

    CUTLASS_TRACE_HOST("  filling workspace");
<<<<<<< HEAD
    CUdeviceptr d_workspace = reinterpret_cast<CUdeviceptr>(workspace);

#if defined(CUTLASS_ENABLE_CUDA_HOST_ADAPTER) && CUTLASS_ENABLE_CUDA_HOST_ADAPTER

    //
    // Use the cuda host adapter
    //
    CUTLASS_ASSERT(cuda_adapter);
    if (cuda_adapter) {
      Status status = Status::kErrorInternal;

      status = cuda_adapter->memsetDevice(workspace, fill_value, fill_count, stream);

      if (status!=Status::kSuccess) {
        return Status::kErrorInternal;
      }
    }
    else {
      return Status::kErrorInternal;
    }
#else
    CUresult result = CUDA_SUCCESS;
=======
    hipDeviceptr_t d_workspace = reinterpret_cast<hipDeviceptr_t>(workspace);
    hipError_t result = hipSuccess;
>>>>>>> dfa93ce8 (hipify other headers)
    if (sizeof(T) == 4) {
      result = hipMemsetAsync(d_workspace, reinterpret_cast<uint32_t&>(fill_value), fill_count, stream);
    }
    else if (sizeof(T) == 2) {
      result = hipMemsetD16Async(d_workspace, reinterpret_cast<uint16_t&>(fill_value), fill_count, stream);
    }
    else if (sizeof(T) == 1) {
      result = hipMemsetD8Async(d_workspace, reinterpret_cast<uint8_t&>(fill_value), fill_count, stream);
    }

    if (hipSuccess != result) {
      const char** error_string_ptr = nullptr;
      (void) hipGetErrorString(result, error_string_ptr);
      if (error_string_ptr != nullptr) {
        CUTLASS_TRACE_HOST("  cuMemsetD" << sizeof(T) * 8 << "Async() returned error " << *error_string_ptr);
      }
      else {
        CUTLASS_TRACE_HOST("  cuMemsetD" << sizeof(T) * 8 << "Async() returned unrecognized error");
      }
      return Status::kErrorInternal;
    }
#endif
  }

  return Status::kSuccess;
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace cutlass
