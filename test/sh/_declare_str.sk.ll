; ModuleID = './test/sh/declare_str.sk'
source_filename = "./test/sh/declare_str.sk"

define i64 @main() {
entry:
  %0 = alloca [7 x i8]
  store [7 x i8] c"abcdef\00", [7 x i8]* %0
  %x = alloca i8*
  %1 = bitcast i8** %x to i8*
  store i8* %1, i8** %x
}
