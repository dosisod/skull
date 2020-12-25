; ModuleID = './test/sh/flow/is_cmp_float.sk'
source_filename = "./test/sh/flow/is_cmp_float.sk"

define i64 @main() {
entry:
  br i1 true, label %if_true, label %end

if_true:                                          ; preds = %entry
  br label %end

end:                                              ; preds = %entry, %if_true
  ret i64 0
}
