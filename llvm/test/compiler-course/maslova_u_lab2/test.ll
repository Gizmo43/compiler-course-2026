; RUN: opt -load-pass-plugin=%llvmshlibdir/maslova_u_lab2_LLVM_IR%pluginext -passes=decompose-rem -S %s | FileCheck %s

define i32 @test_srem_i32(i32 %a, i32 %b) {
; CHECK-LABEL: @test_srem_i32(i32 %a, i32 %b)
; CHECK-NEXT: %[[D:.+]] = sdiv i32 %a, %b
; CHECK-NEXT: %[[M:.+]] = mul i32 %[[D]], %b
; CHECK-NEXT: %[[S:.+]] = sub i32 %a, %[[M]]
; CHECK-NEXT: ret i32 %[[S]]
  %res = srem i32 %a, %b
  ret i32 %res
}

define i8 @test_urem_i8(i8 %a, i8 %b) {
; CHECK-LABEL: @test_urem_i8(i8 %a, i8 %b)
; CHECK-NEXT: %[[D:.+]] = udiv i8 %a, %b
; CHECK-NEXT: %[[M:.+]] = mul i8 %[[D]], %b
; CHECK-NEXT: %[[S:.+]] = sub i8 %a, %[[M]]
; CHECK-NEXT: ret i8 %[[S]]
  %res = urem i8 %a, %b
  ret i8 %res
}

define float @test_frem_float(float %a, float %b) {
; CHECK-LABEL: @test_frem_float(float %a, float %b)
; CHECK-NEXT: %[[D:.+]] = fdiv float %a, %b
; CHECK-NEXT: %[[T:.+]] = call float @llvm.trunc.f32(float %[[D]])
; CHECK-NEXT: %[[M:.+]] = fmul float %[[T]], %b
; CHECK-NEXT: %[[S:.+]] = fsub float %a, %[[M]]
; CHECK-NEXT: ret float %[[S]]
  %res = frem float %a, %b
  ret float %res
}

define double @test_frem_double(double %a, double %b) {
; CHECK-LABEL: @test_frem_double(double %a, double %b)
; CHECK-NEXT: %[[D:.+]] = fdiv double %a, %b
; CHECK-NEXT: %[[T:.+]] = call double @llvm.trunc.f64(double %[[D]])
; CHECK-NEXT: %[[M:.+]] = fmul double %[[T]], %b
; CHECK-NEXT: %[[S:.+]] = fsub double %a, %[[M]]
; CHECK-NEXT: ret double %[[S]]
  %res = frem double %a, %b
  ret double %res
}

define <4 x i32> @test_vec_srem(<4 x i32> %a, <4 x i32> %b) {
; CHECK-LABEL: @test_vec_srem(<4 x i32> %a, <4 x i32> %b)
; CHECK-NEXT: %[[D:.+]] = sdiv <4 x i32> %a, %b
; CHECK-NEXT: %[[M:.+]] = mul <4 x i32> %[[D]], %b
; CHECK-NEXT: %[[S:.+]] = sub <4 x i32> %a, %[[M]]
; CHECK-NEXT: ret <4 x i32> %[[S]]
  %res = srem <4 x i32> %a, %b
  ret <4 x i32> %res
}

define <2 x float> @test_vec_frem(<2 x float> %a, <2 x float> %b) {
; CHECK-LABEL: @test_vec_frem(<2 x float> %a, <2 x float> %b)
; CHECK-NEXT: %[[D:.+]] = fdiv <2 x float> %a, %b
; CHECK-NEXT: %[[T:.+]] = call <2 x float> @llvm.trunc.v2f32(<2 x float> %[[D]])
; CHECK-NEXT: %[[M:.+]] = fmul <2 x float> %[[T]], %b
; CHECK-NEXT: %[[S:.+]] = fsub <2 x float> %a, %[[M]]
; CHECK-NEXT: ret <2 x float> %[[S]]
  %res = frem <2 x float> %a, %b
  ret <2 x float> %res
}

define i32 @test_srem_constant(i32 %a) {
; CHECK-LABEL: @test_srem_constant(i32 %a)
; CHECK-NEXT: %[[D:.+]] = sdiv i32 %a, 10
; CHECK-NEXT: %[[M:.+]] = mul i32 %[[D]], 10
; CHECK-NEXT: %[[S:.+]] = sub i32 %a, %[[M]]
; CHECK-NEXT: ret i32 %[[S]]
  %res = srem i32 %a, 10
  ret i32 %res
}

define i32 @test_chained_rem(i32 %a, i32 %b, i32 %c) {
; CHECK-LABEL: @test_chained_rem(i32 %a, i32 %b, i32 %c)
; CHECK-NEXT: %[[D1:.+]] = sdiv i32 %a, %b
; CHECK-NEXT: %[[M1:.+]] = mul i32 %[[D1]], %b
; CHECK-NEXT: %[[S1:.+]] = sub i32 %a, %[[M1]]
; CHECK-NEXT: %[[D2:.+]] = sdiv i32 %[[S1]], %c
; CHECK-NEXT: %[[M2:.+]] = mul i32 %[[D2]], %c
; CHECK-NEXT: %[[S2:.+]] = sub i32 %[[S1]], %[[M2]]
; CHECK-NEXT: ret i32 %[[S2]]
  %1 = srem i32 %a, %b
  %2 = srem i32 %1, %c
  ret i32 %2
}
