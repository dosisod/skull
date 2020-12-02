; ModuleID = './test/sh/declare/mut.sk'
source_filename = "./test/sh/declare/mut.sk"

@x = private global i64 0

define i64 @main() {
entry:
  store i64 0, i64* @x
  ret i64 0
}
