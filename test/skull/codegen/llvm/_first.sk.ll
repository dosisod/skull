; ModuleID = './test/skull/codegen/llvm/first.sk'
source_filename = "./test/skull/codegen/llvm/first.sk"

@0 = private unnamed_addr constant [12 x i8] c"some string\00", align 1

define i64 @.first() !dbg !5 {
entry:
  %0 = call i64 @f(), !dbg !7
  ret i64 0, !dbg !8
}

define private i64 @f() !dbg !9 {
entry:
  ret i64 1, !dbg !10
}

define private void @f2(i64 %0) !dbg !11 {
entry:
  ret void
}

define private void @f3(i64 %0, i64 %1) !dbg !15 {
entry:
  ret void
}

define private void @f4() !dbg !18 {
entry:
  %local_var = alloca i64
  store i64 1, i64* %local_var, !dbg !33
  call void @llvm.dbg.declare(metadata i64* %local_var, metadata !20, metadata !DIExpression()), !dbg !33
  br i1 true, label %if_true, label %end

if_true:                                          ; preds = %entry
  %if_test = alloca i64
  store i64 1, i64* %if_test, !dbg !34
  br label %end

end:                                              ; preds = %entry, %if_true
  call void @llvm.dbg.declare(metadata i64* %if_test, metadata !21, metadata !DIExpression()), !dbg !34
  %int = alloca i64
  store i64 1234, i64* %int, !dbg !35
  call void @llvm.dbg.declare(metadata i64* %int, metadata !23, metadata !DIExpression()), !dbg !35
  %float = alloca double
  store double 3.141500e+00, double* %float, !dbg !36
  call void @llvm.dbg.declare(metadata double* %float, metadata !24, metadata !DIExpression()), !dbg !36
  %rune = alloca i32
  store i32 120, i32* %rune, !dbg !37
  call void @llvm.dbg.declare(metadata i32* %rune, metadata !26, metadata !DIExpression()), !dbg !37
  %bool = alloca i1
  store i1 false, i1* %bool, !dbg !38
  call void @llvm.dbg.declare(metadata i1* %bool, metadata !28, metadata !DIExpression()), !dbg !38
  %str = alloca i8*
  store i8* getelementptr inbounds ([12 x i8], [12 x i8]* @0, i32 0, i32 0), i8** %str, !dbg !39
  call void @llvm.dbg.declare(metadata i8** %str, metadata !30, metadata !DIExpression()), !dbg !39
  ret void
}

; Function Attrs: nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #0

attributes #0 = { nounwind readnone speculatable willreturn }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "skull", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2, splitDebugInlining: false)
!1 = !DIFile(filename: "./test/skull/codegen/llvm/first.sk", directory: ".")
!2 = !{}
!3 = !{i32 4, !"Dwarf Version", i32 4}
!4 = !{i32 4, !"Debug Info Version", i32 3}
!5 = distinct !DISubprogram(name: ".first", scope: !1, file: !1, line: 1, type: !6, scopeLine: 1, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!6 = !DISubroutineType(types: !2)
!7 = !DILocation(line: 5, column: 1, scope: !5)
!8 = !DILocation(line: 1, column: 1, scope: !5)
!9 = distinct !DISubprogram(name: "f", scope: !5, file: !1, line: 1, type: !6, scopeLine: 1, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!10 = !DILocation(line: 2, column: 2, scope: !9)
!11 = distinct !DISubprogram(name: "f2", scope: !5, file: !1, line: 7, type: !12, scopeLine: 7, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!12 = !DISubroutineType(types: !13)
!13 = !{!14}
!14 = !DIBasicType(name: "Int", size: 64, encoding: DW_ATE_unsigned)
!15 = distinct !DISubprogram(name: "f3", scope: !5, file: !1, line: 11, type: !16, scopeLine: 11, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!16 = !DISubroutineType(types: !17)
!17 = !{!14, !14}
!18 = distinct !DISubprogram(name: "f4", scope: !5, file: !1, line: 15, type: !6, scopeLine: 15, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !19)
!19 = !{!20, !21, !23, !24, !26, !28, !30}
!20 = !DILocalVariable(name: "local_var", scope: !18, file: !1, line: 19, type: !14, align: 8)
!21 = !DILocalVariable(name: "if_test", scope: !22, file: !1, line: 22, type: !14, align: 8)
!22 = distinct !DILexicalBlock(scope: !18, file: !1, line: 21, column: 2)
!23 = !DILocalVariable(name: "int", scope: !18, file: !1, line: 25, type: !14, align: 8)
!24 = !DILocalVariable(name: "float", scope: !18, file: !1, line: 26, type: !25, align: 8)
!25 = !DIBasicType(name: "Float", size: 64, encoding: DW_ATE_float)
!26 = !DILocalVariable(name: "rune", scope: !18, file: !1, line: 27, type: !27, align: 8)
!27 = !DIBasicType(name: "Rune", size: 32, encoding: DW_ATE_UTF)
!28 = !DILocalVariable(name: "bool", scope: !18, file: !1, line: 28, type: !29, align: 8)
!29 = !DIBasicType(name: "Bool", size: 1, encoding: DW_ATE_boolean)
!30 = !DILocalVariable(name: "str", scope: !18, file: !1, line: 29, type: !31, align: 8)
!31 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "Str", baseType: !32, size: 64, dwarfAddressSpace: 0)
!32 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!33 = !DILocation(line: 19, column: 6, scope: !18)
!34 = !DILocation(line: 22, column: 7, scope: !22)
!35 = !DILocation(line: 25, column: 6, scope: !18)
!36 = !DILocation(line: 26, column: 6, scope: !18)
!37 = !DILocation(line: 27, column: 6, scope: !18)
!38 = !DILocation(line: 28, column: 6, scope: !18)
!39 = !DILocation(line: 29, column: 6, scope: !18)
