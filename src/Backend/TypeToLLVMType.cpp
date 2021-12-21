#include "TypeToLLVMType.h"
#include "Scopes.h"
#include "Messages.h"
#include "AtomicType.h"
#include "Scopes.h"
#include "llvm/IR/DerivedTypes.h"

llvm::Type* map_type(const Type& from, const Scopes& scopes, llvm::LLVMContext& ctxt)
{
	llvm::Type* out = nullptr;
	//static_assert(std::is_base_of_v<llvm::Type, llvm::IntegerType>, "LOLWTF");
	for (auto& [kind, var] : from.stored_types)
	{
		switch (kind)
		{
		case TypeKind::Base:
		{
			auto base = std::get<CollectionStmt*>(var);
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
			NOT_IMPLEMENTED;
		default:
			assert(false);
			break;
		}
	}
	return out;
}
