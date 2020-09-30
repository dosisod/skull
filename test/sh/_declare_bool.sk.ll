; ModuleID = './test/sh/declare_bool.sk'
source_filename = "./test/sh/declare_bool.sk"

define i64 @main() {
entry:
  %x = alloca i1
  store i1 true, i1* %x
}
