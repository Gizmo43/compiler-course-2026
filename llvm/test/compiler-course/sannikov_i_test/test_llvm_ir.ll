; RUN: opt -load-pass-plugin %llvmshlibdir/sannikov_i_lab2_LLVM_IR%pluginext \
; RUN: -passes=sanverfmuladddec -S %s | FileCheck %s

; CHECK-LABEL: @only_fmul
; CHECK-NEXT: %mul = fmul float %a, %b
; CHECK-NEXT: ret float %mul
define float @only_fmul(float %a, float %b) {
  %mul = fmul float %a, %b
  ret float %mul
}

; CHECK-LABEL: @only_fadd
; CHECK-NEXT: %add = fadd float %a, %b
; CHECK-NEXT: ret float %add
define float @only_fadd(float %a, float %b) {
  %add = fadd float %a, %b
  ret float %add
}


; CHECK-LABEL: @one_float
; CHECK-NEXT: %fmul = fmul float %a, %b
; CHECK-NEXT: %fadd = fadd float %fmul, %c
; CHECK-NEXT: ret float %fadd
define float @one_float(float %a, float %b, float %c) {
  %res = call float @llvm.fmuladd.f32(float %a, float %b, float %c)
  ret float %res
}

; CHECK-LABEL: @one_double
; CHECK-NEXT: %fmul = fmul double %a, %b
; CHECK-NEXT: %fadd = fadd double %fmul, %c
; CHECK-NEXT: ret double %fadd
define double @one_double(double %a, double %b, double %c) {
  %res = call double @llvm.fmuladd.f64(double %a, double %b, double %c)
  ret double %res
}


; CHECK-LABEL: @one_fast
; CHECK-NEXT: %fmul = fmul fast float %a, %b
; CHECK-NEXT: %fadd = fadd fast float %fmul, %c
; CHECK-NEXT: ret float %fadd
define float @one_fast(float %a, float %b, float %c) {
  %res = call fast float @llvm.fmuladd.f32(float %a, float %b, float %c)
  ret float %res
}


; CHECK-LABEL: @two_float
; CHECK-NEXT: %fmul = fmul float %a, %b
; CHECK-NEXT: %fadd = fadd float %fmul, %c
; CHECK-NEXT: %fmul1 = fmul float %fadd, %b
; CHECK-NEXT: %fadd2 = fadd float %fmul1, %c
; CHECK-NEXT: ret float %fadd2
define float @two_float(float %a, float %b, float %c) {
  %x = call float @llvm.fmuladd.f32(float %a, float %b, float %c)
  %y = call float @llvm.fmuladd.f32(float %x, float %b, float %c)
  ret float %y
}

; CHECK-LABEL: @three_float
; CHECK-NEXT: %fmul = fmul float %a, %b
; CHECK-NEXT: %fadd = fadd float %fmul, %c
; CHECK-NEXT: %fmul1 = fmul float %fadd, %b
; CHECK-NEXT: %fadd2 = fadd float %fmul1, %c
; CHECK-NEXT: %fmul3 = fmul float %fadd2, %a
; CHECK-NEXT: %fadd4 = fadd float %fmul3, %c
; CHECK-NEXT: ret float %fadd4
define float @three_float(float %a, float %b, float %c) {
  %x = call float @llvm.fmuladd.f32(float %a, float %b, float %c)
  %y = call float @llvm.fmuladd.f32(float %x, float %b, float %c)
  %z = call float @llvm.fmuladd.f32(float %y, float %a, float %c)
  ret float %z
}
; CHECK-LABEL: @one_vec8
; CHECK-NEXT: %fmul = fmul <8 x float> %a, %b
; CHECK-NEXT: %fadd = fadd <8 x float> %fmul, %c
; CHECK-NEXT: ret <8 x float> %fadd
define <8 x float> @one_vec8(<8 x float> %a, <8 x float> %b, <8 x float> %c) {
  %res = call <8 x float> @llvm.fmuladd.v4f32(<8 x float> %a, <8 x float> %b, <8 x float> %c)
  ret <8 x float> %res
}
