; ModuleID = './test/sh/if_with_var_true.sk'
source_filename = "./test/sh/if_with_var_true.sk"

define i64 @main() {
entry:
  %x = alloca i1
  store i1 true, i1* %x
  %0 = load i64, i1* %x
  br i64 %0, label %if_true, label %end

if_true:                                          ; preds = %entry
  ret i64 1
  br label %end

end:                                              ; preds = %if_true, %entry
  ret i64 0
}
