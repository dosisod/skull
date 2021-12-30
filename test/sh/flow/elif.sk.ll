; ModuleID = './test/sh/flow/elif.sk'
source_filename = "./test/sh/flow/elif.sk"

define i64 @.elif() {
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
  %0 = call i1 @f()
  br i1 %0, label %if_true4, label %if_false5

if_true4:                                         ; preds = %end
  br label %end3

if_false5:                                        ; preds = %end
  %1 = call i1 @f()
  br i1 %1, label %if_true6, label %end3

if_true6:                                         ; preds = %if_false5
  br label %end3

end3:                                             ; preds = %if_false5, %if_true6, %if_true4
  ret i64 0
}

define private i1 @f() {
entry:
  ret i1 false
}
