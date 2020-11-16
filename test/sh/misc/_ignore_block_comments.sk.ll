; ModuleID = './test/sh/misc/ignore_block_comments.sk'
source_filename = "./test/sh/misc/ignore_block_comments.sk"

define i64 @main() {
entry:
  %is_this_kept = alloca i1
  store i1 true, i1* %is_this_kept
  %what_about_this = alloca i1
  store i1 true, i1* %what_about_this
  ret i64 0
}
