; ModuleID = './test/sh/function/func_in_codeblock_respects_blocks.sk'
source_filename = "./test/sh/function/func_in_codeblock_respects_blocks.sk"

define i64 @.func_in_codeblock_respects_blocks() {
entry:
  br i1 true, label %if_true, label %end

if_true:                                          ; preds = %entry
  br label %end

end:                                              ; preds = %entry, %if_true
  ret i64 0
}

define private void @f() {
entry:
  ret void
}
