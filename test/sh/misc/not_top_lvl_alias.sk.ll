; ModuleID = './test/sh/misc/not_top_lvl_alias.sk'
source_filename = "./test/sh/misc/not_top_lvl_alias.sk"

define i64 @.not_top_lvl_alias() {
entry:
  br i1 false, label %if_true, label %end

if_true:                                          ; preds = %entry
  br label %end

end:                                              ; preds = %entry, %if_true
  ret i64 0
}
