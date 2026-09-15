; RUN: opt -load-pass-plugin=%llvmshlibdir/kazennova_a_lab2_LLVM_IR%pluginext -passes="decompose-fmuladd" -S %s | FileCheck %s

define float @test_float(float %a, float %b, float %c) {
entry:
  %r = call float @llvm.fmuladd.f32(float %a, float %b, float %c)
  ret float %r
}

; CHECK-LABEL: define float @test_float
; CHECK: entry:
; CHECK-NEXT: [[MUL:%[0-9]+]] = fmul float %a, %b
; CHECK-NEXT: [[ADD:%[0-9]+]] = fadd float [[MUL]], %c
; CHECK-NEXT: ret float [[ADD]]

define double @test_double(double %a, double %b, double %c) {
entry:
  %r = call double @llvm.fmuladd.f64(double %a, double %b, double %c)
  ret double %r
}

; CHECK-LABEL: define double @test_double
; CHECK: entry:
; CHECK-NEXT: [[MUL:%[0-9]+]] = fmul double %a, %b
; CHECK-NEXT: [[ADD:%[0-9]+]] = fadd double [[MUL]], %c
; CHECK-NEXT: ret double [[ADD]]

define half @test_half(half %a, half %b, half %c) {
entry:
  %r = call half @llvm.fmuladd.f16(half %a, half %b, half %c)
  ret half %r
}

; CHECK-LABEL: define half @test_half
; CHECK: entry:
; CHECK-NEXT: [[MUL:%[0-9]+]] = fmul half %a, %b
; CHECK-NEXT: [[ADD:%[0-9]+]] = fadd half [[MUL]], %c
; CHECK-NEXT: ret half [[ADD]]

define float @test_fast_math(float %a, float %b, float %c) {
entry:
  %r = call fast float @llvm.fmuladd.f32(float %a, float %b, float %c)
  ret float %r
}

; CHECK-LABEL: define float @test_fast_math
; CHECK: entry:
; CHECK-NEXT: [[MUL:%[0-9]+]] = fmul fast float %a, %b
; CHECK-NEXT: [[ADD:%[0-9]+]] = fadd fast float [[MUL]], %c
; CHECK-NEXT: ret float [[ADD]]

; Тест с двумя последовательными вызовами fmuladd
define float @test_two_fmuladd(float %a, float %b, float %c, float %d) {
entry:
  %r1 = call float @llvm.fmuladd.f32(float %a, float %b, float %c)
  %r2 = call float @llvm.fmuladd.f32(float %r1, float %d, float %c)
  ret float %r2
}

; CHECK-LABEL: define float @test_two_fmuladd
; CHECK: entry:
; CHECK-NEXT: [[MUL1:%[0-9]+]] = fmul float %a, %b
; CHECK-NEXT: [[ADD1:%[0-9]+]] = fadd float [[MUL1]], %c
; CHECK-NEXT: [[MUL2:%[0-9]+]] = fmul float [[ADD1]], %d
; CHECK-NEXT: [[ADD2:%[0-9]+]] = fadd float [[MUL2]], %c
; CHECK-NEXT: ret float [[ADD2]]