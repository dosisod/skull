; ModuleID = 'main_module'
source_filename = "main_module"

define i64 @main() {
entry:
  %x = alloca i64
  store i64 0, i64* %x
  %z = alloca i64
  store i64 0, i64* %z
  %0 = load i64, i64* %z
  ret i64 %0
}
