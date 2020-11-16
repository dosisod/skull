; ModuleID = './test/sh/flow/if_empty_body.sk'
source_filename = "./test/sh/flow/if_empty_body.sk"

define i64 @main() {
entry:
  br i1 true, label %if_true, label %end

if_true:                                          ; preds = %entry
  br label %end

end:                                              ; preds = %if_true, %entry
  ret i64 0
}
