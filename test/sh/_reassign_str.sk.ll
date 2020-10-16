; ModuleID = './test/sh/reassign_str.sk'
source_filename = "./test/sh/reassign_str.sk"

define i64 @main() {
entry:
  %x = alloca i8*
  %0 = alloca [4 x i8]
  store [4 x i8] c"abc\00", [4 x i8]* %0
  %1 = bitcast i8** %x to i8*
  store i8* %1, i8** %x
  %2 = alloca [5 x i8]
  store [5 x i8] c"defg\00", [5 x i8]* %2
  %3 = bitcast i8** %x to i8*
  store i8* %3, i8** %x
}
