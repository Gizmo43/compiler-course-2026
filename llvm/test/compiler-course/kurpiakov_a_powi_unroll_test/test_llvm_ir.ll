; RUN: opt -load-pass-plugin %llvmshlibdir/kurpiakov_a_powi_unroll_LLVM_IR%pluginext \
; RUN: -passes=powi-unroll -S < %s | FileCheck %s

;все поддерживаемые типы данных 
declare float     @llvm.powi.f32.i32(float, i32)
declare double    @llvm.powi.f64.i16(double, i16)
declare x86_fp80  @llvm.powi.f80.i32(x86_fp80, i32)
declare fp128     @llvm.powi.f128.i32(fp128, i32)
declare ppc_fp128 @llvm.powi.ppcf128.i32(ppc_fp128, i32)

;тест 0 степени
define float @test_powi_0_f32(float %x) {
; CHECK-LABEL: define float @test_powi_0_f32(
; CHECK-NEXT:    ret float 1.000000e+00
;
  %res = call float @llvm.powi.f32.i32(float %x, i32 0)
  ret float %res
}

;тест 1 степени
define double @test_powi_1_f64(double %x) {
; CHECK-LABEL: define double @test_powi_1_f64(
; CHECK-NEXT:    ret double %x
;
  %res = call double @llvm.powi.f64.i16(double %x, i16 1)
  ret double %res
}

;тест 2 степени
define x86_fp80 @test_powi_2_f80(x86_fp80 %x) {
; CHECK-LABEL: define x86_fp80 @test_powi_2_f80(
; CHECK-NEXT:    [[MUL:%.*]] = fmul x86_fp80 %x, %x
; CHECK-NEXT:    ret x86_fp80 [[MUL]]
;
  %res = call x86_fp80 @llvm.powi.f80.i32(x86_fp80 %x, i32 2)
  ret x86_fp80 %res
}

;тест 3 степени
define fp128 @test_powi_3_f128(fp128 %x) {
; CHECK-LABEL: define fp128 @test_powi_3_f128(
; CHECK-NEXT:    [[MUL1:%.*]] = fmul fp128 %x, %x
; CHECK-NEXT:    [[MUL2:%.*]] = fmul fp128 [[MUL1]], %x
; CHECK-NEXT:    ret fp128 [[MUL2]]
;
  %res = call fp128 @llvm.powi.f128.i32(fp128 %x, i32 3)
  ret fp128 %res
}

;тест 4 степени
define ppc_fp128 @test_powi_4_ppcf128(ppc_fp128 %x) {
; CHECK-LABEL: define ppc_fp128 @test_powi_4_ppcf128(
; CHECK-NEXT:    [[MUL1:%.*]] = fmul ppc_fp128 %x, %x
; CHECK-NEXT:    [[MUL2:%.*]] = fmul ppc_fp128 [[MUL1]], [[MUL1]]
; CHECK-NEXT:    ret ppc_fp128 [[MUL2]]
;
  %res = call ppc_fp128 @llvm.powi.ppcf128.i32(ppc_fp128 %x, i32 4)
  ret ppc_fp128 %res
}

;тест превышающей разворачивание степени
define double @test_powi_5_f64(double %x) {
; CHECK-LABEL: define double @test_powi_5_f64(
; CHECK-NEXT:    [[RES:%.*]] = call double @llvm.powi.f64.i16(double %x, i16 5)
; CHECK-NEXT:    ret double [[RES]]
;
  %res = call double @llvm.powi.f64.i16(double %x, i16 5)
  ret double %res
}

;тест отрицательной степени
define float @test_powi_negative_f32(float %x) {
; CHECK-LABEL: define float @test_powi_negative_f32(
; CHECK-NEXT:    [[RES:%.*]] = call float @llvm.powi.f32.i32(float %x, i32 -2)
; CHECK-NEXT:    ret float [[RES]]
;
  %res = call float @llvm.powi.f32.i32(float %x, i32 -2)
  ret float %res
}