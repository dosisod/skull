; ModuleID = './test/sh/flow/if/false.sk'
source_filename = "./test/sh/flow/if/false.sk"

define i64 @.false() {
entry:
  br i1 false, label %if_true, label %end

if_true:                                          ; preds = %entry
  ret i64 0

end:                                              ; preds = %entry
  ret i64 1
}
