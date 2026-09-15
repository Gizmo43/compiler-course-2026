; RUN: opt -load-pass-plugin %llvmshlibdir/romanova_v_powi_to_mult_seq_transform_LLVM_IR%pluginext\
; RUN: -passes=powi-to-mult-seq -S < %s | FileCheck %s

; CHECK-LABEL: define dso_local noundef double @_Z5test0d
; CHECK-NOT: call double @llvm.powi
; CHECK: ret double 1.000000e+00
define dso_local noundef double @_Z5test0d(double noundef %x) #0 {
entry:
  %x.addr = alloca double, align 8
  store double %x, ptr %x.addr, align 8
  %0 = load double, ptr %x.addr, align 8
  %1 = call double @llvm.powi.f64.i32(double %0, i32 0)
  ret double %1
}

declare double @llvm.powi.f64.i32(double, i32) #1
declare float @llvm.powi.f32.i32(float, i32) #1
declare x86_fp80 @llvm.powi.fp80.i32(x86_fp80, i32) #1
declare fp128 @llvm.powi.f128.i32(fp128, i32) #1
declare ppc_fp128 @llvm.powi.ppcf128.i32(ppc_fp128, i32) #1

; CHECK-LABEL: define dso_local noundef double @_Z5test1d
; CHECK-NOT: call double @llvm.powi
; CHECK: ret double %0
define dso_local noundef double @_Z5test1d(double noundef %x) #0 {
entry:
  %x.addr = alloca double, align 8
  store double %x, ptr %x.addr, align 8
  %0 = load double, ptr %x.addr, align 8
  %1 = call double @llvm.powi.f64.i32(double %0, i32 1)
  ret double %1
}

; CHECK-LABEL: define dso_local noundef double @_Z5test2d
; CHECK-NOT: call double @llvm.powi
; CHECK: %1 = fmul double %0, %0
; CHECK: ret double %1
define dso_local noundef double @_Z5test2d(double noundef %x) #0 {
entry:
  %x.addr = alloca double, align 8
  store double %x, ptr %x.addr, align 8
  %0 = load double, ptr %x.addr, align 8
  %1 = call double @llvm.powi.f64.i32(double %0, i32 2)
  ret double %1
}

; CHECK-LABEL: define dso_local noundef double @_Z5test3d
; CHECK-NOT: call double @llvm.powi
; CHECK: %1 = fmul double %0, %0
; CHECK: %2 = fmul double %1, %0
; CHECK: ret double %2
define dso_local noundef double @_Z5test3d(double noundef %x) #0 {
entry:
  %x.addr = alloca double, align 8
  store double %x, ptr %x.addr, align 8
  %0 = load double, ptr %x.addr, align 8
  %1 = call double @llvm.powi.f64.i32(double %0, i32 3)
  ret double %1
}

; CHECK-LABEL: define dso_local noundef double @_Z5test4d
; CHECK-NOT: call double @llvm.powi
; CHECK: %1 = fmul double %0, %0
; CHECK: %2 = fmul double %1, %1
; CHECK: ret double %2
define dso_local noundef double @_Z5test4d(double noundef %x) #0 {
entry:
  %x.addr = alloca double, align 8
  store double %x, ptr %x.addr, align 8
  %0 = load double, ptr %x.addr, align 8
  %1 = call double @llvm.powi.f64.i32(double %0, i32 4)
  ret double %1
}

; CHECK-LABEL: define dso_local noundef double @_Z7testextd
; CHECK: %1 = call double @llvm.powi.f64.i32(double %0, i32 5)
; CHECK: ret double %1
define dso_local noundef double @_Z7testextd(double noundef %x) #0 {
entry:
  %x.addr = alloca double, align 8
  store double %x, ptr %x.addr, align 8
  %0 = load double, ptr %x.addr, align 8
  %1 = call double @llvm.powi.f64.i32(double %0, i32 5)
  ret double %1
}

; CHECK-LABEL: define dso_local noundef double @_Z8testdnegd
; CHECK: %1 = call double @llvm.powi.f64.i32(double %0, i32 -1)
; CHECK: ret double %1
define dso_local noundef double @_Z8testdnegd(double noundef %x) #0 {
entry:
  %x.addr = alloca double, align 8
  store double %x, ptr %x.addr, align 8
  %0 = load double, ptr %x.addr, align 8
  %1 = call double @llvm.powi.f64.i32(double %0, i32 -1)
  ret double %1
}

; CHECK-LABEL: define dso_local noundef float @_Z5test1f
; CHECK-NOT: call float @llvm.powi
; CHECK: ret float %0
define dso_local noundef float @_Z5test1f(float noundef %x) #0 {
entry:
  %x.addr = alloca float, align 8
  store float %x, ptr %x.addr, align 8
  %0 = load float, ptr %x.addr, align 8
  %1 = call float @llvm.powi.f32.i32(float %0, i32 1)
  ret float %1
}

; CHECK-LABEL: define dso_local noundef x86_fp80 @_Z5test2ld
; CHECK-NOT: call x86_fp80 @llvm.powi
; CHECK: %1 = fmul x86_fp80 %0, %0
; CHECK: ret x86_fp80 %1
define dso_local noundef x86_fp80 @_Z5test2ld(x86_fp80 noundef %x) #0 {
entry:
  %x.addr = alloca x86_fp80, align 8
  store x86_fp80 %x, ptr %x.addr, align 8
  %0 = load x86_fp80, ptr %x.addr, align 8
  %1 = call x86_fp80 @llvm.powi.f80.i32(x86_fp80 %0, i32 2)
  ret x86_fp80 %1
}

; CHECK-LABEL: define dso_local noundef fp128 @_Z5test3f128
; CHECK-NOT: call fp128 @llvm.powi
; CHECK: %1 = fmul fp128 %0, %0
; CHECK: %2 = fmul fp128 %1, %0
; CHECK: ret fp128 %2
define dso_local noundef fp128 @_Z5test3f128(fp128 noundef %x) #0 {
entry:
  %x.addr = alloca fp128, align 8
  store fp128 %x, ptr %x.addr, align 8
  %0 = load fp128, ptr %x.addr, align 8
  %1 = call fp128 @llvm.powi.f128.i32(fp128 %0, i32 3)
  ret fp128 %1
}

; CHECK-LABEL: define dso_local noundef ppc_fp128 @_Z5test4ppcf128
; CHECK-NOT: call ppc_fp128 @llvm.powi
; CHECK: %1 = fmul ppc_fp128 %0, %0
; CHECK: %2 = fmul ppc_fp128 %1, %1
; CHECK: ret ppc_fp128 %2
define dso_local noundef ppc_fp128 @_Z5test4ppcf128(ppc_fp128 noundef %x) #0 {
entry:
  %x.addr = alloca ppc_fp128, align 8
  store ppc_fp128 %x, ptr %x.addr, align 8
  %0 = load ppc_fp128, ptr %x.addr, align 8
  %1 = call ppc_fp128 @llvm.powi.ppcf128.i32(ppc_fp128 %0, i32 4)
  ret ppc_fp128 %1
}

