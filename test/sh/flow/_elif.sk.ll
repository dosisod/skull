; ModuleID = './test/sh/flow/elif.sk'
source_filename = "./test/sh/flow/elif.sk"

define i64 @main() {
entry:
  br i1 false, label %if_true, label %if_false

if_true:                                          ; preds = %entry
  br label %end

if_false:                                         ; preds = %entry
  br i1 true, label %if_true1, label %if_false2

if_true1:                                         ; preds = %if_false
  br label %end

if_false2:                                        ; preds = %if_false
  br label %end

end:                                              ; preds = %if_false2, %if_true1, %if_true
  ret i64 0
}
