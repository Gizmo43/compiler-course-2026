; RUN: opt -load-pass-plugin %llvmshlibdir/papulina_yuliya_fi3_LLVM_IR%pluginext\
; RUN: -passes=mul-del-optimization -S %s | FileCheck %s

define i32 @_Z4mul1i(i32 noundef %a) {
; CHECK-LABEL: @_Z4mul1i
; CHECK-NEXT:  %Shl = shl i32 %a, 2
; CHECK-NEXT:  ret i32 %Shl
  %mul = mul nsw i32 %a, 4
  ret i32 %mul
}

define i32 @_Z4mul3i(i32 noundef %a) {
; CHECK-LABEL: @_Z4mul3i
; CHECK-NEXT:  %mul = mul nsw i32 %a, 7
; CHECK-NEXT:  ret i32 %mul
  %mul = mul nsw i32 %a, 7
  ret i32 %mul
}

define i32 @_Z4mul4i(i32 noundef %a) {
; CHECK-LABEL: @_Z4mul4i
; CHECK-NEXT:  %mul = mul nsw i32 %a, -8
; CHECK-NEXT:  ret i32 %mul
  %mul = mul nsw i32 %a, -8
  ret i32 %mul
}

define i32 @_Z4div1i(i32 noundef %a) {
; CHECK-LABEL: @_Z4div1i
; CHECK-NEXT:  %1 = icmp slt i32 %a, 0
; CHECK-NEXT:  %2 = select i1 %1, i32 7, i32 0
; CHECK-NEXT:  %3 = add i32 %a, %2
; CHECK-NEXT:  %AShr = ashr i32 %3, 3
; CHECK-NEXT:  ret i32 %AShr
  %div = sdiv i32 %a, 8
  ret i32 %div
}

define i32 @_Z4div2i(i32 noundef %a) {
; CHECK-LABEL: @_Z4div2i
; CHECK-NEXT:  %div = sdiv i32 %a, -8
; CHECK-NEXT:  ret i32 %div
  %div = sdiv i32 %a, -8
  ret i32 %div
}

define i32 @_Z4div3i(i32 noundef %a) {
; CHECK-LABEL: @_Z4div3i
; CHECK-NEXT:  %1 = icmp slt i32 %a, 0
; CHECK-NEXT:  %2 = select i1 %1, i32 3, i32 0
; CHECK-NEXT:  %3 = add i32 %a, %2
; CHECK-NEXT:  %AShr = ashr i32 %3, 2
; CHECK-NEXT:  ret i32 %AShr
  %div = sdiv i32 %a, 4
  ret i32 %div
}

define i32 @_Z4div4i(i32 noundef %a) {
; CHECK-LABEL: @_Z4div4i
; CHECK-NEXT:  %div = sdiv i32 %a, 3
; CHECK-NEXT:  ret i32 %div
  %div = sdiv i32 %a, 3
  ret i32 %div
}

define i32 @_Z4div5j(i32 noundef %a) {
; CHECK-LABEL:  @_Z4div5j
; CHECK-NEXT: %Shr = lshr i32 %a, 3
; CHECK-NEXT: ret i32 %Shr
  %div1 = udiv i32 %a, 8
  ret i32 %div1
}

define i8 @_Z5mul1i8(i8 noundef %a) {
; CHECK-LABEL: @_Z5mul1i8
; CHECK-NEXT:  %Shl = shl i8 %a, 2
; CHECK-NEXT:  ret i8 %Shl
  %mul = mul i8 %a, 4
  ret i8 %mul
}

define i8 @_Z5mul3i8(i8 noundef %a) {
; CHECK-LABEL: @_Z5mul3i8
; CHECK-NEXT:  %mul = mul i8 %a, 7
; CHECK-NEXT:  ret i8 %mul
  %mul = mul i8 %a, 7
  ret i8 %mul
}

define i8 @_Z5mul4i8(i8 noundef %a) {
; CHECK-LABEL: @_Z5mul4i8
; CHECK-NEXT:  %mul = mul nsw i8 %a, -8
; CHECK-NEXT:  ret i8 %mul
  %mul = mul nsw i8 %a, -8
  ret i8 %mul
}

define i8 @_Z5div1i8(i8 noundef %a) {
; CHECK-LABEL: @_Z5div1i8
; CHECK-NEXT:  %1 = icmp slt i8 %a, 0
; CHECK-NEXT:  %2 = select i1 %1, i8 3, i8 0
; CHECK-NEXT:  %3 = add i8 %a, %2
; CHECK-NEXT:  %AShr = ashr i8 %3, 2
; CHECK-NEXT:  ret i8 %AShr
  %div = sdiv i8 %a, 4
  ret i8 %div
}

define i8 @_Z5div2j8(i8 noundef %a) {
; CHECK-LABEL: @_Z5div2j8
; CHECK-NEXT: %Shr = lshr i8 %a, 3
; CHECK-NEXT: ret i8 %Shr
  %div = udiv i8 %a, 8
  ret i8 %div
}

define i16 @_Z6mul1i16(i16 noundef %a) {
; CHECK-LABEL: @_Z6mul1i16
; CHECK-NEXT:  %Shl = shl i16 %a, 2
; CHECK-NEXT:  ret i16 %Shl
  %mul = mul i16 %a, 4
  ret i16 %mul
}

define i64 @_Z6mul1i64(i64 noundef %a) {
; CHECK-LABEL: @_Z6mul1i64
; CHECK-NEXT:  %Shl = shl i64 %a, 2
; CHECK-NEXT:  ret i64 %Shl
  %mul = mul i64 %a, 4
  ret i64 %mul
}
