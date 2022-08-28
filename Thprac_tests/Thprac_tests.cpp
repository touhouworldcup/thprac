#include "pch.h"
#include "CppUnitTest.h"
#include "../thprac/src/thprac/thprac_utils.h"
#include "../thprac/src/3rdParties/rapidjson/include/rapidjson/document.h"
#include "../thprac/src/3rdParties/rapidjson/include/rapidjson/writer.h"
#include "../thprac/src/3rdParties/rapidjson/include/rapidjson/stringbuffer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Thpractests
{
	TEST_CLASS(Thpractests)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
            const char* json = R"(
				{
					"label": "test",
					"sub_warps": [
						{
							"label": "test2",
						}
					],
				}
			)";
			Document d;
			d.Parse(json);

		}
	};
}
