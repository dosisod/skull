@_float_pow = dso_local global double (double, double)* @pow, align 8

define dso_local zeroext i1 @_strcmp(i8* %0, i8* %1) {
  %3 = alloca i1, align 1
  %4 = alloca i8*, align 8
  %5 = alloca i8*, align 8
  store i8* %0, i8** %4, align 8
  store i8* %1, i8** %5, align 8
  br label %6

6:
  %7 = load i8*, i8** %4, align 8
  %8 = load i8, i8* %7, align 1
  %9 = sext i8 %8 to i32
  %10 = icmp ne i32 %9, 0
  br i1 %10, label %11, label %16

11:
  %12 = load i8*, i8** %5, align 8
  %13 = load i8, i8* %12, align 1
  %14 = sext i8 %13 to i32
  %15 = icmp ne i32 %14, 0
  br label %16

16:
  %17 = phi i1 [ false, %6 ], [ %15, %11 ]
  br i1 %17, label %18, label %32

18:
  %19 = load i8*, i8** %4, align 8
  %20 = load i8, i8* %19, align 1
  %21 = sext i8 %20 to i32
  %22 = load i8*, i8** %5, align 8
  %23 = load i8, i8* %22, align 1
  %24 = sext i8 %23 to i32
  %25 = icmp ne i32 %21, %24
  br i1 %25, label %26, label %27

26:
  store i1 false, i1* %3, align 1
  br label %43

27:
  %28 = load i8*, i8** %4, align 8
  %29 = getelementptr inbounds i8, i8* %28, i32 1
  store i8* %29, i8** %4, align 8
  %30 = load i8*, i8** %5, align 8
  %31 = getelementptr inbounds i8, i8* %30, i32 1
  store i8* %31, i8** %5, align 8
  br label %6, !llvm.loop !1

32:
  %33 = load i8*, i8** %4, align 8
  %34 = load i8, i8* %33, align 1
  %35 = icmp ne i8 %34, 0
  br i1 %35, label %41, label %36

36:
  %37 = load i8*, i8** %5, align 8
  %38 = load i8, i8* %37, align 1
  %39 = icmp ne i8 %38, 0
  %40 = xor i1 %39, true
  br label %41

41:
  %42 = phi i1 [ false, %32 ], [ %40, %36 ]
  store i1 %42, i1* %3, align 1
  br label %43

43:
  %44 = load i1, i1* %3, align 1
  ret i1 %44
}

declare double @pow(double, double) #1

define dso_local i64 @_int_pow(i64 %0, i64 %1) {
  %3 = alloca i64, align 8
  %4 = alloca i64, align 8
  %5 = alloca i64, align 8
  %6 = alloca i64, align 8
  store i64 %0, i64* %3, align 8
  store i64 %1, i64* %4, align 8
  %7 = load i64, i64* %3, align 8
  store i64 %7, i64* %5, align 8
  store i64 1, i64* %6, align 8
  br label %8

8:
  %9 = load i64, i64* %6, align 8
  %10 = load i64, i64* %4, align 8
  %11 = icmp slt i64 %9, %10
  br i1 %11, label %12, label %19

12:
  %13 = load i64, i64* %3, align 8
  %14 = load i64, i64* %5, align 8
  %15 = mul nsw i64 %14, %13
  store i64 %15, i64* %5, align 8
  br label %16

16:
  %17 = load i64, i64* %6, align 8
  %18 = add nsw i64 %17, 1
  store i64 %18, i64* %6, align 8
  br label %8, !llvm.loop !3

19:
  %20 = load i64, i64* %5, align 8
  ret i64 %20
}

!1 = distinct !{!1, !2}
!2 = !{!"llvm.loop.mustprogress"}
!3 = distinct !{!3, !2}
