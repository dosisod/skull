; ModuleID = './test/sh/flow/set_var_in_if.sk'
source_filename = "./test/sh/flow/set_var_in_if.sk"

@x = private global i64 0

define i64 @main() {
entry:
  br i1 true, label %if_true, label %end

if_true:                                          ; preds = %entry
  store i64 1, i64* @x
  br label %end

end:                                              ; preds = %if_true, %entry
  ret i64 0
}
