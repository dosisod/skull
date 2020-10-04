; ModuleID = './test/sh/reassign_str.sk'
source_filename = "./test/sh/reassign_str.sk"

define i64 @main() {
entry:
  %x = alloca [4 x i8]
  store [4 x i8] c"abc\00", [4 x i8]* %x
  store [4 x i8] c"def\00", [4 x i8]* %x
}
