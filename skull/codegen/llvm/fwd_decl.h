#pragma once

struct LLVMOpaqueBuilder;
typedef struct LLVMOpaqueBuilder *LLVMBuilderRef;

struct LLVMOpaqueContext;
typedef struct LLVMOpaqueContext *LLVMContextRef;

struct LLVMOpaqueModule;
typedef struct LLVMOpaqueModule *LLVMModuleRef;

struct LLVMOpaqueType;
typedef struct LLVMOpaqueType *LLVMTypeRef;

struct LLVMOpaqueValue;
typedef struct LLVMOpaqueValue *LLVMValueRef;

struct LLVMOpaqueMetadata;
typedef struct LLVMOpaqueMetadata *LLVMMetadataRef;

struct LLVMOpaqueDIBuilder;
typedef struct LLVMOpaqueDIBuilder *LLVMDIBuilderRef;
