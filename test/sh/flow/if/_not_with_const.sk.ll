; ModuleID = './test/sh/flow/if/not_with_const.sk'
source_filename = "./test/sh/flow/if/not_with_const.sk"

define i64 @.not_with_const() {
entry:
  br i1 false, label %if_true, label %end

if_true:                                          ; preds = %entry
  br label %end

end:                                              ; preds = %entry, %if_true
  ret i64 0
}
