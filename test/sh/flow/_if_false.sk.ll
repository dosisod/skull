; ModuleID = './test/sh/flow/if_false.sk'
source_filename = "./test/sh/flow/if_false.sk"

define i64 @main() {
entry:
  br i1 false, label %if_true, label %end

if_true:                                          ; preds = %entry
  ret i64 0

end:                                              ; preds = %entry
  ret i64 1
}
