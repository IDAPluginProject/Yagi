#include <gtest/gtest.h>
#include "yagiarchitecture.hh"
#include "mock_logger_test.h"
#include "mock_symbol_test.h"
#include "mock_type_test.h"
#include "mock_loader_test.h"
#include "ghidra.hh"

#define FUNC_ADDR 0xaaaaaaaa
#define FUNC_SIZE 20
#define FUNC_NAME "test"

static const uint8_t PAYLOAD[] = {
	0x03, 0x00, 0x07, 0x30, 0xC4, 0x00, 0x60, 0x34, 0x83, 0x2A, 0x20, 0x03, 0x91, 0x2A, 0x20, 0x05,
	0x90, 0x00, 0x40, 0x08, 0x82, 0x00, 0x80, 0x08, 0xC0, 0x20, 0x80, 0x08, 0xC0, 0x20, 0x60, 0x04,
	0xC0, 0x20, 0x60, 0x08, 0xC0, 0x70, 0x60, 0x10, 0xC0, 0x70, 0x60, 0x18, 0xC0, 0x20, 0x60, 0x0C,
	0x81, 0xC3, 0xE0, 0x08, 0xC0, 0x70, 0x60, 0x20, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00
	};

// Demonstrate some basic assertions.
TEST(TestDecompilationPayload_sparc_32, DecompileWithoutType) {

	yagi::ghidra::init(std::getenv("GHIDRADIRTEST"));

	auto arch = std::make_unique<yagi::YagiArchitecture>(
		"test",
		"sparc:BE:32:default",
		std::make_unique<MockLoaderFactory>([](uint1* ptr, int4 size, const Address& addr) {
			memcpy(ptr, PAYLOAD + addr.getOffset() - FUNC_ADDR, size);
		}),
		std::make_unique<MockLogger>([](const std::string&) {}),
		std::make_unique<MockSymbolInfoFactory>([](uint64_t ea) -> std::optional<std::unique_ptr<yagi::SymbolInfo>> {
			if (ea == FUNC_ADDR)
			{
				return std::make_unique<MockFunctionSymbolInfo>(
					FUNC_ADDR, FUNC_NAME, FUNC_SIZE, true, false, false, false
				);
			}
			return std::nullopt; 
		}, 
		[](uint64_t func_addr) -> std::optional<std::unique_ptr<yagi::SymbolInfo>> { 
			return std::nullopt;
		}),
		std::make_unique<MockTypeInfoFactory>([](uint64_t) { return std::nullopt; }, [](const std::string&) { return std::nullopt; }),
		"__stdcall"
	);

	DocumentStorage store;
	arch->init(store);

	auto scope = arch->symboltab->getGlobalScope();
	auto func = scope->findFunction(
		Address(arch->getDefaultCodeSpace(), FUNC_ADDR)
	);
	arch->allacts.getCurrent()->perform(*func);

	arch->setPrintLanguage("c-language");

	stringstream ss;
	arch->print->setOutputStream(&ss);
	//print as C
	arch->print->docFunction(func);
	
	ASSERT_STREQ(ss.str().c_str(), "\n__uint32 test(__uint32 param_1)\n\n{\n  if (unk_0xaaac4f4a != 0) {\n    unk_0xaaac4f4a = 0;\n    func_0xaaa9a2ca();\n  }\n  if ((__uint8 *)0xaaac44e8 < unk_0xaaac035a) {\n    func_0xaaaaaa4a();\n  }\n  *unk_0xaaac035a = (char)param_1;\n  unk_0xaaac035a = unk_0xaaac035a + 1;\n  unk_0xaaac4f52 = 0;\n  return param_1;\n}\n");
}