; ModuleID = './test/sh/flow/else.sk'
source_filename = "./test/sh/flow/else.sk"

define i64 @.else() {
entry:
  br i1 true, label %if_true, label %if_false

if_true:                                          ; preds = %entry
  br label %end

if_false:                                         ; preds = %entry
  br label %end

end:                                              ; preds = %if_false, %if_true
  ret i64 0
}
