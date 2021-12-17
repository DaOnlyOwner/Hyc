; ModuleID = 'global'
source_filename = "global"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

define i64 @main() {
entry:
  %0 = call i64 @fac_int(i64 5)
  ret i64 %0
}

define i64 @fac_int(i64 %n) {
entry:
  %n1 = alloca i64, align 8
  store i64 %n, i64* %n1, align 8
  %n2 = load i64, i64* %n1, align 8
  %0 = icmp eq i64 %n2, 0
  br i1 %0, label %then, label %cont

then:                                             ; preds = %entry
  ret i64 1

cont:                                             ; preds = %entry
  %n3 = load i64, i64* %n1, align 8
  %n4 = load i64, i64* %n1, align 8
  %1 = sub i64 %n4, 1
  %2 = call i64 @fac_int(i64 %1)
  %3 = mul i64 %n3, %2
  ret i64 %3
}
