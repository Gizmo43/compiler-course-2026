; RUN: opt -load-pass-plugin %llvmshlibdir/romanov_a_fmuladd_decompose_LLVM_IR%pluginext \
; RUN: -passes=romanov_a_fmuladd_decompose -S %s | FileCheck %s

define float @test_no_fmuladd(float %a, float %b) {
; CHECK-LABEL: @test_no_fmuladd
; CHECK-NEXT: %res = fmul float %a, %b
; CHECK-NEXT: ret float %res
  %res = fmul float %a, %b
  ret float %res
}

define half @test_f16(half %a, half %b, half %c) {
; CHECK-LABEL: @test_f16
; CHECK-NEXT: %decomp.mul = fmul half %a, %b
; CHECK-NEXT: %decomp.add = fadd half %decomp.mul, %c
; CHECK-NEXT: ret half %decomp.add
  %res = call half @llvm.fmuladd.f16(half %a, half %b, half %c)
  ret half %res
}

define float @test_f32(float %a, float %b, float %c) {
; CHECK-LABEL: @test_f32
; CHECK-NEXT: %decomp.mul = fmul float %a, %b
; CHECK-NEXT: %decomp.add = fadd float %decomp.mul, %c
; CHECK-NEXT: ret float %decomp.add
  %res = call float @llvm.fmuladd.f32(float %a, float %b, float %c)
  ret float %res
}

define double @test_f64(double %a, double %b, double %c) {
; CHECK-LABEL: @test_f64
; CHECK-NEXT: %decomp.mul = fmul double %a, %b
; CHECK-NEXT: %decomp.add = fadd double %decomp.mul, %c
; CHECK-NEXT: ret double %decomp.add
  %res = call double @llvm.fmuladd.f64(double %a, double %b, double %c)
  ret double %res
}

define fp128 @test_f128(fp128 %a, fp128 %b, fp128 %c) {
; CHECK-LABEL: @test_f128
; CHECK-NEXT: %decomp.mul = fmul fp128 %a, %b
; CHECK-NEXT: %decomp.add = fadd fp128 %decomp.mul, %c
; CHECK-NEXT: ret fp128 %decomp.add
  %res = call fp128 @llvm.fmuladd.f128(fp128 %a, fp128 %b, fp128 %c)
  ret fp128 %res
}

define float @test_multiple_calls(float %a, float %b, float %c) {
; CHECK-LABEL: @test_multiple_calls
; CHECK-NEXT: %decomp.mul = fmul float %a, %b
; CHECK-NEXT: %decomp.add = fadd float %decomp.mul, %c
; CHECK-NEXT: %decomp.mul1 = fmul float %decomp.add, %a
; CHECK-NEXT: %decomp.add2 = fadd float %decomp.mul1, %b
; CHECK-NEXT: ret float %decomp.add2
  %res1 = call float @llvm.fmuladd.f32(float %a, float %b, float %c)
  %res2 = call float @llvm.fmuladd.f32(float %res1, float %a, float %b)
  ret float %res2
}

define float @test_independent(float %a, float %b, float %c, float %d) {
; CHECK-LABEL: @test_independent
; CHECK-NEXT: %decomp.mul = fmul float %a, %b
; CHECK-NEXT: %decomp.add = fadd float %decomp.mul, %c
; CHECK-NEXT: %decomp.mul1 = fmul float %c, %d
; CHECK-NEXT: %decomp.add2 = fadd float %decomp.mul1, %a
; CHECK-NEXT: %res3 = fadd float %decomp.add, %decomp.add2
; CHECK-NEXT: ret float %res3
  %res1 = call float @llvm.fmuladd.f32(float %a, float %b, float %c)
  %res2 = call float @llvm.fmuladd.f32(float %c, float %d, float %a)
  %res3 = fadd float %res1, %res2
  ret float %res3
}

define float @test_flags_fast(float %a, float %b, float %c) {
; CHECK-LABEL: @test_flags_fast
; CHECK-NEXT: %decomp.mul = fmul fast float %a, %b
; CHECK-NEXT: %decomp.add = fadd fast float %decomp.mul, %c
; CHECK-NEXT: ret float %decomp.add
  %res = call fast float @llvm.fmuladd.f32(float %a, float %b, float %c)
  ret float %res
}

define float @test_flags_reassoc(float %a, float %b, float %c) {
; CHECK-LABEL: @test_flags_reassoc
; CHECK-NEXT: %decomp.mul = fmul reassoc float %a, %b
; CHECK-NEXT: %decomp.add = fadd reassoc float %decomp.mul, %c
; CHECK-NEXT: ret float %decomp.add
  %res = call reassoc float @llvm.fmuladd.f32(float %a, float %b, float %c)
  ret float %res
}

define float @test_flags_nnan(float %a, float %b, float %c) {
; CHECK-LABEL: @test_flags_nnan
; CHECK-NEXT: %decomp.mul = fmul nnan float %a, %b
; CHECK-NEXT: %decomp.add = fadd nnan float %decomp.mul, %c
; CHECK-NEXT: ret float %decomp.add
  %res = call nnan float @llvm.fmuladd.f32(float %a, float %b, float %c)
  ret float %res
}

define float @test_flags_ninf(float %a, float %b, float %c) {
; CHECK-LABEL: @test_flags_ninf
; CHECK-NEXT: %decomp.mul = fmul ninf float %a, %b
; CHECK-NEXT: %decomp.add = fadd ninf float %decomp.mul, %c
; CHECK-NEXT: ret float %decomp.add
  %res = call ninf float @llvm.fmuladd.f32(float %a, float %b, float %c)
  ret float %res
}

define float @test_flags_nsz(float %a, float %b, float %c) {
; CHECK-LABEL: @test_flags_nsz
; CHECK-NEXT: %decomp.mul = fmul nsz float %a, %b
; CHECK-NEXT: %decomp.add = fadd nsz float %decomp.mul, %c
; CHECK-NEXT: ret float %decomp.add
  %res = call nsz float @llvm.fmuladd.f32(float %a, float %b, float %c)
  ret float %res
}

define float @test_flags_arcp(float %a, float %b, float %c) {
; CHECK-LABEL: @test_flags_arcp
; CHECK-NEXT: %decomp.mul = fmul arcp float %a, %b
; CHECK-NEXT: %decomp.add = fadd arcp float %decomp.mul, %c
; CHECK-NEXT: ret float %decomp.add
  %res = call arcp float @llvm.fmuladd.f32(float %a, float %b, float %c)
  ret float %res
}

define float @test_flags_contract(float %a, float %b, float %c) {
; CHECK-LABEL: @test_flags_contract
; CHECK-NEXT: %decomp.mul = fmul contract float %a, %b
; CHECK-NEXT: %decomp.add = fadd contract float %decomp.mul, %c
; CHECK-NEXT: ret float %decomp.add
  %res = call contract float @llvm.fmuladd.f32(float %a, float %b, float %c)
  ret float %res
}

define float @test_flags_afn(float %a, float %b, float %c) {
; CHECK-LABEL: @test_flags_afn
; CHECK-NEXT: %decomp.mul = fmul afn float %a, %b
; CHECK-NEXT: %decomp.add = fadd afn float %decomp.mul, %c
; CHECK-NEXT: ret float %decomp.add
  %res = call afn float @llvm.fmuladd.f32(float %a, float %b, float %c)
  ret float %res
}

define float @test_flags_mixed(float %a, float %b, float %c) {
; CHECK-LABEL: @test_flags_mixed
; CHECK-NEXT: %decomp.mul = fmul nnan nsz arcp float %a, %b
; CHECK-NEXT: %decomp.add = fadd nnan nsz arcp float %decomp.mul, %c
; CHECK-NEXT: ret float %decomp.add
  %res = call nnan nsz arcp float @llvm.fmuladd.f32(float %a, float %b, float %c)
  ret float %res
}

define float @test_if(float %a, float %b, float %c, i1 %cond) {
; CHECK-LABEL: @test_if
entry:
  br i1 %cond, label %then, label %else
then:
; CHECK: %decomp.mul = fmul float %a, %b
; CHECK-NEXT: %decomp.add = fadd float %decomp.mul, %c
  %res1 = call float @llvm.fmuladd.f32(float %a, float %b, float %c)
  br label %merge
else:
; CHECK: %decomp.mul1 = fmul float %a, %c
; CHECK-NEXT: %decomp.add2 = fadd float %decomp.mul1, %b
  %res2 = call float @llvm.fmuladd.f32(float %a, float %c, float %b)
  br label %merge
merge:
  %res3 = phi float [ %res1, %then ], [ %res2, %else ]
  ret float %res3
}

define <8 x float> @test_vec8xf32(<8 x float> %a, <8 x float> %b, <8 x float> %c) {
; CHECK-LABEL: @test_vec8xf32
; CHECK-NEXT: %decomp.mul = fmul <8 x float> %a, %b
; CHECK-NEXT: %decomp.add = fadd <8 x float> %decomp.mul, %c
; CHECK-NEXT: ret <8 x float> %decomp.add
  %res = call <8 x float> @llvm.fmuladd.v8f32(<8 x float> %a, <8 x float> %b, <8 x float> %c)
  ret <8 x float> %res
}

define <4 x double> @test_vec4xf64(<4 x double> %a, <4 x double> %b, <4 x double> %c) {
; CHECK-LABEL: @test_vec4xf64
; CHECK-NEXT: %decomp.mul = fmul <4 x double> %a, %b
; CHECK-NEXT: %decomp.add = fadd <4 x double> %decomp.mul, %c
; CHECK-NEXT: ret <4 x double> %decomp.add
  %res = call <4 x double> @llvm.fmuladd.v4f64(<4 x double> %a, <4 x double> %b, <4 x double> %c)
  ret <4 x double> %res
}

declare half  @llvm.fmuladd.f16(half, half, half)
declare float @llvm.fmuladd.f32(float, float, float)
declare double @llvm.fmuladd.f64(double, double, double)
declare fp128 @llvm.fmuladd.f128(fp128, fp128, fp128)

declare <8 x float>  @llvm.fmuladd.v8f32(<8 x float>, <8 x float>, <8 x float>)
declare <4 x double> @llvm.fmuladd.v4f64(<4 x double>, <4 x double>, <4 x double>)