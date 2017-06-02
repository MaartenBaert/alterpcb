/*
Copyright (C) 2016  The AlterPCB team
Contact: Maarten Baert <maarten-baert@hotmail.com>

This file is part of AlterPCB.

AlterPCB is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

AlterPCB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this AlterPCB.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Basics.h"
#include "Json.h"
#include "StringRegistry.h"

#include "catch.hpp"

#include <random>

TEST_CASE("JSON round-trip, basic data", "[json]") {

	// example data from Wikipedia
	VData data1 = MakeVDict(
		SRNewTag("firstName"), "John",
		SRNewTag("lastName"), "Smith",
		SRNewTag("isAlive"), true,
		SRNewTag("age"), 25,
		SRNewTag("address"), MakeVDict(
			SRNewTag("streetAddress"), "21 2nd Street",
			SRNewTag("city"), "New York",
			SRNewTag("state"), "NY",
			SRNewTag("postalCode"), "10021-3100"
		),
		SRNewTag("phoneNumbers"), MakeVList(
			MakeVDict(
				SRNewTag("type"), "home",
				SRNewTag("number"), "212 555-1234"
			),
			MakeVDict(
				SRNewTag("type"), "office",
				SRNewTag("number"), "646 555-4567"
			),
			MakeVDict(
				SRNewTag("type"), "mobile",
				SRNewTag("number"), "123 456-7890"
			)
		),
		SRNewTag("children"), MakeVList(),
		SRNewTag("spouse"), nullptr
	);

	std::string str;
	Json::ToString(data1, str);

	VData data2;
	Json::FromString(data2, str);

	REQUIRE(data1 == data2);

}

TEST_CASE("JSON round-trip, integer", "[json]") {

	VData data1 = MakeVList(
		-491643673,  1047971138,  -841236793,  1156147561,  -839518223, -1377695557, -1392072969, -1614830157,
		1610102598, -1209200616,   100414215,   487503594,   222667517,  -175554517, -1406192271, -2071287907,
		INT64_C(-7110541475446039392), INT64_C(-1737212338115068923), INT64_C( 7932716620826515944), INT64_C( -793171731155154287),
		INT64_C( 6925779862299188949), INT64_C(-4084387965577959405), INT64_C( 7138059625991908679), INT64_C(-6111336815277294464),
		INT64_C(-5931155557083581104), INT64_C( 5414494821341682757), INT64_C(-7113832751220038376), INT64_C( -667358432979985914),
		INT64_C( 2393593880830494377), INT64_C(-5829428399003651041), INT64_C( 1233145152686995229), INT64_C(-3376677486094172642)
	);

	std::string str;
	Json::ToString(data1, str);

	VData data2;
	Json::FromString(data2, str);

	REQUIRE(data1 == data2);

}

TEST_CASE("JSON round-trip, floating point", "[json]") {

	VData data1 = MakeVList(
		0.0, 1.0, -1.0, 123.456, -123.456,
		242.240e+93, -838.958e+54, 999.297e+19, -993.225e-51, 106.870e+77, -222.505e+07, 317.087e+96, -914.829e+71,
		931.813e+22, -117.116e-54, 251.562e-03, -869.338e-68, 796.046e+27, -328.093e+48, 907.018e-70, -211.852e-22,
		sqrt(101), sqrt(102), sqrt(103), sqrt(104), sqrt(105), sqrt(106), sqrt(107), sqrt(108)
	);

	std::string str;
	Json::ToString(data1, str);

	VData data2;
	Json::FromString(data2, str);

	REQUIRE(data1 == data2);

}

TEST_CASE("JSON round-trip, floating point, special values", "[json]") {

	VData data1 = MakeVList(
		0.0,
		std::numeric_limits<double>::quiet_NaN(),
		std::numeric_limits<double>::infinity(),
		-std::numeric_limits<double>::infinity()
	);

	std::string str;
	Json::ToString(data1, str);

	VData data2;
	Json::FromString(data2, str);

	REQUIRE(data2.GetType() == VDATA_LIST);
	REQUIRE(data2.AsList().size() == 4);
	REQUIRE(data2.AsList()[0].GetType() == VDATA_FLOAT);
	REQUIRE(data2.AsList()[1].GetType() == VDATA_FLOAT);
	REQUIRE(data2.AsList()[2].GetType() == VDATA_FLOAT);
	REQUIRE(data2.AsList()[3].GetType() == VDATA_FLOAT);
	REQUIRE(!std::isnan(data2.AsList()[0].AsFloat()));
	REQUIRE(std::isnan(data2.AsList()[1].AsFloat()));
	REQUIRE(std::isinf(data2.AsList()[2].AsFloat()));
	REQUIRE(std::isinf(data2.AsList()[3].AsFloat()));
	REQUIRE(data2.AsList()[0].AsFloat() == 0.0);
	REQUIRE(data2.AsList()[1].AsFloat() != 0.0);
	REQUIRE(data2.AsList()[2].AsFloat() > 0.0);
	REQUIRE(data2.AsList()[3].AsFloat() < 0.0);

}

TEST_CASE("JSON round-trip, floating point, random", "[json]") {

	Json::Format format;
	format.multiline = false;
	format.precision = 17;

	// generate random doubles, with zero and subnormals, but without inf/nan
	std::mt19937_64 rng(963258741);
	std::uniform_int_distribution<uint64_t> dist(0x0000000000000000, 0x7fefffffffffffff);

	for(size_t i = 0; i < 100; ++i) {

		// generate data
		VData data1;
		VData::List &ref1 = data1.NewList(1000);
		for(size_t j = 0; j < ref1.size(); ++j) {
			uint64_t val = dist(rng) | ((rng() & 1) << 63);
			ref1[j] = MemCast<double>(val);
		}

		// convert
		std::string str;
		Json::ToString(data1, str, format);
		VData data2;
		Json::FromString(data2, str);

		// verify
		bool equal = (data1 == data2);
		if(!equal) { // find out exactly what failed
			REQUIRE(data2.GetType() == VDATA_LIST);
			const VData::List &ref2 = data2.AsList();
			for(size_t j = 0; j < ref1.size(); ++j) {
				uint64_t val1 = MemCast<uint64_t>(ref1[j].AsFloat());
				uint64_t val2 = MemCast<uint64_t>(ref2[j].AsFloat());
				REQUIRE(val1 == val2);
			}
		}
		REQUIRE(equal);

	}

}

TEST_CASE("JSON floating point formatting, scientific", "[json]") {

	Json::Format format;
	format.multiline = false;
	format.precision = 15;
	format.engineering = false;

	VData data = MakeVList(
		1.234e-10, 1.234e-9, 1.234e-8, 1.234e-7, 1.234e-6, 1.234e-5, 1.234e-4,
		1.234e-3 , 1.234e-2, 1.234e-1, 1.234e+0, 1.234e+1, 1.234e+2, 1.234e+3,
		1.234e+4 , 1.234e+5, 1.234e+6, 1.234e+7, 1.234e+8, 1.234e+9, 1.234e+10,
		0.0);
	std::string str, ref;
	Json::ToString(data, str, format);
	ref = "[1.234e-10, 1.234e-9, 1.234e-8, 1.234e-7, 1.234e-6, 1.234e-5, 1.234e-4, "
		  "0.001234, 0.01234, 0.1234, 1.234, 12.34, 123.4, 1234.0, "
		  "12340.0, 123400.0, 1234000.0, 12340000.0, 123400000.0, 1.234e+9, 1.234e+10, "
		  "0.0]";

	REQUIRE(str == ref);

}

TEST_CASE("JSON floating point formatting, engineering", "[json]") {

	Json::Format format;
	format.multiline = false;
	format.precision = 15;
	format.engineering = true;

	VData data = MakeVList(
		1.234e-10, 1.234e-9, 1.234e-8, 1.234e-7, 1.234e-6, 1.234e-5, 1.234e-4,
		1.234e-3 , 1.234e-2, 1.234e-1, 1.234e+0, 1.234e+1, 1.234e+2, 1.234e+3,
		1.234e+4 , 1.234e+5, 1.234e+6, 1.234e+7, 1.234e+8, 1.234e+9, 1.234e+10,
		0.0);
	std::string str, ref;
	Json::ToString(data, str, format);
	ref = "[123.4e-12, 1.234e-9, 12.34e-9, 123.4e-9, 1.234e-6, 12.34e-6, 123.4e-6, "
		  "0.001234, 0.01234, 0.1234, 1.234, 12.34, 123.4, 1234.0, "
		  "12340.0, 123400.0, 1234000.0, 12340000.0, 123400000.0, 1.234e+9, 12.34e+9, "
		  "0.0]";

	REQUIRE(str == ref);

}

TEST_CASE("JSON floating point parsing", "[json]") {

	VData data1;
	Json::FromString(data1, "12345678");
	REQUIRE(data1.GetType() == VDATA_INT);
	REQUIRE(data1.AsInt() == 12345678);

	VData data2;
	Json::FromString(data2, "1234.5678");
	REQUIRE(data2.GetType() == VDATA_FLOAT);
	REQUIRE(data2.AsFloat() == 1234.5678);

	VData data3;
	Json::FromString(data3, "1234.");
	REQUIRE(data3.GetType() == VDATA_FLOAT);
	REQUIRE(data3.AsFloat() == 1234.0);

	VData data4;
	Json::FromString(data4, ".1234");
	REQUIRE(data4.GetType() == VDATA_FLOAT);
	REQUIRE(data4.AsFloat() == 0.1234);

	VData data5;
	Json::FromString(data5, "0.");
	Json::FromString(data5, ".0");
	REQUIRE_THROWS(Json::FromString(data5, "."));

	VData data6;
	Json::FromString(data6, "100000000000000000000000000000000000000000000000000");
	REQUIRE(data6.GetType() == VDATA_FLOAT);
	REQUIRE(data6.AsFloat() == 1e50);

	VData data7;
	Json::FromString(data7, "100000000000000000000000000000000000000000000000000e-350");
	REQUIRE(data7.GetType() == VDATA_FLOAT);
	REQUIRE(data7.AsFloat() == 1e-300);

	VData data8;
	Json::FromString(data8, "1e400");
	REQUIRE(data8.GetType() == VDATA_FLOAT);
	REQUIRE(std::isinf(data8.AsFloat()));

	VData data9;
	Json::FromString(data9, "100000000000000000000000000000000000000000000000000e300");
	REQUIRE(data9.GetType() == VDATA_FLOAT);
	REQUIRE(std::isinf(data9.AsFloat()));

	VData data10;
	Json::FromString(data10, "0.9999999999999999999999999999999999999999");
	REQUIRE(data10.GetType() == VDATA_FLOAT);
	REQUIRE(data10.AsFloat() == 1.0);

}

// These tests can be downloaded from the official JSON website. Since our JSON parser is a bit more flexible than the
// JSON standard requires, some tests which should fail according to the standard will in fact succeed. This is ok.
TEST_CASE("JSON test suite, example1.json", "[json]") { VData data; Json::FromFile(data, "data/json-testsuite/example1.json"); }
TEST_CASE("JSON test suite, example2.json", "[json]") { VData data; Json::FromFile(data, "data/json-testsuite/example2.json"); }
TEST_CASE("JSON test suite, example3.json", "[json]") { VData data; Json::FromFile(data, "data/json-testsuite/example3.json"); }
TEST_CASE("JSON test suite, example4.json", "[json]") { VData data; Json::FromFile(data, "data/json-testsuite/example4.json"); }
TEST_CASE("JSON test suite, example5.json", "[json]") { VData data; Json::FromFile(data, "data/json-testsuite/example5.json"); }
TEST_CASE("JSON test suite, pass1.json", "[json]") { VData data; Json::FromFile(data, "data/json-testsuite/pass1.json"); }
TEST_CASE("JSON test suite, pass2.json", "[json]") { VData data; Json::FromFile(data, "data/json-testsuite/pass2.json"); }
TEST_CASE("JSON test suite, pass3.json", "[json]") { VData data; Json::FromFile(data, "data/json-testsuite/pass3.json"); }
TEST_CASE("JSON test suite, fail1.json", "[json]") { VData data; Json::FromFile(data, "data/json-testsuite/fail1.json"); }
TEST_CASE("JSON test suite, fail2.json", "[json]") { VData data; REQUIRE_THROWS(Json::FromFile(data, "data/json-testsuite/fail2.json")); }
TEST_CASE("JSON test suite, fail3.json", "[json]") { VData data; REQUIRE_THROWS(Json::FromFile(data, "data/json-testsuite/fail3.json")); }
TEST_CASE("JSON test suite, fail4.json", "[json]") { VData data; Json::FromFile(data, "data/json-testsuite/fail4.json"); }
TEST_CASE("JSON test suite, fail5.json", "[json]") { VData data; REQUIRE_THROWS(Json::FromFile(data, "data/json-testsuite/fail5.json")); }
TEST_CASE("JSON test suite, fail6.json", "[json]") { VData data; REQUIRE_THROWS(Json::FromFile(data, "data/json-testsuite/fail6.json")); }
TEST_CASE("JSON test suite, fail7.json", "[json]") { VData data; REQUIRE_THROWS(Json::FromFile(data, "data/json-testsuite/fail7.json")); }
TEST_CASE("JSON test suite, fail8.json", "[json]") { VData data; REQUIRE_THROWS(Json::FromFile(data, "data/json-testsuite/fail8.json")); }
TEST_CASE("JSON test suite, fail9.json", "[json]") { VData data; Json::FromFile(data, "data/json-testsuite/fail9.json"); }
TEST_CASE("JSON test suite, fail10.json", "[json]") { VData data; REQUIRE_THROWS(Json::FromFile(data, "data/json-testsuite/fail10.json")); }
TEST_CASE("JSON test suite, fail11.json", "[json]") { VData data; REQUIRE_THROWS(Json::FromFile(data, "data/json-testsuite/fail11.json")); }
TEST_CASE("JSON test suite, fail12.json", "[json]") { VData data; REQUIRE_THROWS(Json::FromFile(data, "data/json-testsuite/fail12.json")); }
TEST_CASE("JSON test suite, fail13.json", "[json]") { VData data; Json::FromFile(data, "data/json-testsuite/fail13.json"); }
TEST_CASE("JSON test suite, fail14.json", "[json]") { VData data; REQUIRE_THROWS(Json::FromFile(data, "data/json-testsuite/fail14.json")); }
TEST_CASE("JSON test suite, fail15.json", "[json]") { VData data; REQUIRE_THROWS(Json::FromFile(data, "data/json-testsuite/fail15.json")); }
TEST_CASE("JSON test suite, fail16.json", "[json]") { VData data; REQUIRE_THROWS(Json::FromFile(data, "data/json-testsuite/fail16.json")); }
TEST_CASE("JSON test suite, fail17.json", "[json]") { VData data; REQUIRE_THROWS(Json::FromFile(data, "data/json-testsuite/fail17.json")); }
TEST_CASE("JSON test suite, fail18.json", "[json]") { VData data; Json::FromFile(data, "data/json-testsuite/fail18.json"); }
TEST_CASE("JSON test suite, fail19.json", "[json]") { VData data; REQUIRE_THROWS(Json::FromFile(data, "data/json-testsuite/fail19.json")); }
TEST_CASE("JSON test suite, fail20.json", "[json]") { VData data; REQUIRE_THROWS(Json::FromFile(data, "data/json-testsuite/fail20.json")); }
TEST_CASE("JSON test suite, fail21.json", "[json]") { VData data; REQUIRE_THROWS(Json::FromFile(data, "data/json-testsuite/fail21.json")); }
TEST_CASE("JSON test suite, fail22.json", "[json]") { VData data; REQUIRE_THROWS(Json::FromFile(data, "data/json-testsuite/fail22.json")); }
TEST_CASE("JSON test suite, fail23.json", "[json]") { VData data; REQUIRE_THROWS(Json::FromFile(data, "data/json-testsuite/fail23.json")); }
TEST_CASE("JSON test suite, fail24.json", "[json]") { VData data; REQUIRE_THROWS(Json::FromFile(data, "data/json-testsuite/fail24.json")); }
TEST_CASE("JSON test suite, fail25.json", "[json]") { VData data; REQUIRE_THROWS(Json::FromFile(data, "data/json-testsuite/fail25.json")); }
TEST_CASE("JSON test suite, fail26.json", "[json]") { VData data; REQUIRE_THROWS(Json::FromFile(data, "data/json-testsuite/fail26.json")); }
TEST_CASE("JSON test suite, fail27.json", "[json]") { VData data; REQUIRE_THROWS(Json::FromFile(data, "data/json-testsuite/fail27.json")); }
TEST_CASE("JSON test suite, fail28.json", "[json]") { VData data; REQUIRE_THROWS(Json::FromFile(data, "data/json-testsuite/fail28.json")); }
TEST_CASE("JSON test suite, fail29.json", "[json]") { VData data; REQUIRE_THROWS(Json::FromFile(data, "data/json-testsuite/fail29.json")); }
TEST_CASE("JSON test suite, fail30.json", "[json]") { VData data; REQUIRE_THROWS(Json::FromFile(data, "data/json-testsuite/fail30.json")); }
TEST_CASE("JSON test suite, fail31.json", "[json]") { VData data; REQUIRE_THROWS(Json::FromFile(data, "data/json-testsuite/fail31.json")); }
TEST_CASE("JSON test suite, fail32.json", "[json]") { VData data; REQUIRE_THROWS(Json::FromFile(data, "data/json-testsuite/fail32.json")); }
TEST_CASE("JSON test suite, fail33.json", "[json]") { VData data; REQUIRE_THROWS(Json::FromFile(data, "data/json-testsuite/fail33.json")); }
