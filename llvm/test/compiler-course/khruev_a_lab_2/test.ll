; RUN: opt -load-pass-plugin %llvmshlibdir/khruev_a_lab_2_LLVM_IR%pluginext \
; RUN: -passes=example2 -S %s | FileCheck %s

; CHECK-LABEL: @pow_zero_f32
; CHECK-NOT: llvm.powi
; CHECK: ret float 1.000000e+00
define float @pow_zero_f32(float %x) {
  %r = call float @llvm.powi.f32.i32(float %x, i32 0)
  ret float %r
}

; CHECK-LABEL: @pow_one_f64
; CHECK-NOT: llvm.powi
; CHECK: ret double %x
define double @pow_one_f64(double %x) {
  %r = call double @llvm.powi.f64.i16(double %x, i16 1)
  ret double %r
}

; CHECK-LABEL: @pow_two_f32
; CHECK-NOT: llvm.powi
; CHECK: [[RES:%[0-9]+]] = fmul float %x, %x
; CHECK: ret float [[RES]]
define float @pow_two_f32(float %x) {
  %r = call float @llvm.powi.f32.i32(float %x, i32 2)
  ret float %r
}

; CHECK-LABEL: @pow_three_f80
; CHECK-NOT: llvm.powi
; CHECK: [[T1:%[0-9]+]] = fmul x86_fp80 %x, %x
; CHECK: [[T2:%[0-9]+]] = fmul x86_fp80 [[T1]], %x
; CHECK: ret x86_fp80 [[T2]]
define x86_fp80 @pow_three_f80(x86_fp80 %x) {
  %r = call x86_fp80 @llvm.powi.f80.i32(x86_fp80 %x, i32 3)
  ret x86_fp80 %r
}

; CHECK-LABEL: @pow_four_f128
; CHECK-NOT: llvm.powi
; CHECK: [[T1:%[0-9]+]] = fmul fp128 %x, %x
; CHECK: [[T2:%[0-9]+]] = fmul fp128 [[T1]], [[T1]]
; CHECK: ret fp128 [[T2]]
define fp128 @pow_four_f128(fp128 %x) {
  %r = call fp128 @llvm.powi.f128.i32(fp128 %x, i32 4)
  ret fp128 %r
}

; CHECK-LABEL: @pow_four_ppc
; CHECK-NOT: llvm.powi
; CHECK: [[T1:%[0-9]+]] = fmul ppc_fp128 %x, %x
; CHECK: [[T2:%[0-9]+]] = fmul ppc_fp128 [[T1]], [[T1]]
; CHECK: ret ppc_fp128 [[T2]]
define ppc_fp128 @pow_four_ppc(ppc_fp128 %x) {
  %r = call ppc_fp128 @llvm.powi.ppcf128.i32(ppc_fp128 %x, i32 4)
  ret ppc_fp128 %r
}

; CHECK-LABEL: @pow_big_exp
; CHECK: call float @llvm.powi.f32.i64(float %x, i64 999)
; CHECK: ret float %r
define float @pow_big_exp(float %x) {
  %r = call float @llvm.powi.f32.i64(float %x, i64 999)
  ret float %r
}

; CHECK-LABEL: @pow_negative
; CHECK: call float @llvm.powi.f32.i16(float %x, i16 -2)
; CHECK: ret float %r
define float @pow_negative(float %x) {
  %r = call float @llvm.powi.f32.i16(float %x, i16 -2)
  ret float %r
}