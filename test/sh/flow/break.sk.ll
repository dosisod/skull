; ModuleID = './test/sh/flow/break.sk'
source_filename = "./test/sh/flow/break.sk"

define i64 @.break() {
entry:
  br label %while_cond

while_cond:                                       ; preds = %while_loop, %entry
  br i1 true, label %while_loop, label %while_end

while_loop:                                       ; preds = %while_cond
  br label %while_end
  br label %while_cond

while_end:                                        ; preds = %while_loop, %while_cond
  ret i64 0
}
