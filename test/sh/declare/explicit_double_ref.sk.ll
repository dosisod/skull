; ModuleID = './test/sh/declare/explicit_double_ref.sk'
source_filename = "./test/sh/declare/explicit_double_ref.sk"

@a = private global i64 1
@b = private global i64* @a
@c = private global i64** @b

define i64 @.explicit_double_ref() {
entry:
  ret i64 0
}
