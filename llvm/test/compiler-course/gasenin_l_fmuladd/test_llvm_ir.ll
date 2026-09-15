; RUN: opt -load-pass-plugin %llvmshlibdir/fmuladd_LLVM_IR%pluginext \
; RUN:   -passes=fmuladd-decompose -S %s | FileCheck %s


; CHECK-LABEL: define float @test_f32(
; CHECK-NOT:   call float @llvm.fmuladd
; CHECK:       %fmuladd.mul = fmul float %a, %b
; CHECK-NEXT:  %fmuladd.add = fadd float %fmuladd.mul, %c
; CHECK-NEXT:  ret float %fmuladd.add
define float @test_f32(float %a, float %b, float %c) {
  %r = call float @llvm.fmuladd.f32(float %a, float %b, float %c)
  ret float %r
}

; CHECK-LABEL: define double @test_f64(
; CHECK-NOT:   call double @llvm.fmuladd
; CHECK:       %fmuladd.mul = fmul double %a, %b
; CHECK-NEXT:  %fmuladd.add = fadd double %fmuladd.mul, %c
; CHECK-NEXT:  ret double %fmuladd.add
define double @test_f64(double %a, double %b, double %c) {
  %r = call double @llvm.fmuladd.f64(double %a, double %b, double %c)
  ret double %r
}

; CHECK-LABEL: define half @test_f16(
; CHECK-NOT:   call half @llvm.fmuladd
; CHECK:       %fmuladd.mul = fmul half %a, %b
; CHECK-NEXT:  %fmuladd.add = fadd half %fmuladd.mul, %c
; CHECK-NEXT:  ret half %fmuladd.add
define half @test_f16(half %a, half %b, half %c) {
  %r = call half @llvm.fmuladd.f16(half %a, half %b, half %c)
  ret half %r
}


; CHECK-LABEL: define <4 x float> @test_vec4xf32(
; CHECK-NOT:   call <4 x float> @llvm.fmuladd
; CHECK:       %fmuladd.mul = fmul <4 x float> %a, %b
; CHECK-NEXT:  %fmuladd.add = fadd <4 x float> %fmuladd.mul, %c
; CHECK-NEXT:  ret <4 x float> %fmuladd.add
define <4 x float> @test_vec4xf32(<4 x float> %a, <4 x float> %b, <4 x float> %c) {
  %r = call <4 x float> @llvm.fmuladd.v4f32(<4 x float> %a, <4 x float> %b, <4 x float> %c)
  ret <4 x float> %r
}

; CHECK-LABEL: define <2 x double> @test_vec2xf64(
; CHECK-NOT:   call <2 x double> @llvm.fmuladd
; CHECK:       %fmuladd.mul = fmul <2 x double> %a, %b
; CHECK-NEXT:  %fmuladd.add = fadd <2 x double> %fmuladd.mul, %c
; CHECK-NEXT:  ret <2 x double> %fmuladd.add
define <2 x double> @test_vec2xf64(<2 x double> %a, <2 x double> %b, <2 x double> %c) {
  %r = call <2 x double> @llvm.fmuladd.v2f64(<2 x double> %a, <2 x double> %b, <2 x double> %c)
  ret <2 x double> %r
}


; Составной флаг fast.
; CHECK-LABEL: define float @test_fastmath_fast(
; CHECK:       %fmuladd.mul = fmul fast float %a, %b
; CHECK-NEXT:  %fmuladd.add = fadd fast float %fmuladd.mul, %c
define float @test_fastmath_fast(float %a, float %b, float %c) {
  %r = call fast float @llvm.fmuladd.f32(float %a, float %b, float %c)
  ret float %r
}

; Флаг nnan.
; CHECK-LABEL: define float @test_fastmath_nnan(
; CHECK:       %fmuladd.mul = fmul nnan float %a, %b
; CHECK-NEXT:  %fmuladd.add = fadd nnan float %fmuladd.mul, %c
define float @test_fastmath_nnan(float %a, float %b, float %c) {
  %r = call nnan float @llvm.fmuladd.f32(float %a, float %b, float %c)
  ret float %r
}

; Флаг contract — семантически ближайший к fmuladd.
; CHECK-LABEL: define float @test_fastmath_contract(
; CHECK:       %fmuladd.mul = fmul contract float %a, %b
; CHECK-NEXT:  %fmuladd.add = fadd contract float %fmuladd.mul, %c
define float @test_fastmath_contract(float %a, float %b, float %c) {
  %r = call contract float @llvm.fmuladd.f32(float %a, float %b, float %c)
  ret float %r
}

; Без флагов — инструкции тоже без флагов (не навешиваем лишнего).
; CHECK-LABEL: define float @test_no_flags(
; CHECK:       %fmuladd.mul = fmul float %a, %b
; CHECK-NEXT:  %fmuladd.add = fadd float %fmuladd.mul, %c
define float @test_no_flags(float %a, float %b, float %c) {
  %r = call float @llvm.fmuladd.f32(float %a, float %b, float %c)
  ret float %r
}


; CHECK-LABEL: define float @test_result_used_in_expr(
; CHECK:       %fmuladd.mul = fmul float %a, %b
; CHECK:       %fmuladd.add = fadd float %fmuladd.mul, %c
; CHECK:       %extra = fmul float %fmuladd.add, %d
; CHECK-NOT:   call float @llvm.fmuladd
define float @test_result_used_in_expr(float %a, float %b, float %c, float %d) {
  %r = call float @llvm.fmuladd.f32(float %a, float %b, float %c)
  %extra = fmul float %r, %d
  ret float %extra
}


; Оба use-сайта %r должны увидеть %fmuladd.add после replaceAllUsesWith.
; CHECK-LABEL: define float @test_multiple_uses(
; CHECK:       %fmuladd.add = fadd float %fmuladd.mul, %c
; CHECK:       %x = fadd float %fmuladd.add, %fmuladd.add
; CHECK-NOT:   call float @llvm.fmuladd
define float @test_multiple_uses(float %a, float %b, float %c) {
  %r = call float @llvm.fmuladd.f32(float %a, float %b, float %c)
  %x = fadd float %r, %r
  ret float %x
}


; CHECK-LABEL: define float @test_chain(
; CHECK:       %fmuladd.mul = fmul float %a, %b
; CHECK-NEXT:  %fmuladd.add = fadd float %fmuladd.mul, %c
; CHECK-NEXT:  %fmuladd.mul1 = fmul float %fmuladd.add, %d
; CHECK-NEXT:  %fmuladd.add2 = fadd float %fmuladd.mul1, %c
; CHECK-NOT:   call float @llvm.fmuladd
define float @test_chain(float %a, float %b, float %c, float %d) {
  %r1 = call float @llvm.fmuladd.f32(float %a, float %b, float %c)
  %r2 = call float @llvm.fmuladd.f32(float %r1, float %d, float %c)
  ret float %r2
}


; CHECK-LABEL: define float @test_no_fmuladd(
; CHECK-NEXT:  %r = fmul float %a, %b
; CHECK-NEXT:  ret float %r
define float @test_no_fmuladd(float %a, float %b) {
  %r = fmul float %a, %b
  ret float %r
}


declare float        @llvm.fmuladd.f32(float, float, float)
declare double       @llvm.fmuladd.f64(double, double, double)
declare half         @llvm.fmuladd.f16(half, half, half)
declare <4 x float>  @llvm.fmuladd.v4f32(<4 x float>, <4 x float>, <4 x float>)
declare <2 x double> @llvm.fmuladd.v2f64(<2 x double>, <2 x double>, <2 x double>)


; CHECK-LABEL: define float @test_fastmath_ninf(
; CHECK:       %fmuladd.mul = fmul ninf float %a, %b
; CHECK-NEXT:  %fmuladd.add = fadd ninf float %fmuladd.mul, %c
define float @test_fastmath_ninf(float %a, float %b, float %c) {
  %r = call ninf float @llvm.fmuladd.f32(float %a, float %b, float %c)
  ret float %r
}

; CHECK-LABEL: define float @test_fastmath_nsz(
; CHECK:       %fmuladd.mul = fmul nsz float %a, %b
; CHECK-NEXT:  %fmuladd.add = fadd nsz float %fmuladd.mul, %c
define float @test_fastmath_nsz(float %a, float %b, float %c) {
  %r = call nsz float @llvm.fmuladd.f32(float %a, float %b, float %c)
  ret float %r
}

; CHECK-LABEL: define float @test_fastmath_arcp(
; CHECK:       %fmuladd.mul = fmul arcp float %a, %b
; CHECK-NEXT:  %fmuladd.add = fadd arcp float %fmuladd.mul, %c
define float @test_fastmath_arcp(float %a, float %b, float %c) {
  %r = call arcp float @llvm.fmuladd.f32(float %a, float %b, float %c)
  ret float %r
}

; CHECK-LABEL: define float @test_fastmath_afn(
; CHECK:       %fmuladd.mul = fmul afn float %a, %b
; CHECK-NEXT:  %fmuladd.add = fadd afn float %fmuladd.mul, %c
define float @test_fastmath_afn(float %a, float %b, float %c) {
  %r = call afn float @llvm.fmuladd.f32(float %a, float %b, float %c)
  ret float %r
}

; CHECK-LABEL: define float @test_fastmath_reassoc(
; CHECK:       %fmuladd.mul = fmul reassoc float %a, %b
; CHECK-NEXT:  %fmuladd.add = fadd reassoc float %fmuladd.mul, %c
define float @test_fastmath_reassoc(float %a, float %b, float %c) {
  %r = call reassoc float @llvm.fmuladd.f32(float %a, float %b, float %c)
  ret float %r
}


; CHECK-LABEL: define float @test_fastmath_mixed(
; CHECK:       %fmuladd.mul = fmul nnan nsz arcp float %a, %b
; CHECK-NEXT:  %fmuladd.add = fadd nnan nsz arcp float %fmuladd.mul, %c
define float @test_fastmath_mixed(float %a, float %b, float %c) {
  %r = call nnan nsz arcp float @llvm.fmuladd.f32(float %a, float %b, float %c)
  ret float %r
}


; CHECK-LABEL: define <4 x float> @test_vec_fastmath(
; CHECK:       %fmuladd.mul = fmul fast <4 x float> %a, %b
; CHECK-NEXT:  %fmuladd.add = fadd fast <4 x float> %fmuladd.mul, %c
define <4 x float> @test_vec_fastmath(<4 x float> %a, <4 x float> %b, <4 x float> %c) {
  %r = call fast <4 x float> @llvm.fmuladd.v4f32(<4 x float> %a, <4 x float> %b, <4 x float> %c)
  ret <4 x float> %r
}

; CHECK-LABEL: define <2 x double> @test_vec_contract(
; CHECK:       %fmuladd.mul = fmul contract <2 x double> %a, %b
; CHECK-NEXT:  %fmuladd.add = fadd contract <2 x double> %fmuladd.mul, %c
define <2 x double> @test_vec_contract(<2 x double> %a, <2 x double> %b, <2 x double> %c) {
  %r = call contract <2 x double> @llvm.fmuladd.v2f64(<2 x double> %a, <2 x double> %b, <2 x double> %c)
  ret <2 x double> %r
}


; CHECK-LABEL: define float @test_branches(
; CHECK:       %fmuladd.mul = fmul float %a, %b
; CHECK-NEXT:  %fmuladd.add = fadd float %fmuladd.mul, %c
; CHECK:       %fmuladd.mul1 = fmul float %b, %c
; CHECK-NEXT:  %fmuladd.add2 = fadd float %fmuladd.mul1, %a
; CHECK-NOT:   call float @llvm.fmuladd
define float @test_branches(float %a, float %b, float %c, i1 %cond) {
entry:
  br i1 %cond, label %then, label %else
then:
  %r1 = call float @llvm.fmuladd.f32(float %a, float %b, float %c)
  br label %merge
else:
  %r2 = call float @llvm.fmuladd.f32(float %b, float %c, float %a)
  br label %merge
merge:
  %result = phi float [ %r1, %then ], [ %r2, %else ]
  ret float %result
}


; CHECK-LABEL: define float @test_phi(
; CHECK:       %result = phi float [ %a, %entry ], [ %fmuladd.add, %loop ]
; CHECK-NEXT:  %fmuladd.mul = fmul float %a, %b
; CHECK-NEXT:  %fmuladd.add = fadd float %fmuladd.mul, %c
; CHECK-NOT:   call float @llvm.fmuladd
define float @test_phi(float %a, float %b, float %c, i1 %cond) {
entry:
  br label %loop
loop:
  %result = phi float [ %a, %entry ], [ %r, %loop ]
  %r = call float @llvm.fmuladd.f32(float %a, float %b, float %c)
  br i1 %cond, label %loop, label %exit
exit:
  ret float %result
}


; CHECK-LABEL: define float @test_independent(
; CHECK:       %fmuladd.mul = fmul float %a, %b
; CHECK-NEXT:  %fmuladd.add = fadd float %fmuladd.mul, %c
; CHECK:       %fmuladd.mul1 = fmul float %c, %d
; CHECK-NEXT:  %fmuladd.add2 = fadd float %fmuladd.mul1, %a
; CHECK-NOT:   call float @llvm.fmuladd
define float @test_independent(float %a, float %b, float %c, float %d) {
  %r1 = call float @llvm.fmuladd.f32(float %a, float %b, float %c)
  %r2 = call float @llvm.fmuladd.f32(float %c, float %d, float %a)
  %result = fadd float %r1, %r2
  ret float %result
}


; CHECK-LABEL: define fp128 @test_fp128(
; CHECK-NOT:   call fp128 @llvm.fmuladd
; CHECK:       %fmuladd.mul = fmul fp128 %a, %b
; CHECK-NEXT:  %fmuladd.add = fadd fp128 %fmuladd.mul, %c
define fp128 @test_fp128(fp128 %a, fp128 %b, fp128 %c) {
  %r = call fp128 @llvm.fmuladd.f128(fp128 %a, fp128 %b, fp128 %c)
  ret fp128 %r
}

; CHECK-LABEL: define x86_fp80 @test_x86_fp80(
; CHECK-NOT:   call x86_fp80 @llvm.fmuladd
; CHECK:       %fmuladd.mul = fmul x86_fp80 %a, %b
; CHECK-NEXT:  %fmuladd.add = fadd x86_fp80 %fmuladd.mul, %c
define x86_fp80 @test_x86_fp80(x86_fp80 %a, x86_fp80 %b, x86_fp80 %c) {
  %r = call x86_fp80 @llvm.fmuladd.f80(x86_fp80 %a, x86_fp80 %b, x86_fp80 %c)
  ret x86_fp80 %r
}

declare fp128    @llvm.fmuladd.f128(fp128, fp128, fp128)
declare x86_fp80 @llvm.fmuladd.f80(x86_fp80, x86_fp80, x86_fp80)
