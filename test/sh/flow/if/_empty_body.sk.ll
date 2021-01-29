; ModuleID = './test/sh/flow/if/empty_body.sk'
source_filename = "./test/sh/flow/if/empty_body.sk"

define i64 @.empty_body() {
entry:
  br i1 true, label %if_true, label %end

if_true:                                          ; preds = %entry
  br label %end

end:                                              ; preds = %entry, %if_true
  ret i64 0
}
