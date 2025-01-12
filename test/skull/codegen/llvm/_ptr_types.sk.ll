; ModuleID = './test/skull/codegen/llvm/ptr_types.sk'
source_filename = "./test/skull/codegen/llvm/ptr_types.sk"

@bool = private global i1 true, !dbg !0
@int = private global i64 1234, !dbg !5
@float = private global double 3.140000e+00, !dbg !8
@0 = private unnamed_addr constant [12 x i8] c"hello world\00", align 1
@str = private global ptr @0, !dbg !11
@rune = private global i32 120, !dbg !15
@bool_ptr = private global ptr @bool, !dbg !18
@int_ptr = private global ptr @int, !dbg !22
@float_ptr = private global ptr @float, !dbg !25
@str_ptr = private global ptr @str, !dbg !28
@rune_ptr = private global ptr @rune, !dbg !31
@double_ptr = private global ptr @int_ptr, !dbg !34

define i64 @.ptr_types() !dbg !39 {
entry:
  ret i64 0, !dbg !42
}

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!37, !38}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "bool", scope: !2, file: !3, line: 1, type: !21, isLocal: true, isDefinition: true, align: 8)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "skull", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, globals: !4, splitDebugInlining: false)
!3 = !DIFile(filename: "./test/skull/codegen/llvm/ptr_types.sk", directory: ".")
!4 = !{!0, !5, !8, !11, !15, !18, !22, !25, !28, !31, !34}
!5 = !DIGlobalVariableExpression(var: !6, expr: !DIExpression())
!6 = distinct !DIGlobalVariable(name: "int", scope: !2, file: !3, line: 2, type: !7, isLocal: true, isDefinition: true, align: 8)
!7 = !DIBasicType(name: "Int", size: 64, encoding: DW_ATE_signed)
!8 = !DIGlobalVariableExpression(var: !9, expr: !DIExpression())
!9 = distinct !DIGlobalVariable(name: "float", scope: !2, file: !3, line: 3, type: !10, isLocal: true, isDefinition: true, align: 8)
!10 = !DIBasicType(name: "Float", size: 64, encoding: DW_ATE_float)
!11 = !DIGlobalVariableExpression(var: !12, expr: !DIExpression())
!12 = distinct !DIGlobalVariable(name: "str", scope: !2, file: !3, line: 4, type: !13, isLocal: true, isDefinition: true, align: 8)
!13 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "Str", baseType: !14, size: 64, dwarfAddressSpace: 0)
!14 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!15 = !DIGlobalVariableExpression(var: !16, expr: !DIExpression())
!16 = distinct !DIGlobalVariable(name: "rune", scope: !2, file: !3, line: 5, type: !17, isLocal: true, isDefinition: true, align: 8)
!17 = !DIBasicType(name: "Rune", size: 32, encoding: DW_ATE_UTF)
!18 = !DIGlobalVariableExpression(var: !19, expr: !DIExpression())
!19 = distinct !DIGlobalVariable(name: "bool_ptr", scope: !2, file: !3, line: 7, type: !20, isLocal: true, isDefinition: true, align: 8)
!20 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "&Bool", baseType: !21, size: 64, dwarfAddressSpace: 0)
!21 = !DIBasicType(name: "Bool", size: 1, encoding: DW_ATE_boolean)
!22 = !DIGlobalVariableExpression(var: !23, expr: !DIExpression())
!23 = distinct !DIGlobalVariable(name: "int_ptr", scope: !2, file: !3, line: 8, type: !24, isLocal: true, isDefinition: true, align: 8)
!24 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "&Int", baseType: !7, size: 64, dwarfAddressSpace: 0)
!25 = !DIGlobalVariableExpression(var: !26, expr: !DIExpression())
!26 = distinct !DIGlobalVariable(name: "float_ptr", scope: !2, file: !3, line: 9, type: !27, isLocal: true, isDefinition: true, align: 8)
!27 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "&Float", baseType: !10, size: 64, dwarfAddressSpace: 0)
!28 = !DIGlobalVariableExpression(var: !29, expr: !DIExpression())
!29 = distinct !DIGlobalVariable(name: "str_ptr", scope: !2, file: !3, line: 10, type: !30, isLocal: true, isDefinition: true, align: 8)
!30 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "&Str", baseType: !13, size: 64, dwarfAddressSpace: 0)
!31 = !DIGlobalVariableExpression(var: !32, expr: !DIExpression())
!32 = distinct !DIGlobalVariable(name: "rune_ptr", scope: !2, file: !3, line: 11, type: !33, isLocal: true, isDefinition: true, align: 8)
!33 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "&Rune", baseType: !17, size: 64, dwarfAddressSpace: 0)
!34 = !DIGlobalVariableExpression(var: !35, expr: !DIExpression())
!35 = distinct !DIGlobalVariable(name: "double_ptr", scope: !2, file: !3, line: 12, type: !36, isLocal: true, isDefinition: true, align: 8)
!36 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "&&Int", baseType: !24, size: 64, dwarfAddressSpace: 0)
!37 = !{i32 4, !"Dwarf Version", i32 4}
!38 = !{i32 4, !"Debug Info Version", i32 3}
!39 = distinct !DISubprogram(name: ".ptr_types", scope: !3, file: !3, line: 1, type: !40, scopeLine: 1, spFlags: DISPFlagDefinition, unit: !2)
!40 = !DISubroutineType(types: !41)
!41 = !{}
!42 = !DILocation(line: 1, column: 1, scope: !39)
