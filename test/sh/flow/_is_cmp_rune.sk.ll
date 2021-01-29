; ModuleID = './test/sh/flow/is_cmp_rune.sk'
source_filename = "./test/sh/flow/is_cmp_rune.sk"

define i64 @.is_cmp_rune() {
entry:
  br i1 true, label %if_true, label %end

if_true:                                          ; preds = %entry
  br label %end

end:                                              ; preds = %entry, %if_true
  ret i64 0
}
