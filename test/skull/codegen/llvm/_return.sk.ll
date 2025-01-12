; ModuleID = './test/skull/codegen/llvm/return.sk'
source_filename = "./test/skull/codegen/llvm/return.sk"

define i64 @.return() !dbg !4 {
entry:
  ret i64 1, !dbg !7
}

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "skull", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false)
!1 = !DIFile(filename: "./test/skull/codegen/llvm/return.sk", directory: ".")
!2 = !{i32 4, !"Dwarf Version", i32 4}
!3 = !{i32 4, !"Debug Info Version", i32 3}
!4 = distinct !DISubprogram(name: ".return", scope: !1, file: !1, line: 1, type: !5, scopeLine: 1, spFlags: DISPFlagDefinition, unit: !0)
!5 = !DISubroutineType(types: !6)
!6 = !{}
!7 = !DILocation(line: 1, column: 1, scope: !4)
