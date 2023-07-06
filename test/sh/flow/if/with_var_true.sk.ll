; ModuleID = './test/sh/flow/if/with_var_true.sk'
source_filename = "./test/sh/flow/if/with_var_true.sk"

@x = private global i1 true

define i64 @.with_var_true() {
entry:
  %0 = load i1, ptr @x, align 1
  br i1 %0, label %if_true, label %end

if_true:                                          ; preds = %entry
  ret i64 1

end:                                              ; preds = %entry
  ret i64 0
}
