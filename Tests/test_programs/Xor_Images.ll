; ModuleID = '/home/alessio/build-octantis/tests/Xor_Images.c'
source_filename = "/home/alessio/build-octantis/tests/Xor_Images.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @Xor_Images() #0 {
  %1 = alloca [256 x [256 x i32]], align 16
  %2 = alloca [256 x [256 x i32]], align 16
  %3 = alloca [256 x [256 x i32]], align 16
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  store i32 0, i32* %4, align 4
  br label %6

6:                                                ; preds = %39, %0
  %7 = load i32, i32* %4, align 4
  %8 = icmp slt i32 %7, 256
  br i1 %8, label %9, label %42

9:                                                ; preds = %6
  store i32 0, i32* %5, align 4
  br label %10

10:                                               ; preds = %35, %9
  %11 = load i32, i32* %5, align 4
  %12 = icmp slt i32 %11, 256
  br i1 %12, label %13, label %38

13:                                               ; preds = %10
  %14 = load i32, i32* %4, align 4
  %15 = sext i32 %14 to i64
  %16 = getelementptr inbounds [256 x [256 x i32]], [256 x [256 x i32]]* %2, i64 0, i64 %15
  %17 = load i32, i32* %5, align 4
  %18 = sext i32 %17 to i64
  %19 = getelementptr inbounds [256 x i32], [256 x i32]* %16, i64 0, i64 %18
  %20 = load i32, i32* %19, align 4
  %21 = load i32, i32* %5, align 4
  %22 = sext i32 %21 to i64
  %23 = getelementptr inbounds [256 x [256 x i32]], [256 x [256 x i32]]* %3, i64 0, i64 %22
  %24 = load i32, i32* %4, align 4
  %25 = sext i32 %24 to i64
  %26 = getelementptr inbounds [256 x i32], [256 x i32]* %23, i64 0, i64 %25
  %27 = load i32, i32* %26, align 4
  %28 = xor i32 %20, %27
  %29 = load i32, i32* %4, align 4
  %30 = sext i32 %29 to i64
  %31 = getelementptr inbounds [256 x [256 x i32]], [256 x [256 x i32]]* %1, i64 0, i64 %30
  %32 = load i32, i32* %5, align 4
  %33 = sext i32 %32 to i64
  %34 = getelementptr inbounds [256 x i32], [256 x i32]* %31, i64 0, i64 %33
  store i32 %28, i32* %34, align 4
  br label %35

35:                                               ; preds = %13
  %36 = load i32, i32* %5, align 4
  %37 = add nsw i32 %36, 1
  store i32 %37, i32* %5, align 4
  br label %10, !llvm.loop !6

38:                                               ; preds = %10
  br label %39

39:                                               ; preds = %38
  %40 = load i32, i32* %4, align 4
  %41 = add nsw i32 %40, 1
  store i32 %41, i32* %4, align 4
  br label %6, !llvm.loop !8

42:                                               ; preds = %6
  ret void
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Ubuntu clang version 15.0.7"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
!8 = distinct !{!8, !7}
