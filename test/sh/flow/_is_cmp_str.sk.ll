; ModuleID = './test/sh/flow/is_cmp_str.sk'
source_filename = "./test/sh/flow/is_cmp_str.sk"

@0 = private unnamed_addr constant [2 x i8] c"x\00", align 1
@1 = private unnamed_addr constant [2 x i8] c"x\00", align 1

define i64 @.is_cmp_str() {
entry:
  %0 = call i1 @.strcmp(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @0, i32 0, i32 0), i8* getelementptr inbounds ([2 x i8], [2 x i8]* @1, i32 0, i32 0))
  br i1 %0, label %if_true, label %end

if_true:                                          ; preds = %entry
  br label %end

end:                                              ; preds = %entry, %if_true
  ret i64 0
}

declare i1 @.strcmp(i8*, i8*)
