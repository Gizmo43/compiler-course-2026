; RUN: opt -load-pass-plugin %llvmshlibdir/gusev_d_lab2_LLVM_IR%pluginext \
; RUN:   -passes=gusev-d-lab2 -S %s | FileCheck %s

; CHECK-NOT: srem
; CHECK-NOT: urem
; CHECK-NOT: frem

; Integer signed remainder.
define i32 @test_srem(i32 %a, i32 %b) {
; CHECK-LABEL: @test_srem(
; CHECK: entry:
; CHECK-NEXT: [[DIV:%.*]] = sdiv i32 %a, %b
; CHECK-NEXT: [[MUL:%.*]] = mul i32 [[DIV]], %b
; CHECK-NEXT: [[SUB:%.*]] = sub i32 %a, [[MUL]]
; CHECK-NEXT: ret i32 [[SUB]]
entry:
  %r = srem i32 %a, %b
  ret i32 %r
}

; Integer unsigned remainder.
define i32 @test_urem(i32 %a, i32 %b) {
; CHECK-LABEL: @test_urem(
; CHECK: entry:
; CHECK-NEXT: [[DIV:%.*]] = udiv i32 %a, %b
; CHECK-NEXT: [[MUL:%.*]] = mul i32 [[DIV]], %b
; CHECK-NEXT: [[SUB:%.*]] = sub i32 %a, [[MUL]]
; CHECK-NEXT: ret i32 [[SUB]]
entry:
  %r = urem i32 %a, %b
  ret i32 %r
}

; Floating-point remainder.
define float @test_frem(float %x, float %y) {
; CHECK-LABEL: @test_frem(
; CHECK: entry:
; CHECK-NEXT: [[DIV:%.*]] = fdiv float %x, %y
; CHECK-NEXT: [[TRUNC:%.*]] = call float @llvm.trunc.f32(float [[DIV]])
; CHECK-NEXT: [[MUL:%.*]] = fmul float [[TRUNC]], %y
; CHECK-NEXT: [[SUB:%.*]] = fsub float %x, [[MUL]]
; CHECK-NEXT: ret float [[SUB]]
entry:
  %r = frem float %x, %y
  ret float %r
}

; Vector integer signed remainder.
define <4 x i32> @test_vector_srem(<4 x i32> %a, <4 x i32> %b) {
; CHECK-LABEL: @test_vector_srem(
; CHECK: entry:
; CHECK-NEXT: [[DIV:%.*]] = sdiv <4 x i32> %a, %b
; CHECK-NEXT: [[MUL:%.*]] = mul <4 x i32> [[DIV]], %b
; CHECK-NEXT: [[SUB:%.*]] = sub <4 x i32> %a, [[MUL]]
; CHECK-NEXT: ret <4 x i32> [[SUB]]
entry:
  %r = srem <4 x i32> %a, %b
  ret <4 x i32> %r
}

; Vector floating-point remainder.
define <2 x double> @test_vector_frem(<2 x double> %x, <2 x double> %y) {
; CHECK-LABEL: @test_vector_frem(
; CHECK: entry:
; CHECK-NEXT: [[DIV:%.*]] = fdiv <2 x double> %x, %y
; CHECK-NEXT: [[TRUNC:%.*]] = call <2 x double> @llvm.trunc.v2f64(<2 x double> [[DIV]])
; CHECK-NEXT: [[MUL:%.*]] = fmul <2 x double> [[TRUNC]], %y
; CHECK-NEXT: [[SUB:%.*]] = fsub <2 x double> %x, [[MUL]]
; CHECK-NEXT: ret <2 x double> [[SUB]]
entry:
  %r = frem <2 x double> %x, %y
  ret <2 x double> %r
}
