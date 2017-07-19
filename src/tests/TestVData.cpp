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
#include "StringRegistry.h"
#include "VData.h"

#include "catch.hpp"

#include <limits>

TEST_CASE("VData relational operators", "[vdata]") {

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

	VData data2 = data1, data3 = data1;
	data2.AsDictUnique()[3].Value() = 26; // change age
	data3.AsDictUnique()[5].Value().AsListUnique()[1].AsDictUnique()[1].Value() = "222 222-2222"; // change office phone number

	REQUIRE((data1 == data1) == true);
	REQUIRE((data1 < data1) == false);
	REQUIRE((data1 > data1) == false);
	REQUIRE((data1 <= data1) == true);
	REQUIRE((data1 >= data1) == true);

	REQUIRE((data1 == data2) == false);
	REQUIRE((data1 < data2) == true);
	REQUIRE((data1 > data2) == false);
	REQUIRE((data1 <= data2) == true);
	REQUIRE((data1 >= data2) == false);

	REQUIRE((data1 == data3) == false);
	REQUIRE((data1 < data3) == false);
	REQUIRE((data1 > data3) == true);
	REQUIRE((data1 <= data3) == false);
	REQUIRE((data1 >= data3) == true);

}

TEST_CASE("VData float relational operators", "[vdata]") {

	const double DOUBLE_ZERO_POS = MemCast<double>(UINT64_C(0x0000000000000000));
	const double DOUBLE_ZERO_NEG = MemCast<double>(UINT64_C(0x8000000000000000));
	const double DOUBLE_INF_POS = std::numeric_limits<double>::infinity();
	const double DOUBLE_INF_NEG = -std::numeric_limits<double>::infinity();
	const double DOUBLE_NAN = std::numeric_limits<double>::quiet_NaN();

	std::vector<VData> data;
	if(MemCast<uint64_t>(DOUBLE_NAN) >> 63) { // NaN is negative
		data = {DOUBLE_NAN, DOUBLE_INF_NEG, -4.0e80, -5.7, -0.047, -2.3e-150, DOUBLE_ZERO_NEG, DOUBLE_ZERO_POS, 2.3e-150, 0.047, 5.7, 4.0e80, DOUBLE_INF_POS};
	} else { // NaN is positive
		data = {DOUBLE_INF_NEG, -4.0e80, -5.7, -0.047, -2.3e-150, DOUBLE_ZERO_NEG, DOUBLE_ZERO_POS, 2.3e-150, 0.047, 5.7, 4.0e80, DOUBLE_INF_POS, DOUBLE_NAN};
	}

	for(size_t i = 0; i < data.size() - 1; ++i) {
		for(size_t j = i + 1; j < data.size(); ++j) {
			REQUIRE_FALSE(data[i] == data[j]);
			REQUIRE(data[i] < data[j]);
			REQUIRE_FALSE(data[i] > data[j]);
			REQUIRE(data[i] <= data[j]);
			REQUIRE_FALSE(data[i] >= data[j]);
		}
	}

}
