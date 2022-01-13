; ModuleID = './test/sh/misc/locale.sk'
source_filename = "./test/sh/misc/locale.sk"

@0 = private unnamed_addr constant [5 x i8] c"\F0\9F\98\80\00", align 1
@emoji = private global i8* getelementptr inbounds ([5 x i8], [5 x i8]* @0, i32 0, i32 0)

define i64 @.locale() {
entry:
  ret i64 0
}
