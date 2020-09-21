; ModuleID = 'main_module'
source_filename = "main_module"

define i64 @main() {
entry:
  br i1 true, label %if_true, label %end

if_true:                                          ; preds = %entry
  %x = alloca i64
  store i64 1, i64* %x
  br label %end

end:                                              ; preds = %if_true, %entry
  ret i64 0
}
