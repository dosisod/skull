; ModuleID = './test/sh/declare/mut_explicit.sk'
source_filename = "./test/sh/declare/mut_explicit.sk"

@x = private global i64 0

define i64 @.mut_explicit() {
entry:
  store i64 0, i64* @x
  ret i64 0
}
