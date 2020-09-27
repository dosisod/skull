; ModuleID = 'main_module'
source_filename = "main_module"

define i64 @main() {
entry:
  br i1 true, label %if_true, label %end

if_true:                                          ; preds = %entry
  br label %end

end:                                              ; preds = %if_true, %entry
}
