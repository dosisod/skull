; ModuleID = './test/sh/declare/explicit_ref.sk'
source_filename = "./test/sh/declare/explicit_ref.sk"

@a = private global i64 1
@b = private global ptr @a

define i64 @.explicit_ref() {
entry:
  ret i64 0
}
