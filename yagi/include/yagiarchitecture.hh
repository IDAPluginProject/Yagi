#ifndef __YAGI_ARCHITECTURE__
#define __YAGI_ARCHITECTURE__

#include "symbolinfo.hh"
#include "typeinfo.hh"
#include "logger.hh"

#include <libdecomp.hh>

namespace yagi 
{
	/*!
	 *	\brief	Main Ghidra core class
	 *			Use to centralize all class associated with
	 *			Type and symbol management
	 */
	class YagiArchitecture : public SleighArchitecture
	{
	protected:
		std::unique_ptr<SymbolInfoFactory> m_symbols;

		/*!
		 *	\brief use as type backend factory
		 */
		std::unique_ptr<TypeInfoFactory> m_type;

		/*!
		 *	\brief	allow object that have access to the core
		 *			to print informations message to the end user
		 */
		std::unique_ptr<Logger> m_logger;

		/*!
		 *	\brief	stream use by SleighArchitecture to write some log
		 */
		std::stringstream m_err;

		/*!
		 *	\brief	Factory function override to build our internal scope
		 *			Scopes are used to reselve symbols
		 */
		Scope* buildDatabase(DocumentStorage& store) override;

		/*!
		 *	\brief	Internal factory function override
		 *			Use to build an internal loader
		 *			Loader is used to load program (read bytes)
		 */
		void buildLoader(DocumentStorage& store) override;

		/*!
		 *	\brief	Overriden factory function
		 *			Use to set our own type factory
		 */
		void buildTypegrp(DocumentStorage& store) override;

	public:
		/*!
		 *	\brief	default ctor
		 *	\param	name	name of the decompilation file
		 *	\param	sleighId	ID of decompiler to used
		 *	\param	logger	logger used by the entire program to print user informations
		 *	\param	symbols	symbol factory backend, use to find symbol
		 *	\param	type	type factory backend, use to find type informations
		 */
		explicit YagiArchitecture(
			const std::string& name,
			const std::string& sleighId,
			std::unique_ptr<Logger> logger,
			std::unique_ptr<SymbolInfoFactory> symbols,
			std::unique_ptr<TypeInfoFactory> type
		);

		virtual ~YagiArchitecture() = default;

		/*!
		 *	\brief	copy is disable because we have some ressource not copyable
		 */
		YagiArchitecture(const YagiArchitecture&) = delete;
		YagiArchitecture& operator=(const YagiArchitecture&) = delete;

		/*!
		 *	\brief	Move is authorized
		 */
		YagiArchitecture(YagiArchitecture&&) noexcept = default;
		YagiArchitecture& operator=(YagiArchitecture&&) = default;

		/*!
		 *	\brief	Access to the symbol factory backend
		 *	\return	An implementation of the symbol factory
		 */
		SymbolInfoFactory* getSymbolDatabase() const;

		/*!
		 *	\brief	Access to the type factory backend
		 *	\return	An implementation of a type info factory
		 */
		TypeInfoFactory* getTypeInfoFactory() const;

		/*!
		 *	\brief	Access to the current logger
		 *	\return	An implementation of a logger
		 */
		Logger& getLogger() const;
	};
}

#endif