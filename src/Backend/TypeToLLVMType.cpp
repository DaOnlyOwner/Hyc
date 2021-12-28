#include "TypeToLLVMType.h"
#include "Scopes.h"
#include "Messages.h"
#include "AtomicType.h"
#include "Scopes.h"


llvm::Type* map_type(const Type& from, const Scopes& scopes, llvm::LLVMContext& ctxt)
{
	llvm::Type* out = nullptr;
	//static_assert(std::is_base_of_v<llvm::Type, llvm::IntegerType>, "LOLWTF");
	assert(!from.must_be_inferred());
	for (auto& [kind, var] : from.stored_types)
	{
		switch (kind)
		{
		case TypeKind::Base:
		{
			auto base = std::get<TypeDefStmt*>(var);
			if (scopes.is_type_predefined(base))
			{
				auto pd = scopes.get_predefined_type(base);
				switch (pd)
				{
				case PredefinedType::Bool:
					out = static_cast<llvm::Type*>(llvm::Type::getInt1Ty(ctxt));
					break;
				case PredefinedType::UChar:
				case PredefinedType::Char:
					out = llvm::Type::getInt8Ty(ctxt);
					break;
				case PredefinedType::UHalf:
				case PredefinedType::Half:
					out = llvm::Type::getInt32Ty(ctxt);
					break;
				case PredefinedType::UShort:
				case PredefinedType::Short:
					out = llvm::Type::getInt16Ty(ctxt);
					break;
				case PredefinedType::Int:
				case PredefinedType::UInt:
					out = llvm::Type::getInt64Ty(ctxt);
					break;
				case PredefinedType::Void:
					out = llvm::Type::getVoidTy(ctxt);
					break;
				case PredefinedType::Float:
					out = llvm::Type::getFloatTy(ctxt);
					break;
				case PredefinedType::Double:
					out = llvm::Type::getDoubleTy(ctxt);
					break;
				case PredefinedType::Quad:
					out = llvm::Type::getFP128Ty(ctxt);
					break;
				}
			}

			else
			{
				out = base->llvm_struct_type;
			}
		}
		break;
		case TypeKind::Pointer:
		{
			out = out->getPointerTo();
		}
		break;
		case TypeKind::Array:
		{
			auto& val = std::get<ArrayType>(var);
			out = llvm::ArrayType::get(out, val.amount);
		
		}
		break;
		case TypeKind::FunctionPointer:
		{
			auto& val = std::get<FunctionPointerType>(var);
			std::vector<llvm::Type*> params;
			params.reserve(val.args.size());
			for (auto& p : val.args)
			{
				params.push_back(map_type(*p, scopes, ctxt));
			}
			auto ret_type = map_type(*val.return_type, scopes, ctxt);
			out = llvm::FunctionType::get(ret_type, params, false);
			out = out->getPointerTo();
		}
		break;
		default:
			assert(false);
			break;
		}
	}
	return out;
}

llvm::FunctionType* get_function_type(const FunctionPointerType& fptr, const Scopes& scopes, llvm::LLVMContext& ctxt)
{
	std::vector<llvm::Type*> arg_types;
	for (auto& p : fptr.args)
	{
		arg_types.push_back(map_type(*p, scopes, ctxt));
	}

	auto ret_t = map_type(*fptr.return_type, scopes, ctxt);
	return llvm::FunctionType::get(ret_t, arg_types, false);
}
