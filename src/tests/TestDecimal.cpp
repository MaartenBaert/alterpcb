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
#include "Decimal.h"
#include "MiscMath.h"

#include "catch.hpp"

#include <random>

TEST_CASE("Binary to decimal", "[decimal]") {

	Decimal a = ToDecimal(12.34, 8);
	REQUIRE(a.type == FLOATTYPE_NORMAL);
	REQUIRE(a.negative == false);
	REQUIRE(a.expo == -6);
	REQUIRE(a.mant == 12340000);

	Decimal b = ToDecimal(-1.234e50, 15);
	REQUIRE(b.type == FLOATTYPE_NORMAL);
	REQUIRE(b.negative == true);
	REQUIRE(b.expo == 36);
	REQUIRE(b.mant == UINT64_C(123400000000000));

	Decimal c = ToDecimal(0.001234, 3);
	REQUIRE(c.type == FLOATTYPE_NORMAL);
	REQUIRE(c.negative == false);
	REQUIRE(c.expo == -5);
	REQUIRE(c.mant == 123);

	Decimal d = ToDecimal(999999.0, 6);
	REQUIRE(d.type == FLOATTYPE_NORMAL);
	REQUIRE(d.negative == false);
	REQUIRE(d.expo == 0);
	REQUIRE(d.mant == 999999);

	Decimal e = ToDecimal(999999.0, 5);
	REQUIRE(e.type == FLOATTYPE_NORMAL);
	REQUIRE(e.negative == false);
	REQUIRE(e.expo == 2);
	REQUIRE(e.mant == 10000);

}

TEST_CASE("Decimal to binary", "[decimal]") {

	REQUIRE(FromDecimal(Decimal{FLOATTYPE_NORMAL, false, 0, 1234}) == 1234.0e0);
	REQUIRE(FromDecimal(Decimal{FLOATTYPE_NORMAL, false, 10, 1234}) == 1234.0e10);
	REQUIRE(FromDecimal(Decimal{FLOATTYPE_NORMAL, false, -30, 1234}) == 1234.0e-30);
	REQUIRE(FromDecimal(Decimal{FLOATTYPE_NORMAL, false, -50, 1234}) == 1234.0e-50);

	REQUIRE(FromDecimal(Decimal{FLOATTYPE_NORMAL, true, 0, 999999}) == -999999.0e0);
	REQUIRE(FromDecimal(Decimal{FLOATTYPE_NORMAL, true, 10, 999999}) == -999999.0e10);
	REQUIRE(FromDecimal(Decimal{FLOATTYPE_NORMAL, true, -30, 999999}) == -999999.0e-30);
	REQUIRE(FromDecimal(Decimal{FLOATTYPE_NORMAL, true, -50, 999999}) == -999999.0e-50);

	REQUIRE(FromDecimal(Decimal{FLOATTYPE_NORMAL, false, 0, 87654321}) == 87654321.0e0);
	REQUIRE(FromDecimal(Decimal{FLOATTYPE_NORMAL, false, 10, 87654321}) == 87654321.0e10);
	REQUIRE(FromDecimal(Decimal{FLOATTYPE_NORMAL, false, -30, 87654321}) == 87654321.0e-30);
	REQUIRE(FromDecimal(Decimal{FLOATTYPE_NORMAL, false, -50, 87654321}) == 87654321.0e-50);

	REQUIRE(FromDecimal(Decimal{FLOATTYPE_NORMAL, false, 308, 1}) == 1.0e308);
	REQUIRE(FromDecimal(Decimal{FLOATTYPE_NORMAL, false, -308, 5}) == 5.0e-308);

	// subnormals
	REQUIRE(FromDecimal(Decimal{FLOATTYPE_NORMAL, false, -310, 7}) == 7.0e-310);
	REQUIRE(FromDecimal(Decimal{FLOATTYPE_NORMAL, false, -320, 7}) == 7.0e-320);
	REQUIRE(FromDecimal(Decimal{FLOATTYPE_NORMAL, false, -324, 7}) == 7.0e-324);

	// underflows
	REQUIRE(FromDecimal(Decimal{FLOATTYPE_NORMAL, false, -324, 1}) == 0.0);
	REQUIRE(FromDecimal(Decimal{FLOATTYPE_NORMAL, false, -350, 1}) == 0.0);
	REQUIRE(FromDecimal(Decimal{FLOATTYPE_NORMAL, false, INT32_MIN, 1}) == 0.0);

	// overflows
	REQUIRE(std::isinf(FromDecimal(Decimal{FLOATTYPE_NORMAL, false, 308, 2})));
	REQUIRE(std::isinf(FromDecimal(Decimal{FLOATTYPE_NORMAL, false, 350, 1})));
	REQUIRE(std::isinf(FromDecimal(Decimal{FLOATTYPE_NORMAL, false, INT32_MAX, 1})));

}

TEST_CASE("Decimal round-trip", "[decimal]") {

	// generate random doubles, with zero and subnormals, but without inf/nan
	std::mt19937_64 rng(963258741);
	std::uniform_int_distribution<uint64_t> dist(0x0000000000000000, 0x7fefffffffffffff);

	for(size_t i = 0; i < 100000; ++i) {
		uint64_t val0 = dist(rng) | ((rng() & 1) << 63);
		double val1 = MemCast<double>(val0);
		Decimal temp = ToDecimal(val1, 17);
		double val2 = FromDecimal(temp);
		if(val1 != val2) { // for speed
			REQUIRE(val1 == val2);
		}
	}

	for(size_t i = 0; i < 100000; ++i) {
		uint64_t val0 = dist(rng) | ((rng() & 1) << 63);
		val0 |= UINT64_C(0x0000ffffffffff00); // this greatly increases the probability of hitting edge cases
		double val1 = MemCast<double>(val0);
		Decimal temp = ToDecimal(val1, 17);
		double val2 = FromDecimal(temp);
		if(val1 != val2) { // for speed
			REQUIRE(val1 == val2);
		}
	}

	for(size_t i = 0; i < 100000; ++i) {
		uint64_t val0 = dist(rng) | ((rng() & 1) << 63);
		val0 &= UINT64_C(0xffff0000000000ff); // this greatly increases the probability of hitting edge cases
		double val1 = MemCast<double>(val0);
		Decimal temp = ToDecimal(val1, 17);
		double val2 = FromDecimal(temp);
		if(val1 != val2) { // for speed
			REQUIRE(val1 == val2);
		}
	}

}

TEST_CASE("Decimal reverse round-trip", "[decimal]") {

	// generate random decimal numbers that map to the normal range of double values
	std::mt19937_64 rng(963258741);
	std::uniform_int_distribution<int64_t> dist_expo(-307-14, 307-14);
	std::uniform_int_distribution<uint64_t> dist_mant(100000000000000, 999999999999999);

	for(size_t i = 0; i < 100000; ++i) {
		Decimal val1;
		val1.type = FLOATTYPE_NORMAL;
		val1.negative = false;
		val1.expo = dist_expo(rng);
		val1.mant = dist_mant(rng);
		double temp = FromDecimal(val1);
		Decimal val2 = ToDecimal(temp, 15);
		if(val1.type != val2.type || val1.negative != val2.negative || val1.expo != val2.expo || val1.mant != val2.mant) { // for speed
			REQUIRE(val1.type == val2.type);
			REQUIRE(val1.negative == val2.negative);
			REQUIRE(val1.expo == val2.expo);
			REQUIRE(val1.mant == val2.mant);
		}
	}

}
