; ModuleID = './test/sh/flow/if/not_with_var.sk'
source_filename = "./test/sh/flow/if/not_with_var.sk"

@x = private global i1 false

define i64 @main() {
entry:
  store i1 true, i1* @x
  %0 = load i1, i1* @x
  %1 = xor i1 %0, true
  br i1 %1, label %if_true, label %end

if_true:                                          ; preds = %entry
  br label %end

end:                                              ; preds = %entry, %if_true
  ret i64 0
}
