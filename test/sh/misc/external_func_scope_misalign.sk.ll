; ModuleID = './test/sh/misc/external_func_scope_misalign.sk'
source_filename = "./test/sh/misc/external_func_scope_misalign.sk"

define i64 @.external_func_scope_misalign() {
entry:
  br i1 true, label %if_true, label %end

if_true:                                          ; preds = %entry
  br label %end

end:                                              ; preds = %entry, %if_true
  br i1 true, label %if_true2, label %end1

if_true2:                                         ; preds = %end
  br label %end1

end1:                                             ; preds = %end, %if_true2
  ret i64 0
}

declare void @f()
