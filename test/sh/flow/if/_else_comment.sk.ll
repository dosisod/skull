; ModuleID = './test/sh/flow/if/else_comment.sk'
source_filename = "./test/sh/flow/if/else_comment.sk"

define i64 @.else_comment() {
entry:
  br i1 true, label %if_true, label %if_false

if_true:                                          ; preds = %entry
  ret i64 1

if_false:                                         ; preds = %entry
  ret i64 2

end:                                              ; No predecessors!
  ret i64 0
}
