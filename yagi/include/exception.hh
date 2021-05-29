#ifndef __YAGI_ERROR__
#define __YAGI_ERROR__

#include <exception>
#include <string>

namespace yagi 
{
	/*!
	 * \brief	Base class for all Yagi exception
	 */
	class Error : public std::exception
	{
	protected:
		/*!
		 * \brief	message that represent the exception
		 */
		std::string m_reason;
	public:

		/*!
		 * \brief	ctor
		 */
		explicit Error(std::string reason);

		/*!
		 * \brief	Default destructor
		 */
		virtual ~Error() = default;

		virtual char const* what() const override;
	};

	/*!
	 * \brief	The type is unknown from the database point of view
	 */
	class UnknownTypeError : public Error
	{
	public:
		explicit UnknownTypeError(const std::string& typeName);
	};

	/*!
	 * \brief	It seems that the symbol is not linked to a function
	 */
	class SymbolIsNotAFunction : public Error
	{
	public:
		explicit SymbolIsNotAFunction(const std::string& functionName);
	};

	/*!
	 * \brief	Try to access a type as function
	 *			but it's not a function
	 */
	class TypeIsNotAFunction : public Error
	{
	public:
		explicit TypeIsNotAFunction(const std::string& typeName);
	};

	/*!
	 * \brief	Unable to find a function into backend database
	 */
	class UnableToFindFunction : public Error
	{
	public:
		explicit UnableToFindFunction(uint64_t ea);
	};

	/*!
	 * \brief	Unable to find a particular type into backend database
	 */
	class UnableToFindType : public Error
	{
	public:
		explicit UnableToFindType(uint32_t typeName);
	};

	/*!
	 * \brief	Unable to find a prototype associated with a function type
	 */
	class UnableToFindPrototype : public Error
	{
	public:
		explicit UnableToFindPrototype(std::string funName);
	};

	/*!
	 * \brief	Unknown calling convention for a function type 
	 */
	class UnknownCallingConvention : public Error
	{
	public:
		explicit UnknownCallingConvention(std::string funcName);
	};
}

#endif