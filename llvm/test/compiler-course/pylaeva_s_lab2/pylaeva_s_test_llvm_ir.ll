; RUN: opt -load-pass-plugin %llvmshlibdir/pylaeva_s_lab2_LLVM_IR%pluginext\
; RUN: -passes=pylaeva_s_lab2 -S %s | FileCheck %s

; --- frem для float и double ---

; CHECK-LABEL: @_Z13test_frem_floatff
; CHECK:  %1 = fdiv float %a, %b
; CHECK:  %2 = call float @llvm.trunc.f32(float %1)
; CHECK:  %3 = fmul float %2, %b
; CHECK:  %4 = fsub float %a, %3
; CHECK:  ret float %4
define dso_local noundef float @_Z13test_frem_floatff(float noundef %a, float noundef %b) {
  %rem = frem float %a, %b
  ret float %rem
}

; CHECK-LABEL: @_Z14test_frem_doubledd
; CHECK:  %1 = fdiv double %a, %b
; CHECK:  %2 = call double @llvm.trunc.f64(double %1)
; CHECK:  %3 = fmul double %2, %b
; CHECK:  %4 = fsub double %a, %3
; CHECK:  ret double %4
define dso_local noundef double @_Z14test_frem_doubledd(double noundef %a, double noundef %b) {
  %rem = frem double %a, %b
  ret double %rem
}


; --- urem для i8 i16 i32 i64 ---

; CHECK-LABEL: @_Z14test_urem_i8ii
; CHECK:  %1 = udiv i8 %a, %b
; CHECK:  %2 = mul i8 %1, %b
; CHECK:  %3 = sub i8 %a, %2
; CHECK:  ret i8 %3
define dso_local noundef i8 @_Z14test_urem_i8ii(i8 noundef %a, i8 noundef %b) {
  %rem = urem i8 %a, %b
  ret i8 %rem
}

; CHECK-LABEL: @_Z15test_urem_i16ii
; CHECK:  %1 = udiv i16 %a, %b
; CHECK:  %2 = mul i16 %1, %b
; CHECK:  %3 = sub i16 %a, %2
; CHECK:  ret i16 %3
define dso_local noundef i16 @_Z15test_urem_i16ii(i16 noundef %a, i16 noundef %b) {
  %rem = urem i16 %a, %b
  ret i16 %rem
}

; CHECK-LABEL: @_Z13test_urem_i32ii
; CHECK:  %1 = udiv i32 %a, %b
; CHECK:  %2 = mul i32 %1, %b
; CHECK:  %3 = sub i32 %a, %2
; CHECK:  ret i32 %3
define dso_local noundef i32 @_Z13test_urem_i32ii(i32 noundef %a, i32 noundef %b) {
  %rem = urem i32 %a, %b
  ret i32 %rem
}

; CHECK-LABEL: @_Z15test_urem_i64ii
; CHECK:  %1 = udiv i64 %a, %b
; CHECK:  %2 = mul i64 %1, %b
; CHECK:  %3 = sub i64 %a, %2
; CHECK:  ret i64 %3
define dso_local noundef i64 @_Z15test_urem_i64ii(i64 noundef %a, i64 noundef %b) {
  %rem = urem i64 %a, %b
  ret i64 %rem
}


; --- srem для i8 i16 i32 i64 ---

; CHECK-LABEL: @_Z14test_srem_i8ii
; CHECK:  %1 = sdiv i8 %a, %b
; CHECK:  %2 = mul i8 %1, %b
; CHECK:  %3 = sub i8 %a, %2
; CHECK:  ret i8 %3
define dso_local noundef i8 @_Z14test_srem_i8ii(i8 noundef %a, i8 noundef %b) {
  %rem = srem i8 %a, %b
  ret i8 %rem
}

; CHECK-LABEL: @_Z15test_srem_i16ii
; CHECK:  %1 = sdiv i16 %a, %b
; CHECK:  %2 = mul i16 %1, %b
; CHECK:  %3 = sub i16 %a, %2
; CHECK:  ret i16 %3
define dso_local noundef i16 @_Z15test_srem_i16ii(i16 noundef %a, i16 noundef %b) {
  %rem = srem i16 %a, %b
  ret i16 %rem
}

; CHECK-LABEL: @_Z13test_srem_i32ii
; CHECK:  %1 = sdiv i32 %a, %b
; CHECK:  %2 = mul i32 %1, %b
; CHECK:  %3 = sub i32 %a, %2
; CHECK:  ret i32 %3
define dso_local noundef i32 @_Z13test_srem_i32ii(i32 noundef %a, i32 noundef %b) {
  %rem = srem i32 %a, %b
  ret i32 %rem
}

; CHECK-LABEL: @_Z15test_srem_i64ii
; CHECK:  %1 = sdiv i64 %a, %b
; CHECK:  %2 = mul i64 %1, %b
; CHECK:  %3 = sub i64 %a, %2
; CHECK:  ret i64 %3
define dso_local noundef i64 @_Z15test_srem_i64ii(i64 noundef %a, i64 noundef %b) {
  %rem = srem i64 %a, %b
  ret i64 %rem
}


; --- векторные тесты ---

; CHECK-LABEL: @_Z13test_frem_vecDv4_fS_
; CHECK:  %1 = fdiv <4 x float> %a, %b
; CHECK:  %2 = call <4 x float> @llvm.trunc.v4f32(<4 x float> %1)
; CHECK:  %3 = fmul <4 x float> %2, %b
; CHECK:  %4 = fsub <4 x float> %a, %3
; CHECK:  ret <4 x float> %4
define dso_local noundef <4 x float> @_Z13test_frem_vecDv4_fS_(<4 x float> noundef %a, <4 x float> noundef %b) {
  %rem = frem <4 x float> %a, %b
  ret <4 x float> %rem
}

; CHECK-LABEL: @_Z13test_frem_vecDv2_dS_
; CHECK:  %1 = fdiv <2 x double> %a, %b
; CHECK:  %2 = call <2 x double> @llvm.trunc.v2f64(<2 x double> %1)
; CHECK:  %3 = fmul <2 x double> %2, %b
; CHECK:  %4 = fsub <2 x double> %a, %3
; CHECK:  ret <2 x double> %4
define dso_local noundef <2 x double> @_Z13test_frem_vecDv2_dS_(<2 x double> noundef %a, <2 x double> noundef %b) {
  %rem = frem <2 x double> %a, %b
  ret <2 x double> %rem
}

; CHECK-LABEL: @_Z13test_srem_vecDv4_iS_
; CHECK:  %1 = sdiv <4 x i32> %a, %b
; CHECK:  %2 = mul <4 x i32> %1, %b
; CHECK:  %3 = sub <4 x i32> %a, %2
; CHECK:  ret <4 x i32> %3
define dso_local noundef <4 x i32> @_Z13test_srem_vecDv4_iS_(<4 x i32> noundef %a, <4 x i32> noundef %b) {
  %rem = srem <4 x i32> %a, %b
  ret <4 x i32> %rem
}

; CHECK-LABEL: @_Z13test_urem_vecDv4_jS_
; CHECK:  %1 = udiv <4 x i32> %a, %b
; CHECK:  %2 = mul <4 x i32> %1, %b
; CHECK:  %3 = sub <4 x i32> %a, %2
; CHECK:  ret <4 x i32> %3
define dso_local noundef <4 x i32> @_Z13test_urem_vecDv4_jS_(<4 x i32> noundef %a, <4 x i32> noundef %b) {
  %rem = urem <4 x i32> %a, %b
  ret <4 x i32> %rem
}