; ModuleID = './test/sh/function/nested_if.sk'
source_filename = "./test/sh/function/nested_if.sk"

define i64 @.nested_if() {
entry:
  ret i64 0
}

define private void @f() {
entry:
  br i1 true, label %if_true, label %end

if_true:                                          ; preds = %entry
  br label %end

end:                                              ; preds = %entry, %if_true
  ret void
}
