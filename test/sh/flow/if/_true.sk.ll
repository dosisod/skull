; ModuleID = './test/sh/flow/if/true.sk'
source_filename = "./test/sh/flow/if/true.sk"

define i64 @main() {
entry:
  br i1 true, label %if_true, label %end

if_true:                                          ; preds = %entry
  ret i64 0

end:                                              ; preds = %entry
  ret i64 1
}
