/// @file
///	@ingroup 	minexamples
///	@copyright	Copyright 2018 The Min-DevKit Authors. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"

using namespace c74::min;

class manta : public object<manta> {
public:
    MIN_DESCRIPTION	{ "Snyderphonics Manta Object" };
    MIN_TAGS		{ "control" };
    MIN_AUTHOR		{ "Jeff Snyder" };
    MIN_RELATED		{ "hi" };

    inlet<>  input	{ this, "(bang) coolest input" };
    outlet<> output	{ this, "(number) approximate value of pi" };

    message<> bang { this, "bang", "coolest output",
        MIN_FUNCTION {
            output.send(1.f);
            return {};
        }
    };
};

MIN_EXTERNAL(manta);
