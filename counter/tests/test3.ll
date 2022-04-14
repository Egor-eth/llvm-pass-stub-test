; RUN: opt -codegenprepare -S < %s | FileCheck %s

target datalayout = "e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

declare zeroext i1 @return_i1()

define i32 @test_sor_basic(i32 addrspace(1)* %base) gc "statepoint-example" {
; CHECK: getelementptr i32, i32 addrspace(1)* %base, i32 15
; CHECK: getelementptr i32, i32 addrspace(1)* %base-new, i32 15
entry:
       %ptr = getelementptr i32, i32 addrspace(1)* %base, i32 15
       %tok = call token (i64, i32, i1 ()*, i32, i32, ...) @llvm.experimental.gc.statepoint.p0f_i1f(i64 0, i32 0, i1 ()* elementtype(i1 ()) @return_i1, i32 0, i32 0, i32 0, i32 0) ["gc-live"(i32 addrspace(1)* %base, double 1.0, float 2.0)]
;       %base-new = call i32 addrspace(1)* @llvm.experimental.gc.relocate.p1i32(token %tok, i32 0, i32 1)
;       %ptr-new = call i32 addrspace(1)* @llvm.experimental.gc.relocate.p1i32(token %tok, i32 0, i32 1)
;       %ret = load i32, i32 addrspace(1)* %ptr-new
       ret i32 20
}

declare token @llvm.experimental.gc.statepoint.p0f_i1f(i64, i32, i1 ()*, i32, i32, ...)
declare i32 addrspace(1)* @llvm.experimental.gc.relocate.p1i32(token, i32, i32)
declare [3 x i32] addrspace(1)* @llvm.experimental.gc.relocate.p1a3i32(token, i32, i32)