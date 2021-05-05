; ModuleID = './test/sh/misc/noop.sk'
source_filename = "./test/sh/misc/noop.sk"

define i64 @.noop() {
entry:
  br i1 true, label %if_true, label %end

if_true:                                          ; preds = %entry
  br label %end

end:                                              ; preds = %entry, %if_true
  ret i64 0
}
