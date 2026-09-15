; RUN: opt -load-pass-plugin=%llvmshlibdir/lifanov_k_llvm_ir_LLVM_IR%pluginext -passes=lifanov_rem_decompose -S %s | FileCheck %s

; 1. FRem
define float @test_frem_f32(float %a, float %b) {
; CHECK-LABEL: @test_frem_f32
; CHECK-NEXT:  %fdiv.tmp = fdiv float %a, %b
; CHECK-NEXT:  %trunc.tmp = call float @llvm.trunc.f32(float %fdiv.tmp)
; CHECK-NEXT:  %fmul.tmp = fmul float %trunc.tmp, %b
; CHECK-NEXT:  %fsub.tmp = fsub float %a, %fmul.tmp
; CHECK-NEXT:  ret float %fsub.tmp
  %rem = frem float %a, %b
  ret float %rem
}

; 2. SRem
define i32 @test_srem_i32(i32 %a, i32 %b) {
; CHECK-LABEL: @test_srem_i32
; CHECK-NEXT:  %sdiv.tmp = sdiv i32 %a, %b
; CHECK-NEXT:  %smul.tmp = mul i32 %sdiv.tmp, %b
; CHECK-NEXT:  %ssub.tmp = sub i32 %a, %smul.tmp
; CHECK-NEXT:  ret i32 %ssub.tmp
  %rem = srem i32 %a, %b
  ret i32 %rem
}

; 3. URem
define i64 @test_urem_i64(i64 %a, i64 %b) {
; CHECK-LABEL: @test_urem_i64
; CHECK-NEXT:  %udiv.tmp = udiv i64 %a, %b
; CHECK-NEXT:  %umul.tmp = mul i64 %udiv.tmp, %b
; CHECK-NEXT:  %usub.tmp = sub i64 %a, %umul.tmp
; CHECK-NEXT:  ret i64 %usub.tmp
  %rem = urem i64 %a, %b
  ret i64 %rem
}

; 4. Vectirs
define <2 x double> @test_frem_vec(<2 x double> %a, <2 x double> %b) {
; CHECK-LABEL: @test_frem_vec
; CHECK-NEXT:  %fdiv.tmp = fdiv <2 x double> %a, %b
; CHECK-NEXT:  %trunc.tmp = call <2 x double> @llvm.trunc.v2f64(<2 x double> %fdiv.tmp)
; CHECK-NEXT:  %fmul.tmp = fmul <2 x double> %trunc.tmp, %b
; CHECK-NEXT:  %fsub.tmp = fsub <2 x double> %a, %fmul.tmp
; CHECK-NEXT:  ret <2 x double> %fsub.tmp
  %rem = frem <2 x double> %a, %b
  ret <2 x double> %rem
}

; 5. Default (ignor)
define i32 @test_ignore_add(i32 %a, i32 %b) {
; CHECK-LABEL: @test_ignore_add
; CHECK-NEXT:  %sum = add i32 %a, %b
; CHECK-NEXT:  ret i32 %sum
  %sum = add i32 %a, %b
  ret i32 %sum
}

; 6. Default (ignor)
define i32 @test_ignore_mul(i32 %a, i32 %b) {
; CHECK-LABEL: @test_ignore_mul
; CHECK-NEXT:  %prod = mul i32 %a, %b
; CHECK-NEXT:  ret i32 %prod
  %prod = mul i32 %a, %b
  ret i32 %prod
}