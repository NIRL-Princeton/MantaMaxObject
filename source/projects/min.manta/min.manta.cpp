/// @file
///	@ingroup 	minexamples
///	@copyright	Copyright 2018 The Min-DevKit Authors. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <hidapi.h>
#include "Manta.h"
#include "MantaExceptions.h"
#include "MantaMulti.h"


using namespace c74::min;


class manta : public object<manta> {
private:
    // initialized first!
  	// CRITICAL because other member initialization below relies on this value!
	bool m_initialized { false };
public:
    MIN_DESCRIPTION	{ "Snyderphonics Manta Object" };
    MIN_TAGS		{ "control" };
    MIN_AUTHOR		{ "Jeff Snyder" };
    MIN_RELATED		{ "hi" };
    Manta myManta;
    manta(const atoms& args = {})
    {

        myManta.Connect();
        cout << "my dudes, the connection is:" << myManta.IsConnected() << c74::min::endl;
         // initialized, we can switch our flag used to prevent unsafe access
        // in the attribute setters (below)

		m_initialized = true;



    }
  inlet<>  input	{ this, "(bang) coolest input" };
  outlet<> output	{ this, "(number) approximate value of pi" };
    timer<> m_timer { this,
    MIN_FUNCTION {
      output.send(5);

      /*try
      {
        MantaUSB::HandleEvents();
      }
      catch(MantaCommunicationException e) {
        MantaMulti *errorManta = static_cast<MantaMulti *>(e.errorManta);
        cout << "manta: Communication with Manta" << errorManta->GetSerialNumber() << "interrupted" << c74::min::endl;


        delete errorManta;
        //DetachAllManta(errorManta);
        //ConnectedMantaList.remove(errorManta);
      }
      */
      m_timer.delay(1);

      return{};
      }
      };


    message<> bang { this, "bang", "coolest output",
        MIN_FUNCTION {
            int res;
            hid_device *handle;
            res = hid_init();
            int worked = 1;
            handle = hid_open(0x2424, 0x2424, NULL);
            if (!handle) {
		        worked = 0;
		        hid_exit();
 		    }

            output.send(worked);
            return {};
        }
    };
};

MIN_EXTERNAL(manta);
