; ModuleID = './test/sh/escape_sequences.sk'
source_filename = "./test/sh/escape_sequences.sk"

define i64 @main() {
entry:
  %x = alloca i8*
  %0 = alloca [10 x i8]
  store [10 x i8] c"\5C \0D \0A \09 A\00", [10 x i8]* %0
  %1 = bitcast [10 x i8]* %0 to i8*
  store i8* %1, i8** %x
}
