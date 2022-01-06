; ModuleID = './test/skull/codegen/llvm/first.sk'
source_filename = "./test/skull/codegen/llvm/first.sk"

@0 = private unnamed_addr constant [12 x i8] c"some string\00", align 1
@global = private global i64 1, !dbg !0

define i64 @.first() !dbg !9 {
entry:
  %0 = call i64 @f(), !dbg !11
  ret i64 0, !dbg !12
}

define private i64 @f() !dbg !13 {
entry:
  ret i64 1, !dbg !16
}

define private void @f2(i64 %0) !dbg !17 {
entry:
  %x = alloca i64, align 8
  store i64 %0, i64* %x, align 4
  call void @llvm.dbg.declare(metadata i64* %x, metadata !21, metadata !DIExpression()), !dbg !22
  %noop = alloca i64, align 8
  store i64 0, i64* %noop, align 4, !dbg !23
  ret void
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #0

define private void @f3(i64 %0, i64 %1) !dbg !24 {
entry:
  %y = alloca i64, align 8
  store i64 %0, i64* %y, align 4
  call void @llvm.dbg.declare(metadata i64* %y, metadata !28, metadata !DIExpression()), !dbg !30
  %z = alloca i64, align 8
  store i64 %1, i64* %z, align 4
  call void @llvm.dbg.declare(metadata i64* %z, metadata !29, metadata !DIExpression()), !dbg !30
  %noop = alloca i64, align 8
  store i64 0, i64* %noop, align 4, !dbg !31
  ret void
}

define private void @f4() !dbg !32 {
entry:
  %local_var = alloca i64, align 8
  store i64 1, i64* %local_var, align 4, !dbg !49
  call void @llvm.dbg.declare(metadata i64* %local_var, metadata !36, metadata !DIExpression()), !dbg !49
  br i1 true, label %if_true, label %end

if_true:                                          ; preds = %entry
  %if_test = alloca i64, align 8
  store i64 1, i64* %if_test, align 4, !dbg !50
  br label %end

end:                                              ; preds = %entry, %if_true
  call void @llvm.dbg.declare(metadata i64* %if_test, metadata !37, metadata !DIExpression()), !dbg !50
  %int = alloca i64, align 8
  store i64 1234, i64* %int, align 4, !dbg !51
  call void @llvm.dbg.declare(metadata i64* %int, metadata !39, metadata !DIExpression()), !dbg !51
  %float = alloca double, align 8
  store double 3.141500e+00, double* %float, align 8, !dbg !52
  call void @llvm.dbg.declare(metadata double* %float, metadata !40, metadata !DIExpression()), !dbg !52
  %rune = alloca i32, align 4
  store i32 120, i32* %rune, align 4, !dbg !53
  call void @llvm.dbg.declare(metadata i32* %rune, metadata !42, metadata !DIExpression()), !dbg !53
  %bool = alloca i1, align 1
  store i1 false, i1* %bool, align 1, !dbg !54
  call void @llvm.dbg.declare(metadata i1* %bool, metadata !44, metadata !DIExpression()), !dbg !54
  %str = alloca i8*, align 8
  store i8* getelementptr inbounds ([12 x i8], [12 x i8]* @0, i32 0, i32 0), i8** %str, align 8, !dbg !55
  call void @llvm.dbg.declare(metadata i8** %str, metadata !46, metadata !DIExpression()), !dbg !55
  %0 = load i1, i1* %bool, align 1, !dbg !56
  br i1 %0, label %if_true2, label %end1

if_true2:                                         ; preds = %end
  %noop = alloca i64, align 8
  store i64 0, i64* %noop, align 4, !dbg !57
  br label %end1

end1:                                             ; preds = %end, %if_true2
  ret void
}

attributes #0 = { nofree nosync nounwind readnone speculatable willreturn }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!7, !8}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "global", scope: !2, file: !3, line: 36, type: !6, isLocal: true, isDefinition: true, align: 8)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "skull", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !4, globals: !5, splitDebugInlining: false)
!3 = !DIFile(filename: "./test/skull/codegen/llvm/first.sk", directory: ".")
!4 = !{}
!5 = !{!0}
!6 = !DIBasicType(name: "Int", size: 64, encoding: DW_ATE_signed)
!7 = !{i32 4, !"Dwarf Version", i32 4}
!8 = !{i32 4, !"Debug Info Version", i32 3}
!9 = distinct !DISubprogram(name: ".first", scope: !3, file: !3, line: 1, type: !10, scopeLine: 1, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!10 = !DISubroutineType(types: !4)
!11 = !DILocation(line: 5, column: 1, scope: !9)
!12 = !DILocation(line: 1, column: 1, scope: !9)
!13 = distinct !DISubprogram(name: "f", scope: !3, file: !3, line: 1, type: !14, scopeLine: 1, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!14 = !DISubroutineType(types: !15)
!15 = !{!6}
!16 = !DILocation(line: 2, column: 2, scope: !13)
!17 = distinct !DISubprogram(name: "f2", scope: !3, file: !3, line: 7, type: !18, scopeLine: 7, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !20)
!18 = !DISubroutineType(types: !19)
!19 = !{null, !6}
!20 = !{!21}
!21 = !DILocalVariable(name: "x", arg: 1, scope: !17, file: !3, line: 7, type: !6)
!22 = !DILocation(line: 7, column: 1, scope: !17)
!23 = !DILocation(line: 8, column: 2, scope: !17)
!24 = distinct !DISubprogram(name: "f3", scope: !3, file: !3, line: 11, type: !25, scopeLine: 11, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !27)
!25 = !DISubroutineType(types: !26)
!26 = !{null, !6, !6}
!27 = !{!28, !29}
!28 = !DILocalVariable(name: "y", arg: 1, scope: !24, file: !3, line: 11, type: !6)
!29 = !DILocalVariable(name: "z", arg: 2, scope: !24, file: !3, line: 11, type: !6)
!30 = !DILocation(line: 11, column: 1, scope: !24)
!31 = !DILocation(line: 12, column: 2, scope: !24)
!32 = distinct !DISubprogram(name: "f4", scope: !3, file: !3, line: 15, type: !33, scopeLine: 15, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !35)
!33 = !DISubroutineType(types: !34)
!34 = !{null}
!35 = !{!36, !37, !39, !40, !42, !44, !46}
!36 = !DILocalVariable(name: "local_var", scope: !32, file: !3, line: 19, type: !6, align: 8)
!37 = !DILocalVariable(name: "if_test", scope: !38, file: !3, line: 22, type: !6, align: 8)
!38 = distinct !DILexicalBlock(scope: !32, file: !3, line: 21, column: 2)
!39 = !DILocalVariable(name: "int", scope: !32, file: !3, line: 25, type: !6, align: 8)
!40 = !DILocalVariable(name: "float", scope: !32, file: !3, line: 26, type: !41, align: 8)
!41 = !DIBasicType(name: "Float", size: 64, encoding: DW_ATE_float)
!42 = !DILocalVariable(name: "rune", scope: !32, file: !3, line: 27, type: !43, align: 8)
!43 = !DIBasicType(name: "Rune", size: 32, encoding: DW_ATE_UTF)
!44 = !DILocalVariable(name: "bool", scope: !32, file: !3, line: 28, type: !45, align: 8)
!45 = !DIBasicType(name: "Bool", size: 1, encoding: DW_ATE_boolean)
!46 = !DILocalVariable(name: "str", scope: !32, file: !3, line: 29, type: !47, align: 8)
!47 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "Str", baseType: !48, size: 64, dwarfAddressSpace: 0)
!48 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!49 = !DILocation(line: 19, column: 6, scope: !32)
!50 = !DILocation(line: 22, column: 7, scope: !38)
!51 = !DILocation(line: 25, column: 6, scope: !32)
!52 = !DILocation(line: 26, column: 6, scope: !32)
!53 = !DILocation(line: 27, column: 6, scope: !32)
!54 = !DILocation(line: 28, column: 6, scope: !32)
!55 = !DILocation(line: 29, column: 6, scope: !32)
!56 = !DILocation(line: 31, column: 5, scope: !32)
!57 = !DILocation(line: 32, column: 3, scope: !58)
!58 = distinct !DILexicalBlock(scope: !32, file: !3, line: 31, column: 2)
