; ModuleID = './test/sh/declare_str.sk'
source_filename = "./test/sh/declare_str.sk"

define i64 @main() {
entry:
  %x = alloca i8*
  %0 = alloca [7 x i8]
  store [7 x i8] c"abcdef\00", [7 x i8]* %0
  %1 = bitcast [7 x i8]* %0 to i8*
  store i8* %1, i8** %x
}
