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

#pragma once

#include <hip/hip_runtime.h>
#include <hipblas/hipblas.h>

//-- BLAM_DEBUG_OUT ---------------------------------------------------------
#ifdef BLAM_DEBUG
# include <iostream>
# ifndef BLAM_DEBUG_OUT
#  define BLAM_DEBUG_OUT(msg)    std::cerr << "BLAM: " << msg << std::endl
#  define BLAM_DEBUG_OUT_2(msg)  std::cerr << msg << std::endl
# endif // BLAM_DEBUG_OUT
#else
# ifndef BLAM_DEBUG_OUT
#  define BLAM_DEBUG_OUT(msg)
#  define BLAM_DEBUG_OUT_2(msg)
# endif // BLAM_DEBUG_OUT
#endif // BLAM_DEBUG

// User could potentially define ComplexFloat/ComplexDouble instead of std::
#ifndef BLAM_COMPLEX_TYPES
#define BLAM_COMPLEX_TYPES 1
#include <cuda/std/complex>
namespace blam {
template <typename T>
using Complex       = cuda::std::complex<T>;
using ComplexFloat  = cuda::std::complex<float>;
using ComplexDouble = cuda::std::complex<double>;
}
#endif // BLAM_COMPLEX_TYPES

// User could potentially define Half instead of cute::
#ifndef BLAM_HALF_TYPE
#define BLAM_HALF_TYPE 1
#include <cute/numeric/numeric_types.hpp>
namespace blam {
using Half = cute::half_t;
}
#endif // BLAM_HALF_TYPE

namespace blam
{
namespace cublas
{

inline const char*
cublas_get_error(hipblasStatus_t status)
{
  switch (status) {
    case HIPBLAS_STATUS_SUCCESS:
      return "HIPBLAS_STATUS_SUCCESS";
    case HIPBLAS_STATUS_NOT_INITIALIZED:
      return "HIPBLAS_STATUS_NOT_INITIALIZED -- The cuBLAS library was not initialized.";
    case HIPBLAS_STATUS_ALLOC_FAILED:
      return "HIPBLAS_STATUS_ALLOC_FAILED -- Resource allocation failed inside the cuBLAS library.";
    case HIPBLAS_STATUS_INVALID_VALUE:
      return "HIPBLAS_STATUS_INVALID_VALUE -- An unsupported value or parameter was passed to the function.";
    case HIPBLAS_STATUS_ARCH_MISMATCH:
      return "HIPBLAS_STATUS_ARCH_MISMATCH -- The function requires a feature absent from the device architecture.";
    case HIPBLAS_STATUS_MAPPING_ERROR:
      return "HIPBLAS_STATUS_MAPPING_ERROR -- An access to GPU memory space failed.";
    case HIPBLAS_STATUS_EXECUTION_FAILED:
      return "HIPBLAS_STATUS_EXECUTION_FAILED -- The GPU program failed to execute.";
    case HIPBLAS_STATUS_INTERNAL_ERROR:
      return "HIPBLAS_STATUS_INTERNAL_ERROR -- An internal cuBLAS operation failed.";
    case HIPBLAS_STATUS_NOT_SUPPORTED:
      return "HIPBLAS_STATUS_NOT_SUPPORTED -- The functionality requested is not supported.";
    case CUBLAS_STATUS_LICENSE_ERROR:
      return "CUBLAS_STATUS_LICENSE_ERROR -- An error was detected when checking the current licensing.";
    default:
      return "CUBLAS_ERROR -- <unknown>";
  }
}

inline bool
cublas_is_error(hipblasStatus_t status)
{
  return status != HIPBLAS_STATUS_SUCCESS;
}


// hgemm
inline hipblasStatus_t
gemm(hipblasHandle_t handle,
     hipblasOperation_t transA, hipblasOperation_t transB,
     int m, int n, int k,
     const Half* alpha,
     const Half* A, int ldA,
     const Half* B, int ldB,
     const Half* beta,
     Half* C, int ldC)
{
  BLAM_DEBUG_OUT("hipblasHgemm");

  return hipblasGemmEx(handle, transA, transB,
                      m, n, k,
                      reinterpret_cast<const __half*>(alpha),
                      reinterpret_cast<const __half*>(A), HIP_R_16F, ldA,
                      reinterpret_cast<const __half*>(B), HIP_R_16F, ldB,
                      reinterpret_cast<const __half*>(beta),
                      reinterpret_cast<      __half*>(C), HIP_R_16F, ldC,
                      HIP_R_16F, CUBLAS_GEMM_DEFAULT_TENSOR_OP);
}

// mixed hf gemm
inline hipblasStatus_t
gemm(hipblasHandle_t handle,
     hipblasOperation_t transA, hipblasOperation_t transB,
     int m, int n, int k,
     const float* alpha,
     const Half* A, int ldA,
     const Half* B, int ldB,
     const float* beta,
     float* C, int ldC)
{
  BLAM_DEBUG_OUT("hipblasGemmEx mixed half-float");

  return hipblasGemmEx(handle, transA, transB,
                      m, n, k,
                      alpha,
                      reinterpret_cast<const __half*>(A), HIP_R_16F, ldA,
                      reinterpret_cast<const __half*>(B), HIP_R_16F, ldB,
                      beta,
                      C, HIP_R_32F, ldC,
                      HIP_R_32F, CUBLAS_GEMM_DEFAULT_TENSOR_OP);
}

// igemm
inline hipblasStatus_t
gemm(hipblasHandle_t handle,
     hipblasOperation_t transA, hipblasOperation_t transB,
     int m, int n, int k,
     const int32_t* alpha,
     const int8_t* A, int ldA,
     const int8_t* B, int ldB,
     const int32_t* beta,
     int32_t* C, int ldC)
{
  BLAM_DEBUG_OUT("cublasIgemm");

  return hipblasGemmEx(handle, transA, transB,
                      m, n, k,
                      alpha,
                      A, HIP_R_8I, ldA,
                      B, HIP_R_8I, ldB,
                      beta,
                      C, HIP_R_32I, ldC,
                      HIP_R_32I, CUBLAS_GEMM_DEFAULT_TENSOR_OP);
}

// sgemm
inline hipblasStatus_t
gemm(hipblasHandle_t handle,
     hipblasOperation_t transA, hipblasOperation_t transB,
     int m, int n, int k,
     const float* alpha,
     const float* A, int ldA,
     const float* B, int ldB,
     const float* beta,
     float* C, int ldC)
{
  BLAM_DEBUG_OUT("hipblasSgemm");

  return hipblasSgemm(handle, transA, transB,
                     m, n, k,
                     alpha,
                     A, ldA,
                     B, ldB,
                     beta,
                     C, ldC);
}

// dgemm
inline hipblasStatus_t
gemm(hipblasHandle_t handle,
     hipblasOperation_t transA, hipblasOperation_t transB,
     int m, int n, int k,
     const double* alpha,
     const double* A, int ldA,
     const double* B, int ldB,
     const double* beta,
     double* C, int ldC)
{
  BLAM_DEBUG_OUT("hipblasDgemm");

  return hipblasDgemm(handle, transA, transB,
                     m, n, k,
                     alpha,
                     A, ldA,
                     B, ldB,
                     beta,
                     C, ldC);
}

// cgemm
inline hipblasStatus_t
gemm(hipblasHandle_t handle,
     hipblasOperation_t transA, hipblasOperation_t transB,
     int m, int n, int k,
     const ComplexFloat* alpha,
     const ComplexFloat* A, int ldA,
     const ComplexFloat* B, int ldB,
     const ComplexFloat* beta,
     ComplexFloat* C, int ldC)
{
  BLAM_DEBUG_OUT("hipblasCgemm");

  return hipblasCgemm(handle, transA, transB,
                     m, n, k,
                     reinterpret_cast<const cuFloatComplex*>(alpha),
                     reinterpret_cast<const cuFloatComplex*>(A), ldA,
                     reinterpret_cast<const cuFloatComplex*>(B), ldB,
                     reinterpret_cast<const cuFloatComplex*>(beta),
                     reinterpret_cast<cuFloatComplex*>(C), ldC);
}

// zgemm
inline hipblasStatus_t
gemm(hipblasHandle_t handle,
     hipblasOperation_t transA, hipblasOperation_t transB,
     int m, int n, int k,
     const ComplexDouble* alpha,
     const ComplexDouble* A, int ldA,
     const ComplexDouble* B, int ldB,
     const ComplexDouble* beta,
     ComplexDouble* C, int ldC)
{
  BLAM_DEBUG_OUT("hipblasZgemm");

  return hipblasZgemm(handle, transA, transB,
                     m, n, k,
                     reinterpret_cast<const hipDoubleComplex*>(alpha),
                     reinterpret_cast<const hipDoubleComplex*>(A), ldA,
                     reinterpret_cast<const hipDoubleComplex*>(B), ldB,
                     reinterpret_cast<const hipDoubleComplex*>(beta),
                     reinterpret_cast<hipDoubleComplex*>(C), ldC);
}

// hgemm
inline hipblasStatus_t
gemm_batch(hipblasHandle_t handle,
           hipblasOperation_t transA, hipblasOperation_t transB,
           int m, int n, int k,
           const Half* alpha,
           const Half* A, int ldA, int loA,
           const Half* B, int ldB, int loB,
           const Half* beta,
           Half* C, int ldC, int loC,
           int batch_size)
{
  BLAM_DEBUG_OUT("hipblasHgemmStridedBatched");

  return hipblasHgemmStridedBatched(handle, transA, transB,
                                   m, n, k,
                                   reinterpret_cast<const __half*>(alpha),
                                   reinterpret_cast<const __half*>(A), ldA, loA,
                                   reinterpret_cast<const __half*>(B), ldB, loB,
                                   reinterpret_cast<const __half*>(beta),
                                   reinterpret_cast<__half*>(C), ldC, loC,
                                   batch_size);
}

// sgemm
inline hipblasStatus_t
gemm_batch(hipblasHandle_t handle,
           hipblasOperation_t transA, hipblasOperation_t transB,
           int m, int n, int k,
           const float* alpha,
           const float* A, int ldA, int loA,
           const float* B, int ldB, int loB,
           const float* beta,
           float* C, int ldC, int loC,
           int batch_size)
{
  BLAM_DEBUG_OUT("hipblasSgemmStridedBatched");

  return hipblasSgemmStridedBatched(handle, transA, transB,
                                   m, n, k,
                                   alpha,
                                   A, ldA, loA,
                                   B, ldB, loB,
                                   beta,
                                   C, ldC, loC,
                                   batch_size);
}

// dgemm
inline hipblasStatus_t
gemm_batch(hipblasHandle_t handle,
           hipblasOperation_t transA, hipblasOperation_t transB,
           int m, int n, int k,
           const double* alpha,
           const double* A, int ldA, int loA,
           const double* B, int ldB, int loB,
           const double* beta,
           double* C, int ldC, int loC,
           int batch_size)
{
  BLAM_DEBUG_OUT("hipblasDgemmStridedBatched");

  return hipblasDgemmStridedBatched(handle, transA, transB,
                                   m, n, k,
                                   alpha,
                                   A, ldA, loA,
                                   B, ldB, loB,
                                   beta,
                                   C, ldC, loC,
                                   batch_size);
}

// cgemm
inline hipblasStatus_t
gemm_batch(hipblasHandle_t handle,
           hipblasOperation_t transA, hipblasOperation_t transB,
           int m, int n, int k,
           const ComplexFloat* alpha,
           const ComplexFloat* A, int ldA, int loA,
           const ComplexFloat* B, int ldB, int loB,
           const ComplexFloat* beta,
           ComplexFloat* C, int ldC, int loC,
           int batch_size)
{
  BLAM_DEBUG_OUT("hipblasCgemmStridedBatched");

  return hipblasCgemmStridedBatched(handle, transA, transB,
                                   m, n, k,
                                   reinterpret_cast<const cuFloatComplex*>(alpha),
                                   reinterpret_cast<const cuFloatComplex*>(A), ldA, loA,
                                   reinterpret_cast<const cuFloatComplex*>(B), ldB, loB,
                                   reinterpret_cast<const cuFloatComplex*>(beta),
                                   reinterpret_cast<cuFloatComplex*>(C), ldC, loC,
                                   batch_size);
}

// zgemm
inline hipblasStatus_t
gemm_batch(hipblasHandle_t handle,
           hipblasOperation_t transA, hipblasOperation_t transB,
           int m, int n, int k,
           const ComplexDouble* alpha,
           const ComplexDouble* A, int ldA, int loA,
           const ComplexDouble* B, int ldB, int loB,
           const ComplexDouble* beta,
           ComplexDouble* C, int ldC, int loC,
           int batch_size)
{
  BLAM_DEBUG_OUT("hipblasZgemmStridedBatched");

  return hipblasZgemmStridedBatched(handle, transA, transB,
                                   m, n, k,
                                   reinterpret_cast<const hipDoubleComplex*>(alpha),
                                   reinterpret_cast<const hipDoubleComplex*>(A), ldA, loA,
                                   reinterpret_cast<const hipDoubleComplex*>(B), ldB, loB,
                                   reinterpret_cast<const hipDoubleComplex*>(beta),
                                   reinterpret_cast<hipDoubleComplex*>(C), ldC, loC,
                                   batch_size);
}

// hgemm
inline hipblasStatus_t
gemm_batch(hipblasHandle_t handle,
           hipblasOperation_t transA, hipblasOperation_t transB,
           int m, int n, int k,
           const Half* alpha,
           const Half* const A[], int ldA,
           const Half* const B[], int ldB,
           const Half* beta,
           Half* const C[], int ldC,
           int batch_size)
{
  BLAM_DEBUG_OUT("hipblasHgemmBatched");

  return hipblasHgemmBatched(handle, transA, transB,
                            m, n, k,
                            reinterpret_cast<const __half*>(alpha),
                            reinterpret_cast<const __half**>(const_cast<const Half**>(A)), ldA,
                            // A, ldA,   // cuBLAS 9.2
                            reinterpret_cast<const __half**>(const_cast<const Half**>(B)), ldB,
                            // B, ldB,   // cuBLAS 9.2
                            reinterpret_cast<const __half*>(beta),
                            reinterpret_cast<__half**>(const_cast<Half**>(C)), ldC,
                            // C, ldC,   // cuBLAS 9.2
                            batch_size);
}

// sgemm
inline hipblasStatus_t
gemm_batch(hipblasHandle_t handle,
           hipblasOperation_t transA, hipblasOperation_t transB,
           int m, int n, int k,
           const float* alpha,
           const float* const A[], int ldA,
           const float* const B[], int ldB,
           const float* beta,
           float* const C[], int ldC,
           int batch_size)
{
  BLAM_DEBUG_OUT("hipblasSgemmBatched");

  return hipblasSgemmBatched(handle, transA, transB,
                            m, n, k,
                            alpha,
                            const_cast<const float**>(A), ldA,
                            // A, ldA,   // cuBLAS 9.2
                            const_cast<const float**>(B), ldB,
                            // B, ldB,   // cuBLAS 9.2
                            beta,
                            const_cast<float**>(C), ldC,
                            // C, ldC,   // cuBLAS 9.2
                            batch_size);
}

// dgemm
inline hipblasStatus_t
gemm_batch(hipblasHandle_t handle,
           hipblasOperation_t transA, hipblasOperation_t transB,
           int m, int n, int k,
           const double* alpha,
           const double* const A[], int ldA,
           const double* const B[], int ldB,
           const double* beta,
           double* const C[], int ldC,
           int batch_size)
{
  BLAM_DEBUG_OUT("hipblasDgemmBatched");

  return hipblasDgemmBatched(handle, transA, transB,
                            m, n, k,
                            alpha,
                            const_cast<const double**>(A), ldA,
                            // A, ldA,   // cuBLAS 9.2
                            const_cast<const double**>(B), ldB,
                            // B, ldB,   // cuBLAS 9.2
                            beta,
                            const_cast<double**>(C), ldC,
                            // C, ldC,   // cuBLAS 9.2
                            batch_size);
}

// cgemm
inline hipblasStatus_t
gemm_batch(hipblasHandle_t handle,
           hipblasOperation_t transA, hipblasOperation_t transB,
           int m, int n, int k,
           const ComplexFloat* alpha,
           const ComplexFloat* const A[], int ldA,
           const ComplexFloat* const B[], int ldB,
           const ComplexFloat* beta,
           ComplexFloat* const C[], int ldC,
           int batch_size)
{
  BLAM_DEBUG_OUT("hipblasCgemmBatched");

  return hipblasCgemmBatched(handle, transA, transB,
                            m, n, k,
                            reinterpret_cast<const cuFloatComplex*>(alpha),
                            const_cast<const cuFloatComplex**>(reinterpret_cast<const cuFloatComplex* const *>(A)), ldA,
                            //reinterpret_cast<const cuFloatComplex* const *>(A), ldA,  // cuBLAS 9.2
                            const_cast<const cuFloatComplex**>(reinterpret_cast<const cuFloatComplex* const *>(B)), ldB,
                            //reinterpret_cast<const cuFloatComplex* const *>(B), ldB,  // cuBLAS 9.2
                            reinterpret_cast<const cuFloatComplex*>(beta),
                            const_cast<cuFloatComplex**>(reinterpret_cast<cuFloatComplex* const *>(C)), ldC,
                            //reinterpret_cast<cuFloatComplex* const *>(C), ldC,        // cuBLAS 9.2
                            batch_size);
}

// zgemm
inline hipblasStatus_t
gemm_batch(hipblasHandle_t handle,
           hipblasOperation_t transA, hipblasOperation_t transB,
           int m, int n, int k,
           const ComplexDouble* alpha,
           const ComplexDouble* const A[], int ldA,
           const ComplexDouble* const B[], int ldB,
           const ComplexDouble* beta,
           ComplexDouble* const C[], int ldC,
           int batch_size)
{
  BLAM_DEBUG_OUT("hipblasZgemmBatched");

  return hipblasZgemmBatched(handle, transA, transB,
                            m, n, k,
                            reinterpret_cast<const hipDoubleComplex*>(alpha),
                            const_cast<const hipDoubleComplex**>(reinterpret_cast<const hipDoubleComplex* const *>(A)), ldA,
                            //reinterpret_cast<const hipDoubleComplex* const *>(A), ldA,  // cuBLAS 9.2
                            const_cast<const hipDoubleComplex**>(reinterpret_cast<const hipDoubleComplex* const *>(B)), ldB,
                            //reinterpret_cast<const hipDoubleComplex* const *>(B), ldB,  // cuBLAS 9.2
                            reinterpret_cast<const hipDoubleComplex*>(beta),
                            const_cast<hipDoubleComplex**>(reinterpret_cast<hipDoubleComplex* const *>(C)), ldC,
                            //reinterpret_cast<hipDoubleComplex* const *>(C), ldC,        // cuBLAS 9.2
                            batch_size);
}

} // end namespace cublas
} // end namespace blam
