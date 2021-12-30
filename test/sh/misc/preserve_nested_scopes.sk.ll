; ModuleID = './test/sh/misc/preserve_nested_scopes.sk'
source_filename = "./test/sh/misc/preserve_nested_scopes.sk"

define i64 @.preserve_nested_scopes() {
entry:
  br i1 true, label %if_true, label %end

if_true:                                          ; preds = %entry
  br i1 true, label %if_true2, label %end1

end:                                              ; preds = %entry, %end1
  ret i64 0

if_true2:                                         ; preds = %if_true
  br label %end1

end1:                                             ; preds = %if_true, %if_true2
  br label %end
}
