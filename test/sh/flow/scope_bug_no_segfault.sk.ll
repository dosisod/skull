; ModuleID = './test/sh/flow/scope_bug_no_segfault.sk'
source_filename = "./test/sh/flow/scope_bug_no_segfault.sk"

define i64 @.scope_bug_no_segfault() {
entry:
  br i1 true, label %if_true, label %end

if_true:                                          ; preds = %entry
  br i1 true, label %if_true2, label %end1

end:                                              ; preds = %entry, %end1
  ret i64 0

if_true2:                                         ; preds = %if_true
  br i1 true, label %if_true4, label %end3

end1:                                             ; preds = %if_true, %end3
  br label %end

if_true4:                                         ; preds = %if_true2
  br label %end3

end3:                                             ; preds = %if_true2, %if_true4
  br label %end1
}

declare void @f()
