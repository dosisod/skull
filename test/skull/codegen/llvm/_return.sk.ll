; ModuleID = './test/skull/codegen/llvm/return.sk'
source_filename = "./test/skull/codegen/llvm/return.sk"

define i64 @.return() !dbg !5 {
entry:
  ret i64 1, !dbg !7
}

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "skull", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2, splitDebugInlining: false)
!1 = !DIFile(filename: "./test/skull/codegen/llvm/return.sk", directory: ".")
!2 = !{}
!3 = !{i32 4, !"Dwarf Version", i32 4}
!4 = !{i32 4, !"Debug Info Version", i32 3}
!5 = distinct !DISubprogram(name: ".return", scope: !1, file: !1, line: 1, type: !6, scopeLine: 1, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!6 = !DISubroutineType(types: !2)
!7 = !DILocation(line: 1, column: 1, scope: !5)
