; ModuleID = './test/sh/flow/continue.sk'
source_filename = "./test/sh/flow/continue.sk"

define i64 @.continue() {
entry:
  br label %while_cond

while_cond:                                       ; preds = %while_loop, %entry
  br i1 true, label %while_loop, label %while_end

while_loop:                                       ; preds = %while_cond
  br label %while_cond

while_end:                                        ; preds = %while_cond
  ret i64 0
}
