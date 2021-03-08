; ModuleID = './test/sh/declare/mut_with_explicit_type.sk'
source_filename = "./test/sh/declare/mut_with_explicit_type.sk"

@x = private global i64 0

define i64 @.mut_with_explicit_type() {
entry:
  store i64 0, i64* @x
  ret i64 0
}
