; ModuleID = './test/sh/flow/if_true.sk'
source_filename = "./test/sh/flow/if_true.sk"

define i64 @main() {
entry:
  br i1 true, label %if_true, label %end

if_true:                                          ; preds = %entry
  ret i64 0
  br label %end

end:                                              ; preds = %if_true, %entry
  ret i64 1
}
