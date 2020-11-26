; ModuleID = './test/sh/flow/nested_if.sk'
source_filename = "./test/sh/flow/nested_if.sk"

define i64 @main() {
entry:
  br i1 true, label %if_true, label %end

if_true:                                          ; preds = %entry
  br i1 true, label %if_true1, label %end2

end:                                              ; preds = %end2, %entry
  ret i64 0

if_true1:                                         ; preds = %if_true
  ret i64 1

end2:                                             ; preds = %if_true
  br label %end
}
