; ModuleID = './test/sh/function/nested_if.sk'
source_filename = "./test/sh/function/nested_if.sk"

define i64 @main() {
entry:
  ret i64 0
}

define void @f() {
entry:
  br i1 true, label %if_true, label %end

if_true:                                          ; preds = %entry
  br label %end

end:                                              ; preds = %if_true, %entry
  ret void
}
