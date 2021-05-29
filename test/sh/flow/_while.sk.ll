; ModuleID = './test/sh/flow/while.sk'
source_filename = "./test/sh/flow/while.sk"

define i64 @.while() {
entry:
  br label %while_cond

while_cond:                                       ; preds = %while_loop, %entry
  br i1 true, label %while_loop, label %while_end

while_loop:                                       ; preds = %while_cond
  br label %while_cond

while_end:                                        ; preds = %while_cond
  br label %while_cond1

while_cond1:                                      ; preds = %while_loop2, %while_end
  br i1 true, label %while_loop2, label %while_end3

while_loop2:                                      ; preds = %while_cond1
  br label %while_cond1

while_end3:                                       ; preds = %while_cond1
  ret i64 0
}
