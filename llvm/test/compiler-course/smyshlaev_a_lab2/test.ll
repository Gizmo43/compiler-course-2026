; RUN: opt -load-pass-plugin %llvmshlibdir/smyshlaev_a_lab2_LLVM_IR%pluginext\
; RUN: -passes=mulshift -S %s | FileCheck %s

define i8 @_Z6mul_i8c(i8 noundef %a) {
; CHECK-LABEL: @_Z6mul_i8c
; CHECK-NEXT:  %1 = shl i8 %a, 3
; CHECK-NEXT:  ret i8 %1
  %mul = mul i8 %a, 8
  ret i8 %mul
}

define i8 @_Z7udiv_i8h(i8 noundef %a) {
; CHECK-LABEL: @_Z7udiv_i8h
; CHECK-NEXT:  %1 = lshr i8 %a, 2
; CHECK-NEXT:  ret i8 %1
  %div = udiv i8 %a, 4
  ret i8 %div
}

define i8 @_Z7sdiv_i8c(i8 noundef %a) {
; CHECK-LABEL: @_Z7sdiv_i8c
; CHECK-NEXT:  %1 = icmp slt i8 %a, 0
; CHECK-NEXT:  %2 = add i8 %a, 31
; CHECK-NEXT:  %3 = select i1 %1, i8 %2, i8 %a
; CHECK-NEXT:  %4 = ashr i8 %3, 5
; CHECK-NEXT:  ret i8 %4
  %div = sdiv i8 %a, 32
  ret i8 %div
}

define i16 @_Z7mul_i16s(i16 noundef %a) {
; CHECK-LABEL: @_Z7mul_i16s
; CHECK-NEXT:  %1 = shl i16 %a, 3
; CHECK-NEXT:  ret i16 %1
  %mul = mul i16 %a, 8
  ret i16 %mul
}

define i16 @_Z8udiv_i16t(i16 noundef %a) {
; CHECK-LABEL: @_Z8udiv_i16t
; CHECK-NEXT:  %1 = lshr i16 %a, 2
; CHECK-NEXT:  ret i16 %1
  %div = udiv i16 %a, 4
  ret i16 %div
}

define i16 @_Z8sdiv_i16s(i16 noundef %a) {
; CHECK-LABEL: @_Z8sdiv_i16s
; CHECK-NEXT:  %1 = icmp slt i16 %a, 0
; CHECK-NEXT:  %2 = add i16 %a, 31
; CHECK-NEXT:  %3 = select i1 %1, i16 %2, i16 %a
; CHECK-NEXT:  %4 = ashr i16 %3, 5
; CHECK-NEXT:  ret i16 %4
  %div = sdiv i16 %a, 32
  ret i16 %div
}

define i32 @_Z7mul_rhsi(i32 noundef %a) {
; CHECK-LABEL: @_Z7mul_rhsi
; CHECK-NEXT:  %1 = shl i32 %a, 3
; CHECK-NEXT:  ret i32 %1
  %mul = mul nsw i32 %a, 8
  ret i32 %mul
}

define i32 @_Z7mul_lhsi(i32 noundef %a) {
; CHECK-LABEL: @_Z7mul_lhsi
; CHECK-NEXT:  %1 = shl i32 %a, 4
; CHECK-NEXT:  ret i32 %1
  %mul = mul nsw i32 16, %a
  ret i32 %mul
}

define i32 @_Z9udiv_testj(i32 noundef %a) {
; CHECK-LABEL: @_Z9udiv_testj
; CHECK-NEXT:  %1 = lshr i32 %a, 2
; CHECK-NEXT:  ret i32 %1
  %div = udiv i32 %a, 4
  ret i32 %div
}

define i32 @_Z9sdiv_testi(i32 noundef %a) {
; CHECK-LABEL: @_Z9sdiv_testi
; CHECK-NEXT:  %1 = icmp slt i32 %a, 0
; CHECK-NEXT:  %2 = add i32 %a, 31
; CHECK-NEXT:  %3 = select i1 %1, i32 %2, i32 %a
; CHECK-NEXT:  %4 = ashr i32 %3, 5
; CHECK-NEXT:  ret i32 %4
  %div = sdiv i32 %a, 32
  ret i32 %div
}

define i32 @_Z12mul_not_pow2i(i32 noundef %a) {
; CHECK-LABEL: @_Z12mul_not_pow2i
; CHECK-NEXT:  %mul = mul nsw i32 %a, 7
; CHECK-NEXT:  ret i32 %mul
  %mul = mul nsw i32 %a, 7
  ret i32 %mul
}

define i32 @_Z13sdiv_not_pow2i(i32 noundef %a) {
; CHECK-LABEL: @_Z13sdiv_not_pow2i
; CHECK-NEXT:  %div = sdiv i32 %a, 10
; CHECK-NEXT:  ret i32 %div
  %div = sdiv i32 %a, 10
  ret i32 %div
}

define i32 @_Z17mul_negative_pow2i(i32 noundef %a) {
; CHECK-LABEL: @_Z17mul_negative_pow2i
; CHECK-NEXT:  %mul = mul nsw i32 %a, -8
; CHECK-NEXT:  ret i32 %mul
  %mul = mul nsw i32 %a, -8
  ret i32 %mul
}

define i64 @_Z7mul_i64x(i64 noundef %a) {
; CHECK-LABEL: @_Z7mul_i64x
; CHECK-NEXT:  %1 = shl i64 %a, 3
; CHECK-NEXT:  ret i64 %1
  %mul = mul i64 %a, 8
  ret i64 %mul
}

define i64 @_Z8udiv_i64y(i64 noundef %a) {
; CHECK-LABEL: @_Z8udiv_i64y
; CHECK-NEXT:  %1 = lshr i64 %a, 2
; CHECK-NEXT:  ret i64 %1
  %div = udiv i64 %a, 4
  ret i64 %div
}

define i64 @_Z8sdiv_i64x(i64 noundef %a) {
; CHECK-LABEL: @_Z8sdiv_i64x
; CHECK-NEXT:  %1 = icmp slt i64 %a, 0
; CHECK-NEXT:  %2 = add i64 %a, 31
; CHECK-NEXT:  %3 = select i1 %1, i64 %2, i64 %a
; CHECK-NEXT:  %4 = ashr i64 %3, 5
; CHECK-NEXT:  ret i64 %4
  %div = sdiv i64 %a, 32
  ret i64 %div
}