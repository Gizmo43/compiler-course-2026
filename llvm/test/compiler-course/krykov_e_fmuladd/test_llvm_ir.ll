; RUN: opt -load-pass-plugin %llvmshlibdir/krykov_e_fmuladd_LLVM_IR%pluginext \
; RUN:   -passes=replace-fmuladd -S %s | FileCheck %s


; CHECK-LABEL: @test_float
; CHECK-NOT: call float @llvm.fmuladd.f32
; CHECK: %fmuladd.mul = fmul float %a, %b
; CHECK: %fmuladd.add = fadd float %fmuladd.mul, %c
; CHECK: ret float %fmuladd.add

define float @test_float(float %a, float %b, float %c) {
  %res = call float @llvm.fmuladd.f32(float %a, float %b, float %c)
  ret float %res
}

; CHECK-LABEL: @test_double
; CHECK-NOT: call double @llvm.fmuladd.f64
; CHECK: %fmuladd.mul = fmul double %a, %b
; CHECK: %fmuladd.add = fadd double %fmuladd.mul, %c
; CHECK: ret double %fmuladd.add

define double @test_double(double %a, double %b, double %c) {
  %res = call double @llvm.fmuladd.f64(double %a, double %b, double %c)
  ret double %res
}

; CHECK-LABEL: @test_fast_flags
; CHECK: %fmuladd.mul = fmul fast float %a, %b
; CHECK: %fmuladd.add = fadd fast float %fmuladd.mul, %c
; CHECK: ret float %fmuladd.add

define float @test_fast_flags(float %a, float %b, float %c) {
  %res = call fast float @llvm.fmuladd.f32(float %a, float %b, float %c)
  ret float %res
}

; CHECK-LABEL: @test_combined_flags
; CHECK: %fmuladd.mul = fmul nnan ninf float %a, %b
; CHECK: %fmuladd.add = fadd nnan ninf float %fmuladd.mul, %c
; CHECK: ret float %fmuladd.add

define float @test_combined_flags(float %a, float %b, float %c) {
  %res = call nnan ninf float @llvm.fmuladd.f32(float %a, float %b, float %c)
  ret float %res
}

; CHECK-LABEL: @test_multiple_uses
; CHECK: %fmuladd.mul = fmul float %a, %b
; CHECK: %fmuladd.add = fadd float %fmuladd.mul, %c
; CHECK: %add1 = fadd float %fmuladd.add, 1.0
; CHECK: %add2 = fadd float %fmuladd.add, 2.0
; CHECK: %res = fadd float %add1, %add2
; CHECK: ret float %res

define float @test_multiple_uses(float %a, float %b, float %c) {
  %t = call float @llvm.fmuladd.f32(float %a, float %b, float %c)
  %add1 = fadd float %t, 1.0
  %add2 = fadd float %t, 2.0
  %res = fadd float %add1, %add2
  ret float %res
}

; CHECK-LABEL: @test_loop
; CHECK: loop:
; CHECK: %fmuladd.mul = fmul float %a, %b
; CHECK: %fmuladd.add = fadd float %fmuladd.mul, %c
; CHECK: %cond = icmp ult i32 %next, 10
; CHECK: br i1 %cond, label %loop, label %exit

define float @test_loop(float %a, float %b, float %c, i32 %n) {
entry:
  br label %loop
loop:
  %i = phi i32 [ 0, %entry ], [ %next, %loop ]
  %res = call float @llvm.fmuladd.f32(float %a, float %b, float %c)
  %next = add i32 %i, 1
  %cond = icmp ult i32 %next, 10
  br i1 %cond, label %loop, label %exit
exit:
  ret float %res
}

declare float @llvm.fmuladd.f32(float, float, float)
declare double @llvm.fmuladd.f64(double, double, double)