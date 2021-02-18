; ModuleID = './test/sh/oper/int_pow.sk'
source_filename = "./test/sh/oper/int_pow.sk"

@x = private global i64 0

define i64 @.int_pow() {
entry:
  %0 = call i64 @_int_pow(i64 10, i64 3)
  store i64 %0, i64* @x
  ret i64 0
}

declare i64 @_int_pow(i64, i64)
