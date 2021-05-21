#include "idatypeinfo.hh"
#include "base.hh"
#include "exception.hh"
#include "idatool.hh"

#include <algorithm>
#include <sstream>


namespace yagi 
{
	IdaTypeInfo::IdaTypeInfo(tinfo_t id)
		: m_type{ id }
	{

	}

	IdaTypeInfo::~IdaTypeInfo()
	{

	}

	/**********************************************************************/
	std::vector<std::unique_ptr<TypeInfo>> IdaTypeInfo::getFuncPrototype() const
	{
		std::vector<std::unique_ptr<TypeInfo>> result;
		func_type_data_t funcInfo;
		if (!m_type.is_func() || !m_type.get_func_details(&funcInfo))
		{
			throw TypeIsNotAFunction(getName());
		}

		result.push_back(std::make_unique<IdaTypeInfo>(funcInfo.rettype));

		idatool::transform<funcarg_t, std::unique_ptr<IdaTypeInfo>>(
			funcInfo, std::back_insert_iterator(result),
			[this](const funcarg_t& arg) -> std::unique_ptr<IdaTypeInfo>
			{
				return std::make_unique<IdaTypeInfo>(arg.type);
			}
		);

		return result;
	}

	/**********************************************************************/
	std::vector<std::string> IdaTypeInfo::getFuncParamName() const
	{
		std::vector<std::string> result;
		func_type_data_t funcInfo;
		if (!m_type.is_func() || !m_type.get_func_details(&funcInfo))
		{
			throw TypeIsNotAFunction(getName());
		}

		// retrieve the output type
		result.push_back("");

		uint16_t index = 0;
		idatool::transform<funcarg_t, std::string>(
			funcInfo, std::back_insert_iterator(result),
			[&index](const funcarg_t& arg) -> std::string
			{
				index++;
				auto name = std::string(arg.name.c_str());
				if (name.empty())
				{
					std::stringstream ss;
					ss << "a" << index;
					return ss.str();
				}
				else
				{
					return name.c_str();
				}
			}
		);

		return result;
	}

	size_t IdaTypeInfo::getSize() const
	{
		return m_type.get_size();
	}

	bool IdaTypeInfo::isInt() const
	{
		return m_type.is_int();
	}

	std::string IdaTypeInfo::getName() const
	{
		qstring name;
		m_type.print(&name);
		return name.c_str();
	}

	bool IdaTypeInfo::isPtr() const
	{
		return m_type.is_ptr();
	}

	std::unique_ptr<TypeInfo> IdaTypeInfo::getPointedObject() const
	{
		if (isPtr()) {
			return std::make_unique<IdaTypeInfo>(m_type.get_pointed_object());
		}
		if (isArray()) {
			return std::make_unique<IdaTypeInfo>(m_type.get_array_element());
		}
		throw InvalidType("The current is not a pointer of an array");
	}

	bool IdaTypeInfo::isBool() const
	{
		return m_type.is_bool();
	}

	bool IdaTypeInfo::isFloat() const
	{
		return m_type.is_float();
	}

	bool IdaTypeInfo::isStruct() const
	{
		return m_type.is_struct();
	}

	bool IdaTypeInfo::isDotDotDot() const
	{
		return m_type.is_vararg_cc();
	}

	bool IdaTypeInfo::isVoid() const
	{
		return m_type.is_void();
	}

	bool IdaTypeInfo::isFunc() const
	{
		return m_type.is_func();
	}

	bool IdaTypeInfo::isConst() const
	{
		return m_type.is_const();
	}

	bool IdaTypeInfo::isChar() const
	{
		return m_type.is_char();
	}

	bool IdaTypeInfo::isArray() const
	{
		return m_type.is_array();
	}

	std::vector<TypeStructField> IdaTypeInfo::getFields() const
	{
		udt_type_data_t attributes;
		m_type.get_udt_details(&attributes);
		std::vector<TypeStructField> result;

		idatool::transform<udt_member_t, TypeStructField>(
			attributes, std::back_insert_iterator(result),
			[this](const udt_member_t& arg) -> TypeStructField
			{
				return TypeStructField { arg.offset / 8, arg.name.c_str(), std::make_unique<IdaTypeInfo>(arg.type) };
			}
		);

		return result;
	}

	uint64_t IdaTypeInfo::getArraySize() const
	{
		return m_type.get_array_nelems();
	}

	IdaTypeInfoFactory::IdaTypeInfoFactory()
	{

	}

	IdaTypeInfoFactory::~IdaTypeInfoFactory()
	{}

	std::optional<std::unique_ptr<TypeInfo>> IdaTypeInfoFactory::build(const std::string& name)
	{
		// Rewrite raw type
		tinfo_t idaTypeInfo;

		if (!idaTypeInfo.get_named_type(get_idati(), name.c_str()))
		{
			return std::nullopt;
		}

		return std::make_unique<IdaTypeInfo>(idaTypeInfo);
	}

	std::optional<std::unique_ptr<TypeInfo>> IdaTypeInfoFactory::build(uint64_t ea)
	{
		tinfo_t idaTypeInfo;

		if (!get_tinfo(&idaTypeInfo, ea) && !guess_tinfo(&idaTypeInfo, ea))
		{
			return std::nullopt;
		}

		return std::make_unique<IdaTypeInfo>(idaTypeInfo);
	}

} // end of namespace yagi