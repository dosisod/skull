; ModuleID = 'main_module'
source_filename = "main_module"

define i64 @main() {
entry:
  %x = alloca i1
  store i1 false, i1* %x
  br i1 false, label %if_true, label %end

if_true:                                          ; preds = %entry
  ret i64 1
  br label %end

end:                                              ; preds = %if_true, %entry
  ret i64 0
}
