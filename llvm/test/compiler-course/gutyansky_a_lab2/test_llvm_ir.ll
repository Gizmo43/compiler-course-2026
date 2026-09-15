; RUN: opt -load-pass-plugin %llvmshlibdir/gutyansky_a_lab2_LLVM_IR%pluginext\
; RUN: -passes=gutyansky-a-mul-div-optim -S %s | FileCheck %s

; CHECK-LABEL: @mul_by_4
; CHECK-NEXT: %shl = shl i32 %x, 2
; CHECK-NEXT: ret i32 %shl
define i32 @mul_by_4(i32 %x) {
  %mul = mul nsw i32 %x, 4
  ret i32 %mul
}

; CHECK-LABEL: @mul_by_2_u
; CHECK-NEXT: %shl = shl i32 %x, 1
; CHECK-NEXT: ret i32 %shl
define i32 @mul_by_2_u(i32 %x) {
  %mul = mul i32 %x, 2
  ret i32 %mul
}

; CHECK-LABEL: @div_by_16
; CHECK-NEXT: %1 = icmp slt i32 %x, 0
; CHECK-NEXT: %2 = select i1 %1, i32 15, i32 0
; CHECK-NEXT: %3 = add i32 %x, %2
; CHECK-NEXT: %4 = ashr i32 %3, 4
; CHECK-NEXT: ret i32 %4
define i32 @div_by_16(i32 %x) {
  %div = sdiv i32 %x, 16
  ret i32 %div
}

; CHECK-LABEL: @div_by_4_u
; CHECK-NEXT: %lshr = lshr i32 %x, 2
; CHECK-NEXT: ret i32 %lshr
define i32 @div_by_4_u(i32 %x) {
  %div = udiv i32 %x, 4
  ret i32 %div
}

; CHECK-LABEL: @mul_by_1
; CHECK-NEXT: %shl = shl i32 %x, 0
; CHECK-NEXT: ret i32 %shl
define i32 @mul_by_1(i32 %x) {
  %mul = mul nsw i32 %x, 1
  ret i32 %mul
}

; CHECK-LABEL: @div_by_1
; CHECK-NEXT: %1 = icmp slt i32 %x, 0
; CHECK-NEXT: %2 = select i1 %1, i32 0, i32 0
; CHECK-NEXT: %3 = add i32 %x, %2
; CHECK-NEXT: %4 = ashr i32 %3, 0
; CHECK-NEXT: ret i32 %4
define i32 @div_by_1(i32 %x) {
  %div = sdiv i32 %x, 1
  ret i32 %div
}

; CHECK-LABEL: @mul_by_8_long
; CHECK-NEXT: %shl = shl i64 %x, 3
; CHECK-NEXT: ret i64 %shl
define i64 @mul_by_8_long(i64 %x) {
  %mul = mul nsw i64 %x, 8
  ret i64 %mul
}

; CHECK-LABEL: @mul_by_16_longlong
; CHECK-NEXT: %shl = shl i64 %x, 4
; CHECK-NEXT: ret i64 %shl
define i64 @mul_by_16_longlong(i64 %x) {
  %mul = mul nsw i64 %x, 16
  ret i64 %mul
}

; CHECK-LABEL: @test_mul_2_i8
; CHECK-NEXT: %shl = shl i8 %a, 1
; CHECK-NEXT: ret i8 %shl
define i8 @test_mul_2_i8(i8 %a) {
  %r = mul i8 %a, 2
  ret i8 %r
}

; CHECK-LABEL: @test_div_4_i8
; CHECK-NEXT: %1 = icmp slt i8 %a, 0
; CHECK-NEXT: %2 = select i1 %1, i8 3, i8 0
; CHECK-NEXT: %3 = add i8 %a, %2
; CHECK-NEXT: %4 = ashr i8 %3, 2
; CHECK-NEXT: ret i8 %4
define i8 @test_div_4_i8(i8 %a) {
  %r = sdiv i8 %a, 4
  ret i8 %r
}

; CHECK-LABEL: @test_udiv_2_i8
; CHECK-NEXT: %lshr = lshr i8 %a, 1
; CHECK-NEXT: ret i8 %lshr
define i8 @test_udiv_2_i8(i8 %a) {
  %r = udiv i8 %a, 2
  ret i8 %r
}

; CHECK-LABEL: @test_mul_4_i16
; CHECK-NEXT: %shl = shl i16 %a, 2
; CHECK-NEXT: ret i16 %shl
define i16 @test_mul_4_i16(i16 %a) {
  %r = mul i16 %a, 4
  ret i16 %r
}

; CHECK-LABEL: @test_div_8_i16
; CHECK-NEXT: %1 = icmp slt i16 %a, 0
; CHECK-NEXT: %2 = select i1 %1, i16 7, i16 0
; CHECK-NEXT: %3 = add i16 %a, %2
; CHECK-NEXT: %4 = ashr i16 %3, 3
; CHECK-NEXT: ret i16 %4
define i16 @test_div_8_i16(i16 %a) {
  %r = sdiv i16 %a, 8
  ret i16 %r
}

; CHECK-LABEL: @test_udiv_16_i16
; CHECK-NEXT: %lshr = lshr i16 %a, 4
; CHECK-NEXT: ret i16 %lshr
define i16 @test_udiv_16_i16(i16 %a) {
  %r = udiv i16 %a, 16
  ret i16 %r
}

; CHECK-LABEL: @mul_by_neg2
; CHECK-NEXT: %mul = mul nsw i32 %x, -2
; CHECK-NEXT: ret i32 %mul
define i32 @mul_by_neg2(i32 %x) {
  %mul = mul nsw i32 %x, -2
  ret i32 %mul
}

; CHECK-LABEL: @div_by_neg4
; CHECK-NEXT: %div = sdiv i32 %x, -4
; CHECK-NEXT: ret i32 %div
define i32 @div_by_neg4(i32 %x) {
  %div = sdiv i32 %x, -4
  ret i32 %div
}

; CHECK-LABEL: @mul_by_6
; CHECK-NEXT: %mul = mul nsw i32 %x, 6
; CHECK-NEXT: ret i32 %mul
define i32 @mul_by_6(i32 %x) {
  %mul = mul nsw i32 %x, 6
  ret i32 %mul
}

; CHECK-LABEL: @div_by_6
; CHECK-NEXT: %div = sdiv i32 %x, 6
; CHECK-NEXT: ret i32 %div
define i32 @div_by_6(i32 %x) {
  %div = sdiv i32 %x, 6
  ret i32 %div
}

; CHECK-LABEL: @mul_by_2_30
; CHECK-NEXT: %shl = shl i32 %x, 30
; CHECK-NEXT: ret i32 %shl
define i32 @mul_by_2_30(i32 %x) {
  %mul = mul nsw i32 %x, 1073741824
  ret i32 %mul
}

; CHECK-LABEL: @div_by_2_30_u
; CHECK-NEXT: %lshr = lshr i32 %x, 30
; CHECK-NEXT: ret i32 %lshr
define i32 @div_by_2_30_u(i32 %x) {
  %div = udiv i32 %x, 1073741824
  ret i32 %div
}

