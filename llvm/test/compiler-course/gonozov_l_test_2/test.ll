; RUN: opt -load-pass-plugin %llvmshlibdir/gonozov_l_lab_2_LLVM_IR%pluginext\
; RUN: -passes=example -S %s | FileCheck %s

; CHECK-LABEL: @_Z11test_powi_0f32
; CHECK-NOT: call float @llvm.powi.f32.i32
; CHECK: ret float 1.000000e+00
define float @_Z11test_powi_0f32(float %x) {
  %res = call float @llvm.powi.f32.i32(float %x, i32 0)
  ret float %res
}

; CHECK-LABEL: @_Z11test_powi_1d
; CHECK-NOT: call double @llvm.powi.f64.i16
; CHECK: ret double %x
define double @_Z11test_powi_1d(double %x) {
  %res = call double @llvm.powi.f64.i16(double %x, i16 1)
  ret double %res
}

; CHECK-LABEL: @_Z11test_powi_2f32
; CHECK-NOT: call float @llvm.powi.f32.i32
; CHECK: {{%[0-9]+}} = fmul float %x, %x
; CHECK: ret float {{%[0-9]+}}
define float @_Z11test_powi_2f32(float %x) {
  %res = call float @llvm.powi.f32.i32(float %x, i32 2)
  ret float %res
}

; CHECK-LABEL: @_Z11test_powi_3x86_fp80
; CHECK-NOT: call x86_fp80 @llvm.powi.f80.i32
; CHECK: {{%[0-9]+}} = fmul x86_fp80 %x, %x
; CHECK: {{%[0-9]+}} = fmul x86_fp80 {{%[0-9]+}}, %x
; CHECK: ret x86_fp80 {{%[0-9]+}}
define x86_fp80 @_Z11test_powi_3x86_fp80(x86_fp80 %x) {
  %res = call x86_fp80 @llvm.powi.f80.i32(x86_fp80 %x, i32 3)
  ret x86_fp80 %res
}

; CHECK-LABEL: @_Z11test_powi_4fp128
; CHECK-NOT: call fp128 @llvm.powi.f128.i32
; CHECK: {{%[0-9]+}} = fmul fp128 %x, %x
; CHECK: {{%[0-9]+}} = fmul fp128 {{%[0-9]+}}, {{%[0-9]+}}
; CHECK: ret fp128 {{%[0-9]+}}
define fp128 @_Z11test_powi_4fp128(fp128 %x) {
  %res = call fp128 @llvm.powi.f128.i32(fp128 %x, i32 4)
  ret fp128 %res
}

; CHECK-LABEL: @_Z11test_powi_4ppc_fp128
; CHECK-NOT: call ppc_fp128 @llvm.powi.ppcf128.i32
; CHECK: {{%[0-9]+}} = fmul ppc_fp128 %x, %x
; CHECK: {{%[0-9]+}} = fmul ppc_fp128 {{%[0-9]+}}, {{%[0-9]+}}
; CHECK: ret ppc_fp128 {{%[0-9]+}}
define ppc_fp128 @_Z11test_powi_4ppc_fp128(ppc_fp128 %x) {
  %res = call ppc_fp128 @llvm.powi.ppcf128.i32(ppc_fp128 %x, i32 4)
  ret ppc_fp128 %res
}

; CHECK-LABEL: @_Z14test_powi_1000f32
; CHECK: call float @llvm.powi.f32.i64(float %x, i64 1000)
; CHECK: ret float %res
define float @_Z14test_powi_1000f32(float %x) {
  %res = call float @llvm.powi.f32.i64(float %x, i64 1000)
  ret float %res
}

; CHECK-LABEL: @_Z14test_powi_neg1f16
; CHECK: call float @llvm.powi.f32.i16(float %x, i16 -1)
; CHECK: ret float %res
define float @_Z14test_powi_neg1f16(float %x) {
  %res = call float @llvm.powi.f32.i16(float %x, i16 -1)
  ret float %res
}
