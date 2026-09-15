; RUN: opt -load-pass-plugin %llvmshlibdir/kruglova_a_lab2_LLVM_IR%pluginext \
; RUN:     -passes=lab2_llvm_powi -S %s | FileCheck %s

declare float @llvm.powi.f32.i32(float, i32)
declare double @llvm.powi.f64.i32(double, i32)
declare fp128 @llvm.powi.f128.i32(fp128, i32)
declare <2 x double> @llvm.powi.v2f64.i32(<2 x double>, i32)
declare <4 x float> @llvm.powi.v4f32.i32(<4 x float>, i32)
declare <8 x float> @llvm.powi.v8f32.i32(<8 x float>, i32)



define float @test_scalar_pow0(float %x) {
; CHECK-LABEL: @test_scalar_pow0(
; CHECK-NEXT:    ret float 1.000000e+00
  %res = call float @llvm.powi.f32.i32(float %x, i32 0)
  ret float %res
}

define fp128 @test_fp128_pow1(fp128 %x) {
; CHECK-LABEL: @test_fp128_pow1(
; CHECK-NEXT:    ret fp128 %x
  %res = call fp128 @llvm.powi.f128.i32(fp128 %x, i32 1)
  ret fp128 %res
}

define double @test_scalar_pow2(double %base) {
; CHECK-LABEL: @test_scalar_pow2(
; CHECK-NEXT:    [[MUL:%.*]] = fmul double %base, %base
; CHECK-NEXT:    ret double [[MUL]]
  %res = call double @llvm.powi.f64.i32(double %base, i32 2)
  ret double %res
}

define float @test_scalar_pow3_float(float %val) {
; CHECK-LABEL: @test_scalar_pow3_float(
; CHECK-NEXT:    [[TEMP:%.*]] = fmul float %val, %val
; CHECK-NEXT:    [[RES:%.*]] = fmul float [[TEMP]], %val
; CHECK-NEXT:    ret float [[RES]]
  %res = call float @llvm.powi.f32.i32(float %val, i32 3)
  ret float %res
}

define float @test_scalar_pow4(float %val) {
; CHECK-LABEL: @test_scalar_pow4(
; CHECK-NEXT:    [[SQ:%.*]] = fmul float %val, %val
; CHECK-NEXT:    [[QD:%.*]] = fmul float [[SQ]], [[SQ]]
; CHECK-NEXT:    ret float [[QD]]
  %res = call float @llvm.powi.f32.i32(float %val, i32 4)
  ret float %res
}

; степень 4 для ppc_fp128
define ppc_fp128 @test_ppc_pow4(ppc_fp128 %x) {
; CHECK-LABEL: @test_ppc_pow4(
; CHECK-NEXT:    [[SQ:%.*]] = fmul ppc_fp128 %x, %x
; CHECK-NEXT:    [[QD:%.*]] = fmul ppc_fp128 [[SQ]], [[SQ]]
; CHECK-NEXT:    ret ppc_fp128 [[QD]]
  %res = call ppc_fp128 @llvm.powi.ppcf128.i32(ppc_fp128 %x, i32 4)
  ret ppc_fp128 %res
}

; степень 0 для вектора 
define <2 x double> @test_vec_zero_double(<2 x double> %v) {
; CHECK-LABEL: @test_vec_zero_double(
; CHECK-NEXT:    ret <2 x double> splat (double 1.000000e+00)
  %res = call <2 x double> @llvm.powi.v2f64.i32(<2 x double> %v, i32 0)
  ret <2 x double> %res
}

; тест степени 1 для вектора
define <8 x float> @test_vec_first_float(<8 x float> %v) {
; CHECK-LABEL: @test_vec_first_float(
; CHECK-NEXT:    ret <8 x float> %v
  %res = call <8 x float> @llvm.powi.v8f32.i32(<8 x float> %v, i32 1)
  ret <8 x float> %res
}

; тест степени 2 для вектора <4 x float>
define <4 x float> @test_vec_square(<4 x float> %v) {
; CHECK-LABEL: @test_vec_square(
; CHECK-NEXT:    [[V2:%.*]] = fmul <4 x float> %v, %v
; CHECK-NEXT:    ret <4 x float> [[V2]]
  %res = call <4 x float> @llvm.powi.v4f32.i32(<4 x float> %v, i32 2)
  ret <4 x float> %res
}

; тест степени 3 для вектора <8 x float>
define <8 x float> @test_vec_cube(<8 x float> %v) {
; CHECK-LABEL: @test_vec_cube(
; CHECK-NEXT:    [[VTMP:%.*]] = fmul <8 x float> %v, %v
; CHECK-NEXT:    [[VRES:%.*]] = fmul <8 x float> [[VTMP]], %v
; CHECK-NEXT:    ret <8 x float> [[VRES]]
  %res = call <8 x float> @llvm.powi.v8f32.i32(<8 x float> %v, i32 3)
  ret <8 x float> %res
}

; тест степени 4 для вектора <2 x double>
define <2 x double> @test_vec_pow4(<2 x double> %v) {
; CHECK-LABEL: @test_vec_pow4(
; CHECK-NEXT:    [[VSQ:%.*]] = fmul <2 x double> %v, %v
; CHECK-NEXT:    [[VQD:%.*]] = fmul <2 x double> [[VSQ]], [[VSQ]]
; CHECK-NEXT:    ret <2 x double> [[VQD]]
  %res = call <2 x double> @llvm.powi.v2f64.i32(<2 x double> %v, i32 4)
  ret <2 x double> %res
}

; степень 5 не должна обрабатываться
define float @test_ignore_pow5(float %x) {
; CHECK-LABEL: @test_ignore_pow5(
; CHECK-NEXT:    [[CALL:%.*]] = call float @llvm.powi.f32.i32(float %x, i32 5)
; CHECK-NEXT:    ret float [[CALL]]
  %res = call float @llvm.powi.f32.i32(float %x, i32 5)
  ret float %res
}

; отрицательная степень не должна обрабатываться
define double @test_ignore_neg(double %x) {
; CHECK-LABEL: @test_ignore_neg(
; CHECK-NEXT:    [[CALL:%.*]] = call double @llvm.powi.f64.i32(double %x, i32 -2)
; CHECK-NEXT:    ret double [[CALL]]
  %res = call double @llvm.powi.f64.i32(double %x, i32 -2)
  ret double %res
}

; переменная вместо константы не должна меняться
define float @test_ignore_var(float %base, i32 %n) {
; CHECK-LABEL: @test_ignore_var(
; CHECK-NEXT:    [[RES:%.*]] = call float @llvm.powi.f32.i32(float %base, i32 %n)
; CHECK-NEXT:    ret float [[RES]]
  %1 = call float @llvm.powi.f32.i32(float %base, i32 %n)
  ret float %1
}
