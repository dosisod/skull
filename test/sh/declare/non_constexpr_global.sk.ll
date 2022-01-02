; ModuleID = './test/sh/declare/non_constexpr_global.sk'
source_filename = "./test/sh/declare/non_constexpr_global.sk"

@x = private global i64 0

define i64 @.non_constexpr_global() {
entry:
  %0 = call i64 @f()
  store i64 %0, i64* @x, align 4
  ret i64 0
}

define private i64 @f() {
entry:
  ret i64 1
}
