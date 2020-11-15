; ModuleID = './test/sh/declare/escape_sequences.sk'
source_filename = "./test/sh/declare/escape_sequences.sk"

@0 = private unnamed_addr constant [12 x i8] c"\5C \0D \0A \09 A \1B\00", align 1

define i64 @main() {
entry:
  %x = alloca i8*
  store i8* getelementptr inbounds ([12 x i8], [12 x i8]* @0, i32 0, i32 0), i8** %x
}
