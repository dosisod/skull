; ModuleID = './test/sh/flow/if/with_var_false.sk'
source_filename = "./test/sh/flow/if/with_var_false.sk"

@x = private global i1 false

define i64 @.with_var_false() {
entry:
  store i1 false, i1* @x
  %0 = load i1, i1* @x
  br i1 %0, label %if_true, label %end

if_true:                                          ; preds = %entry
  ret i64 1

end:                                              ; preds = %entry
  ret i64 0
}
