; ModuleID = './test/skull/codegen/llvm/first.sk'
source_filename = "./test/skull/codegen/llvm/first.sk"

@0 = private unnamed_addr constant [12 x i8] c"some string\00", align 1

define i64 @.first() !dbg !5 {
entry:
  %0 = call i64 @f(), !dbg !12
  ret i64 0, !dbg !13
}

define private i64 @f() !dbg !14 {
entry:
  ret i64 1, !dbg !15
}

define private void @f2(i64 %0) !dbg !16 {
entry:
  call void @llvm.dbg.declare(metadata i64 %0, metadata !8, metadata !DIExpression()), !dbg !19
  ret void
}

; Function Attrs: nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #0

define private void @f3(i64 %0, i64 %1) !dbg !20 {
entry:
  call void @llvm.dbg.declare(metadata i64 %0, metadata !10, metadata !DIExpression()), !dbg !23
  call void @llvm.dbg.declare(metadata i64 %1, metadata !11, metadata !DIExpression()), !dbg !23
  ret void
}

define private void @f4() !dbg !24 {
entry:
  %local_var = alloca i64
  store i64 1, i64* %local_var, !dbg !39
  call void @llvm.dbg.declare(metadata i64* %local_var, metadata !26, metadata !DIExpression()), !dbg !39
  br i1 true, label %if_true, label %end

if_true:                                          ; preds = %entry
  %if_test = alloca i64
  store i64 1, i64* %if_test, !dbg !40
  br label %end

end:                                              ; preds = %entry, %if_true
  call void @llvm.dbg.declare(metadata i64* %if_test, metadata !27, metadata !DIExpression()), !dbg !40
  %int = alloca i64
  store i64 1234, i64* %int, !dbg !41
  call void @llvm.dbg.declare(metadata i64* %int, metadata !29, metadata !DIExpression()), !dbg !41
  %float = alloca double
  store double 3.141500e+00, double* %float, !dbg !42
  call void @llvm.dbg.declare(metadata double* %float, metadata !30, metadata !DIExpression()), !dbg !42
  %rune = alloca i32
  store i32 120, i32* %rune, !dbg !43
  call void @llvm.dbg.declare(metadata i32* %rune, metadata !32, metadata !DIExpression()), !dbg !43
  %bool = alloca i1
  store i1 false, i1* %bool, !dbg !44
  call void @llvm.dbg.declare(metadata i1* %bool, metadata !34, metadata !DIExpression()), !dbg !44
  %str = alloca i8*
  store i8* getelementptr inbounds ([12 x i8], [12 x i8]* @0, i32 0, i32 0), i8** %str, !dbg !45
  call void @llvm.dbg.declare(metadata i8** %str, metadata !36, metadata !DIExpression()), !dbg !45
  ret void
}

attributes #0 = { nounwind readnone speculatable willreturn }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "skull", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2, splitDebugInlining: false)
!1 = !DIFile(filename: "./test/skull/codegen/llvm/first.sk", directory: ".")
!2 = !{}
!3 = !{i32 4, !"Dwarf Version", i32 4}
!4 = !{i32 4, !"Debug Info Version", i32 3}
!5 = distinct !DISubprogram(name: ".first", scope: !1, file: !1, line: 1, type: !6, scopeLine: 1, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !7)
!6 = !DISubroutineType(types: !2)
!7 = !{!8, !10, !11}
!8 = !DILocalVariable(name: "x", arg: 1, scope: !5, file: !1, line: 7, type: !9)
!9 = !DIBasicType(name: "Int", size: 64, encoding: DW_ATE_signed)
!10 = !DILocalVariable(name: "y", arg: 1, scope: !5, file: !1, line: 11, type: !9)
!11 = !DILocalVariable(name: "z", arg: 2, scope: !5, file: !1, line: 11, type: !9)
!12 = !DILocation(line: 5, column: 1, scope: !5)
!13 = !DILocation(line: 1, column: 1, scope: !5)
!14 = distinct !DISubprogram(name: "f", scope: !1, file: !1, line: 1, type: !6, scopeLine: 1, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!15 = !DILocation(line: 2, column: 2, scope: !14)
!16 = distinct !DISubprogram(name: "f2", scope: !1, file: !1, line: 7, type: !17, scopeLine: 7, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!17 = !DISubroutineType(types: !18)
!18 = !{!9}
!19 = !DILocation(line: 7, column: 1, scope: !5)
!20 = distinct !DISubprogram(name: "f3", scope: !1, file: !1, line: 11, type: !21, scopeLine: 11, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!21 = !DISubroutineType(types: !22)
!22 = !{!9, !9}
!23 = !DILocation(line: 11, column: 1, scope: !5)
!24 = distinct !DISubprogram(name: "f4", scope: !1, file: !1, line: 15, type: !6, scopeLine: 15, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !25)
!25 = !{!26, !27, !29, !30, !32, !34, !36}
!26 = !DILocalVariable(name: "local_var", scope: !24, file: !1, line: 19, type: !9, align: 8)
!27 = !DILocalVariable(name: "if_test", scope: !28, file: !1, line: 22, type: !9, align: 8)
!28 = distinct !DILexicalBlock(scope: !24, file: !1, line: 21, column: 2)
!29 = !DILocalVariable(name: "int", scope: !24, file: !1, line: 25, type: !9, align: 8)
!30 = !DILocalVariable(name: "float", scope: !24, file: !1, line: 26, type: !31, align: 8)
!31 = !DIBasicType(name: "Float", size: 64, encoding: DW_ATE_float)
!32 = !DILocalVariable(name: "rune", scope: !24, file: !1, line: 27, type: !33, align: 8)
!33 = !DIBasicType(name: "Rune", size: 32, encoding: DW_ATE_UTF)
!34 = !DILocalVariable(name: "bool", scope: !24, file: !1, line: 28, type: !35, align: 8)
!35 = !DIBasicType(name: "Bool", size: 1, encoding: DW_ATE_boolean)
!36 = !DILocalVariable(name: "str", scope: !24, file: !1, line: 29, type: !37, align: 8)
!37 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "Str", baseType: !38, size: 64, dwarfAddressSpace: 0)
!38 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!39 = !DILocation(line: 19, column: 6, scope: !24)
!40 = !DILocation(line: 22, column: 7, scope: !28)
!41 = !DILocation(line: 25, column: 6, scope: !24)
!42 = !DILocation(line: 26, column: 6, scope: !24)
!43 = !DILocation(line: 27, column: 6, scope: !24)
!44 = !DILocation(line: 28, column: 6, scope: !24)
!45 = !DILocation(line: 29, column: 6, scope: !24)
