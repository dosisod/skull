; ModuleID = 'main_module'
source_filename = "main_module"

define i64 @main() {
entry:
  %x = alloca double
  store double 3.141500e+00, double* %x
}
