; ModuleID = './test/sh/flow/set_var_in_if.sk'
source_filename = "./test/sh/flow/set_var_in_if.sk"

@x = private global i64 0

define i64 @.set_var_in_if() {
entry:
  br i1 true, label %if_true, label %end

if_true:                                          ; preds = %entry
  store i64 1, i64* @x
  br label %end

end:                                              ; preds = %entry, %if_true
  ret i64 0
}
