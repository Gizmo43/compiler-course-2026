; RUN: opt -load-pass-plugin %llvmshlibdir/chyokotov_a_decompose_frem_LLVM_IR%pluginext\
; RUN: -passes=chyokotov_a_frem -S %s | FileCheck %s

; CHECK-LABEL: define dso_local noundef i32 @_Z9test_sremii(i32 noundef %a, i32 noundef %b)
; CHECK:   %0 = sdiv i32 %a, %b
; CHECK:   %1 = mul i32 %0, %b
; CHECK:   %2 = sub i32 %a, %1
; CHECK:   ret i32 %2

define dso_local noundef i32 @_Z9test_sremii(i32 noundef %a, i32 noundef %b) {
entry:
  %rem = srem i32 %a, %b
  ret i32 %rem
}

; CHECK-LABEL: define dso_local noundef i32 @_Z9test_uremjj(i32 noundef %a, i32 noundef %b)
; CHECK:   %0 = udiv i32 %a, %b
; CHECK:   %1 = mul i32 %0, %b
; CHECK:   %2 = sub i32 %a, %1
; CHECK:   ret i32 %2

define dso_local noundef i32 @_Z9test_uremjj(i32 noundef %a, i32 noundef %b) {
entry:
  %rem = urem i32 %a, %b
  ret i32 %rem
}

; CHECK-LABEL: define dso_local noundef i32 @_Z9test_fremdd(double noundef %a, double noundef %b)
; CHECK:   %0 = fdiv double %a, %b
; CHECK:   %1 = call double @llvm.trunc.f64(double %0)
; CHECK:   %2 = fmul double %1, %b
; CHECK:   %3 = fsub double %a, %2
; CHECK:   %conv = fptosi double %3 to i32
; CHECK:   ret i32 %conv

define dso_local noundef i32 @_Z9test_fremdd(double noundef %a, double noundef %b) {
entry:
  %fmod = frem double %a, %b
  %conv = fptosi double %fmod to i32
  ret i32 %conv
}

; CHECK-LABEL: define dso_local noundef <4 x i32> @_Z13test_vec_sremDv4_iS_(<4 x i32> noundef %a, <4 x i32> noundef %b)
; CHECK:   %0 = sdiv <4 x i32> %a, %b
; CHECK:   %1 = mul <4 x i32> %0, %b
; CHECK:   %2 = sub <4 x i32> %a, %1
; CHECK:   ret <4 x i32> %2

define dso_local noundef <4 x i32> @_Z13test_vec_sremDv4_iS_(<4 x i32> noundef %a, <4 x i32> noundef %b) {
entry:
  %rem = srem <4 x i32> %a, %b
  ret <4 x i32> %rem
}

; CHECK-LABEL: define dso_local noundef <4 x i32> @_Z13test_vec_uremDv4_jS_(<4 x i32> noundef %a, <4 x i32> noundef %b)
; CHECK:  %0 = udiv <4 x i32> %a, %b
; CHECK:  %1 = mul <4 x i32> %0, %b
; CHECK:  %2 = sub <4 x i32> %a, %1
; CHECK:  ret <4 x i32> %2

define dso_local noundef <4 x i32> @_Z13test_vec_uremDv4_jS_(<4 x i32> noundef %a, <4 x i32> noundef %b) {
entry:
  %rem = urem <4 x i32> %a, %b
  ret <4 x i32> %rem
}
