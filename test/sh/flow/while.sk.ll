; ModuleID = './test/sh/flow/while.sk'
source_filename = "./test/sh/flow/while.sk"

@i = private global i64 0

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
  %0 = load i64, ptr @i, align 4
  %1 = icmp slt i64 %0, 10
  br i1 %1, label %while_loop2, label %while_end3

while_loop2:                                      ; preds = %while_cond1
  br label %while_cond1

while_end3:                                       ; preds = %while_cond1
  br label %while_cond4

while_cond4:                                      ; preds = %while_loop5, %while_end3
  %2 = call i1 @f()
  br i1 %2, label %while_loop5, label %while_end6

while_loop5:                                      ; preds = %while_cond4
  br label %while_cond4

while_end6:                                       ; preds = %while_cond4
  ret i64 0
}

define private i1 @f() {
entry:
  ret i1 true
}
