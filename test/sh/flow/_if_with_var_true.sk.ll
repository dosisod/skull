; ModuleID = './test/sh/flow/if_with_var_true.sk'
source_filename = "./test/sh/flow/if_with_var_true.sk"

define i64 @main() {
entry:
  %x = alloca i1
  store i1 true, i1* %x
  %0 = load i1, i1* %x
  br i1 %0, label %if_true, label %end

if_true:                                          ; preds = %entry
  ret i64 1

end:                                              ; preds = %entry
  ret i64 0
}
