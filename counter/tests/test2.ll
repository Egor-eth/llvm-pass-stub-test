
target datalayout = "e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

declare zeroext i1 @return_i1()


define i32 @test_sor_noop(i32 addrspace(1)* %base) gc "statepoint-example" {
; CHECK: getelementptr i32, i32 addrspace(1)* %base, i32 15
; CHECK: call i32 addrspace(1)* @llvm.experimental.gc.relocate.p1i32(token %tok, i32 0, i32 1)
; CHECK: call i32 addrspace(1)* @llvm.experimental.gc.relocate.p1i32(token %tok, i32 0, i32 2)
entry:
       %ptr = getelementptr i32, i32 addrspace(1)* %base, i32 15
       %ptr2 = getelementptr i32, i32 addrspace(1)* %base, i32 12
       %ptr3 = getelementptr i32, i32 addrspace(1)* %base, i32 15
       %ptr4 = getelementptr i32, i32 addrspace(1)* %base, i32 12
       %ptr5 = getelementptr i32, i32 addrspace(1)* %base, i32 15
       %ptr6 = getelementptr i32, i32 addrspace(1)* %base, i32 12
       %tok = call token (i64, i32, i1 ()*, i32, i32, ...) @llvm.experimental.gc.statepoint.p0f_i1f(i64 0, i32 0, i1 ()* elementtype(i1 ()) @return_i1, i32 0, i32 0, i32 0, i32 0) ["gc-live"(i32 addrspace(1)* %base, i32 addrspace(1)* %ptr, i32 addrspace(1)* %ptr2, i32 addrspace(1)* %ptr3, i32 addrspace(1)* %ptr4, i32 addrspace(1)* %ptr5, i32 addrspace(1)* %ptr6)]
       %ptr-new = call i32 addrspace(1)* @llvm.experimental.gc.relocate.p1i32(token %tok, i32 0, i32 1)
       %ptr2-new = call i32 addrspace(1)* @llvm.experimental.gc.relocate.p1i32(token %tok, i32 0, i32 2)
       %ret = load i32, i32 addrspace(1)* %ptr-new
       ret i32 %ret
}


define i32 @test_sor_noop_same_bb(i1 %external-cond, i32 addrspace(1)* %base) gc "statepoint-example" {
; CHECK-LABEL: @test_sor_noop_same_bb
; Here base relocate doesn't dominate derived relocate. Make sure that we don't
; produce undefined use of the relocated base pointer.
entry:
       %ptr1 = getelementptr i32, i32 addrspace(1)* %base, i32 15
       ; CHECK: getelementptr i32, i32 addrspace(1)* %base, i32 15
       %ptr2 = getelementptr i32, i32 addrspace(1)* %base, i32 5
       ; CHECK: getelementptr i32, i32 addrspace(1)* %base, i32 5
       %tok = call token (i64, i32, i1 ()*, i32, i32, ...) @llvm.experimental.gc.statepoint.p0f_i1f(i64 0, i32 0, i1 ()* elementtype(i1 ()) @return_i1, i32 0, i32 0, i32 0, i32 0) ["gc-live"(i32 addrspace(1)* %base)]
       ; CHECK: call i32 addrspace(1)* @llvm.experimental.gc.relocate.p1i32(token %tok, i32 0, i32 0)
       %ptr2-new = call i32 addrspace(1)* @llvm.experimental.gc.relocate.p1i32(token %tok, i32 0, i32 0)
       %ret2-new = load i32, i32 addrspace(1)* %ptr2-new
       ; CHECK: getelementptr i32, i32 addrspace(1)* %base-new, i32 5
       %ptr1-new = call i32 addrspace(1)* @llvm.experimental.gc.relocate.p1i32(token %tok, i32 0, i32 0)
       %ret1-new = load i32, i32 addrspace(1)* %ptr1-new
       ; CHECK: getelementptr i32, i32 addrspace(1)* %base-new, i32 15
       %base-new = call i32 addrspace(1)* @llvm.experimental.gc.relocate.p1i32(token %tok, i32 0, i32 0)
       %ret-new = add i32 %ret2-new, %ret1-new
       ret i32 %ret-new
}

declare token @llvm.experimental.gc.statepoint.p0f_i1f(i64, i32, i1 ()*, i32, i32, ...)
declare i32 addrspace(1)* @llvm.experimental.gc.relocate.p1i32(token, i32, i32)
declare [3 x i32] addrspace(1)* @llvm.experimental.gc.relocate.p1a3i32(token, i32, i32)