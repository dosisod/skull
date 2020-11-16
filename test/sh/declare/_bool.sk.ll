; ModuleID = './test/sh/declare/bool.sk'
source_filename = "./test/sh/declare/bool.sk"

define i64 @main() {
entry:
  %x = alloca i1
  store i1 true, i1* %x
  ret i64 0
}
