; ModuleID = './test/sh/misc/ignore_block_comments.sk'
source_filename = "./test/sh/misc/ignore_block_comments.sk"

@is_this_kept = private global i1 false
@what_about_this = private global i1 false

define i64 @main() {
entry:
  store i1 true, i1* @is_this_kept
  store i1 true, i1* @what_about_this
  ret i64 0
}
