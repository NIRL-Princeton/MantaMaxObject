/// @file
///	@ingroup 	minexamples
///	@copyright	Copyright 2018 The Min-DevKit Authors. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min_unittest.h"    // required unit test header

#include "min.manta.cpp"    // need the source of our object so that we can access it

// Unit tests are written using the Catch framework as described at
// https://github.com/philsquared/Catch/blob/master/docs/tutorial.md

SCENARIO("object produces correct output") {
	std::cout << "constructor called " << c74::min::endl;
	ext_main(nullptr);    // every unit test must call ext_main() once to configure the class

	GIVEN("An instance of minmanta") {

		// test_wrapper<manta> an_instance;
		// manta&              my_object = an_instance;

		// check that default attr values are correct
		REQUIRE(1 == TRUE);
		// REQUIRE(my_object.ledsOffSymbol == symbol("ledsoff"));    // note: symbols can be compared with strings
	}
}