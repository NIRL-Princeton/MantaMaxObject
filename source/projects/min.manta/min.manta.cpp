/// @file
///	@ingroup 	minexamples
///	@copyright	Copyright 2018 The Min-DevKit Authors. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <hidapi.h>
#include "Manta.h"
#include "MantaExceptions.h"
#include "MantaMulti.h"
#include <list>
#include <stdexcept>



using namespace c74::min;

struct MantaMultiListEntry
{
  MantaMultiListEntry();
  ~MantaMultiListEntry();

  MantaMulti *mantaServer;
};

class manta : public object<manta>, public MantaClient {
private:
  // initialized first!
  // CRITICAL because other member initialization below relies on this value!
  bool m_initialized { false };
  //MantaServer::LEDState ledStateFromSymbol(const t_symbol *stateSymbol);
  //MantaServer::LEDState ledStateFromInt(int stateSymbol);
  MantaMulti *ConnectedManta;
  bool OneIndexed;
  int inputArg;
  //static void PollConnectedMantas(void *param);
  //static MantaMulti *FindConnectedMantaBySerial(int serialNumber);
  //static void DetachAllMantaFlext(MantaMulti *multi);
  //! Shared list of all connected mantas
  static list<MantaMulti *> ConnectedMantaList;
  static list<manta *> listOfMantaObjects;
  int lastSliderValue[2];
  int pollTimerOn;
  c74::max::t_symbol  *padSymbol;
  c74::max::t_symbol  *buttonSymbol;
   c74::max::t_symbol *sliderSymbol;
   c74::max::t_symbol *sliderMaskSymbol;
   c74::max::t_symbol *amberSymbol;
   c74::max::t_symbol *redSymbol;
   c74::max::t_symbol *offSymbol;
   c74::max::t_symbol *rowSymbol;
   c74::max::t_symbol *columnSymbol;
   c74::max::t_symbol *frameSymbol;
   c74::max::t_symbol *padAndButtonSymbol;
   c74::max::t_symbol *ledsOffSymbol;
    mutex mtx;
public:

  MIN_DESCRIPTION	{ "Snyderphonics Manta Object" };
  MIN_TAGS		{ "control" };
  MIN_AUTHOR		{ "Jeff Snyder and Spencer Russell" };
  MIN_RELATED		{ "hi" };
  //Manta myManta;
void PollConnectedMantas()
  {
    mtx.lock();
    try
    {
      MantaUSB::HandleEvents();
      //cout << "polling " << c74::min::endl;
    }
    catch(MantaCommunicationException e)
    {
      MantaMulti *errorManta = static_cast<MantaMulti *>(e.errorManta);
      cout << "manta: Communication with Manta " << errorManta->GetSerialNumber() << " interrupted" << c74::min::endl;
        delete errorManta;
      DetachAllMantaFlext(errorManta);
      ConnectedMantaList.remove(errorManta);
        if(ConnectedMantaList.empty())
        {
          pollTimerOn = 0;
        }

    }
    mtx.unlock();
  }

  void Detach()
  {
    //cout << "detach called" << c74::min::endl;
    if(Attached())
    {
      cout << "manta: Detaching from manta " << ConnectedManta->GetSerialNumber() << c74::min::endl;
      if(ConnectedManta->GetReferenceCount() == 1)
      {
        /* TODO: if the polling thread is still running here and the callbacks
         * get called for the cancelled USB transfers, there will probably
         * be a segfault */
        cout << "manta: no more connections to manta " << ConnectedManta->GetSerialNumber() << ", destroying." << c74::min::endl;

        ConnectedMantaList.remove(ConnectedManta);
          delete ConnectedManta;
        ConnectedManta = NULL;
          
      }
      else
      {
        /* There are still other MantaFlext instances connected to this MantaMulti,
         * so just detach ourselves */
        ConnectedManta->DetachClient(this);
        ConnectedManta = NULL;
      }
    }
    if(ConnectedMantaList.empty())
    {
      pollTimerOn = 0;
    }
  }

  bool Attached()
  {
    return ConnectedManta != NULL;
  }

  void Attach(int serialNumber){
    if(!Attached())
    {

      MantaMulti *device = FindConnectedMantaBySerial(serialNumber);
      /* see if the device is already in the connected list */
      //cout << "ifnotattached " << ConnectedMantaList.size() << c74::min::endl;
      if(NULL != device)
      {
        cout << "manta: Attaching to manta " << device->GetSerialNumber() << c74::min::endl;
        device->AttachClient(this);
        ConnectedManta = device;
        //cout << "beforepush1 " << ConnectedMantaList.size() << c74::min::endl;
        ConnectedMantaList.push_back(ConnectedManta);
        //cout << "afterpush1 " << ConnectedMantaList.size() << c74::min::endl;
      }
      else {
        /* TODO: open by serial number */
        device = new MantaMulti;
        try
        {
          device->Connect(serialNumber);
          cout << "manta: Connected to manta "<< device->GetSerialNumber() << c74::min::endl;
          device->AttachClient(this);
          device->ResendLEDState();
          ConnectedManta = device;
         // cout << "beforepush2 " << ConnectedMantaList.size() << c74::min::endl;
          ConnectedMantaList.push_back(ConnectedManta);
         // cout << "afterpush2 " << ConnectedMantaList.size() << c74::min::endl;
        }
        catch(MantaNotFoundException e)
        {
          cout << "manta: could not find matching manta"<< c74::min::endl;
          delete device;
        }
        catch(MantaOpenException e)
        {
          cout << "manta: Could not connect to attached Manta" << c74::min::endl;
          delete device;
        }

        if(!ConnectedMantaList.empty())
        {
          pollTimerOn = 1;
          //cout << "got Mantas " << ConnectedMantaList.size() << c74::min::endl;
          metro.delay(0.0);    //communication with manta started
        }
      }

    }
    else
    {
      cout <<"manta: already attached" << c74::min::endl;
    }
  }

  static MantaMulti *FindConnectedMantaBySerial(int serialNumber)
  {
    list<MantaMulti *>::iterator i = ConnectedMantaList.begin();
    while(i != ConnectedMantaList.end())
    {
      if(serialNumber == 0 || (*i)->GetSerialNumber() == serialNumber)
      {
        return *i;
      }
      ++i;
    }
    return NULL;
  }
  static void DetachAllMantaFlext(MantaMulti *multi)
  {
    list<manta *>::iterator i = listOfMantaObjects.begin();
    while(listOfMantaObjects.end() != i)
    {
      if((*i)->ConnectedManta == multi)
      {
        (*i)->ConnectedManta = NULL;
      }
      ++i;
    }
  }


  void PadEvent(int row, int column, int const id, int const value) override
  {
    int const fixedID =  id + (OneIndexed ? 1 : 0);
  pad_and_button_continuous_outlet.send(padSymbol, fixedID, value);
  }

  void ButtonEvent(int const id, int const value) override
  {
    int const fixedID =  id + (OneIndexed ? 1 : 0);
    pad_and_button_continuous_outlet.send(buttonSymbol, fixedID, value);
  }

  void SliderEvent(int const id, int const value) override
  {

    int const fixedID =  id + (OneIndexed ? 1 : 0);
    int const fixedValue1 = (value != 0xFFFF) ? value : lastSliderValue[id];
    int const fixedValue2 =(value != 0xFFFF) ? 1 : 0;

    slider_outlet.send(sliderSymbol, fixedID, fixedValue1, fixedValue2);
    lastSliderValue[id] = value;
  }

  void PadVelocityEvent(int row, int column, int const id, int const value) override
  {

    int const fixedID =  id + (OneIndexed ? 1 : 0);

    pad_velocity_outlet.send(padSymbol, fixedID, value);
  }

  void ButtonVelocityEvent(int const id, int const value) override
  {
    int const fixedID =  id + (OneIndexed ? 1 : 0);

    pad_velocity_outlet.send(buttonSymbol, fixedID, value);
  }

  void FrameEvent(uint8_t *frame) override
  {
    atoms frameCopy;
    frameCopy.reserve(48*sizeof(int));
    frameCopy.insert(frameCopy.end(),&frame[1], &frame[49]);

    frame_outlet(frameCopy);
  }

MantaServer::LEDState const ledStateFromSymbol(const  c74::max::t_symbol *stateSymbol)
{
   if(stateSymbol == amberSymbol)
   {
      return MantaServer::Amber;
   }
   else if(stateSymbol == redSymbol)
   {
       if (ConnectedManta->GetSerialNumber() >= 70)
       {
           return MantaServer::Red;
       }
       else
       {
           return MantaServer::Amber;
       }
   }
   else
   {
      return MantaServer::Off;
   }
}

MantaServer::LEDState ledStateFromInt(int state)
{
   if(state == 1)
   {
      return MantaServer::Amber;
   }
   else if(state == 2)
   {
       if (ConnectedManta->GetSerialNumber() >= 70)
       {
           return MantaServer::Red;
       }
       else
       {
           return MantaServer::Amber;
       }
   }
   else
   {
      return MantaServer::Off;
   }
}


  message<> ledcontrol { this, "ledcontrol", "set LED control",

     MIN_FUNCTION {
       //cout <<"ledcontrol" << c74::min::endl;
       //cout << args[0]<< c74::min::endl;
       //cout << args[1]<< c74::min::endl;
       c74::max::t_symbol* control = from_atoms<c74::max::t_symbol*>(args);
       int state = args[1];
       //cout << control << c74::min::endl;
       //cout << state << c74::min::endl;
       if(Attached())
       {
         if(control == padAndButtonSymbol)
         {
           if(0 == state)
           {
             ConnectedManta->ClearPadAndButtonLEDs();
           }
           ConnectedManta->SetLEDControl(MantaServer::PadAndButton, state);
         }
         else if(control == sliderSymbol)
         {
           ConnectedManta->SetLEDControl(MantaServer::Slider, state);
         }
         else if(control == buttonSymbol)
         {
           if(0 == state)
           {
             ConnectedManta->ClearButtonLEDs();
           }
           ConnectedManta->SetLEDControl(MantaServer::Button, state);
         }
       }
       return {};
     }

  };



  message<> pad { this, "pad", "set pad led",
    MIN_FUNCTION
    {
      if(Attached())
      {
        MantaServer::LEDState parsedState;

        c74::max::t_symbol* control = from_atoms<c74::max::t_symbol*>(args);
        if ((control == amberSymbol) || (control == redSymbol) || (control == offSymbol))
        {
          parsedState = ledStateFromSymbol(control);
        }
        else
        {
          parsedState = ledStateFromInt((args[0]));
        }

        for(int i = 1; i < args.size(); ++i)
        {

            ConnectedManta->SetPadLED(parsedState, (int)args[i]- (OneIndexed ? 1 : 0));

        }
      }
      return {};
    }
  };

  message<> row { this, "row", "set pad row led",
  MIN_FUNCTION
  {
    if(Attached())
    {
      MantaServer::LEDState parsedState;
      c74::max::t_symbol* control = from_atoms<c74::max::t_symbol*>(args);
      if ((control == amberSymbol) || (control == redSymbol) || (control == offSymbol))
      {
        parsedState = ledStateFromSymbol(control);
      }
      else
      {
        parsedState = ledStateFromInt((args[0]));
      }
      ConnectedManta->SetPadLEDRow(parsedState, (int)args[1] - (OneIndexed ? 1 : 0), (int)args[2]);
    }
    return {};
  }
};

    message<> column { this, "column", "set pad column led",
  MIN_FUNCTION
  {
    if(Attached()) {
      MantaServer::LEDState parsedState;
      c74::max::t_symbol* control = from_atoms<c74::max::t_symbol*>(args);
      if ((control == amberSymbol) || (control == redSymbol) || (control == offSymbol))
      {
        parsedState = ledStateFromSymbol(control);
      }
      else
      {
        parsedState = ledStateFromInt((args[0]));
      }

      ConnectedManta->SetPadLEDColumn(parsedState, (int)args[1] - (OneIndexed ? 1 : 0), (int)args[2]);

    }
    return {};
  }
};
  message<> slidermask { this, "slidermask", "set slider led with bitmask",
   MIN_FUNCTION
   {
     if(Attached()) {
       MantaServer::LEDState parsedState;
       c74::max::t_symbol* control = from_atoms<c74::max::t_symbol*>(args);
       if ((control == amberSymbol) || (control == redSymbol) || (control == offSymbol))
       {
         parsedState = ledStateFromSymbol(control);
       }
       else
       {
         parsedState = ledStateFromInt((args[0]));
       }

       ConnectedManta->SetSliderLED(parsedState, (int)args[1]- (OneIndexed ? 1 : 0),(int)args[2]);

     }
     return {};
   }
  };

  message<> sliderled { this, "slider", "set slider led",
     MIN_FUNCTION
     {
       if(Attached()) {
         MantaServer::LEDState parsedState;
         c74::max::t_symbol* control = from_atoms<c74::max::t_symbol*>(args);
         if ((control == amberSymbol) || (control == redSymbol) || (control == offSymbol))
         {
           parsedState = ledStateFromSymbol(control);
         }
         else
         {
           parsedState = ledStateFromInt((args[0]));
         }


         if(parsedState == 0)
         {
            /* turn all LEDs on that slider off */
            ConnectedManta->SetSliderLED(MantaServer::Off, (int)args[1], 0xFF);
         }
         int id = (int)args[1];
         int ledNum = (int)args[2];
         if(OneIndexed)
         {
            id -= 1;
            ledNum -= 1;
         }
         if(ledNum >= 0 && ledNum < 8)
         {
            uint8_t mask = 0x80 >> ledNum;
            ConnectedManta->SetSliderLED(MantaServer::Amber, id, mask);
            ConnectedManta->SetSliderLED(MantaServer::Off, id, ~mask);
         }
         else if(ledNum == -1)
         {
            ConnectedManta->SetSliderLED(MantaServer::Off, id, 0xFF);
         }
       }
       return {};
     }
  };


  message<> buttonled { this, "button", "set button led",
    MIN_FUNCTION
    {
      if(Attached()) {
        MantaServer::LEDState parsedState;
        c74::max::t_symbol* control = from_atoms<c74::max::t_symbol*>(args);
        if ((control == amberSymbol) || (control == redSymbol) || (control == offSymbol))
        {
          parsedState = ledStateFromSymbol(control);
        }
        else
        {
          parsedState = ledStateFromInt((args[0]));
        }

        for(int i = 1; i < args.size(); ++i)
        {
          ConnectedManta->SetButtonLED(parsedState, (int)args[i]- (OneIndexed ? 1 : 0));
        }
      }
      return {};
    }
  };

  message<> recalibrate { this, "reset", "recalibrate manta",
     MIN_FUNCTION
     {
       if(Attached()) {
         ConnectedManta->Recalibrate();
       }
       return {};
     }
  };

  message<> ledsoff { this, "ledsoff", "clear all pad and button leds",
   MIN_FUNCTION
   {
     if(Attached()) {
       ConnectedManta->ClearPadAndButtonLEDs();
     }
     return {};
   }
  };

  message<> setturbomode { this, "turbo", "turn on/off turbo mode (scan only the bottom 16 sensors but much faster)",
 MIN_FUNCTION
 {
   if(Attached())
   {
     ConnectedManta->SetTurboMode((int) args[0]);
   }
   return {};
 }
  };

  message<> setrawmode { this, "raw", "turn on/off raw mode (output raw sensor values, without removing baseline values)",
MIN_FUNCTION
{
  if(Attached())
  {
    ConnectedManta->SetRawMode((int) args[0]);
  }
  return {};
}
  };

  message<> setOneIndexed { this, "oneindex", "turn on/off one indexed mode (first sensor is 1 instead of 0)",
 MIN_FUNCTION
 {

     OneIndexed = (int) args[0];

   return {};
 }
  };


  manta(const atoms& args = {}):ConnectedManta(NULL),OneIndexed(false), pollTimerOn(0)
  {
  padSymbol = c74::max::gensym("pad");
  sliderSymbol = c74::max::gensym("slider");
  sliderMaskSymbol = c74::max::gensym("slidermask");
  buttonSymbol = c74::max::gensym("button");
  amberSymbol = c74::max::gensym("amber");
  offSymbol = c74::max::gensym("off");
  redSymbol = c74::max::gensym("red");
  rowSymbol = c74::max::gensym("row");
  columnSymbol = c74::max::gensym("column");
  frameSymbol = c74::max::gensym("frame");
  padAndButtonSymbol = c74::max::gensym("padandbutton");
  ledsOffSymbol = c74::max::gensym("ledsoff");
    inputArg = args[0];
    listOfMantaObjects.push_back(this);
    //cout << "trying Serial Number" << inputArg << c74::min::endl;
//    if(!args.empty())
//    {
//      Attach(inputArg);
//
//    }
//    else
//    {
//      Attach(0);
//    }

    //myManta.Connect();
    // initialized, we can switch our flag used to prevent unsafe access
    // in the attribute setters (below)

    m_initialized = true;

  }

  ~manta() override {
    if (m_initialized)
    {
      Detach();
      listOfMantaObjects.remove(this);
    }
  }

  inlet<>  input	{ this, "input" };
  outlet<> pad_velocity_outlet	{ this, "(list) pad and button note on/off with velocities)" };
  outlet<> pad_and_button_continuous_outlet	{ this, "(list) pad and button continuous values" };
  outlet<> slider_outlet	{ this, "(list) slider values" };
  outlet<> frame_outlet	{ this, "(list) processed data output" };


  timer<> metro { this,
        MIN_FUNCTION {
          if (pollTimerOn) {

            //cout << "hi" << c74::min::endl;
            PollConnectedMantas();
            metro.delay(2.0);
          }
          return {};
        }
  };
  argument<number> greeting_arg { this, "serial number", "Manta serial number to look for (if you are using multiple mantas)",
          MIN_ARGUMENT_FUNCTION {
            inputArg = arg;
          }
    };
  message<> bang { this, "bang", "coolest output",


    MIN_FUNCTION {
      int worked =1;
      pad_velocity_outlet.send(pollTimerOn);
      if (ConnectedManta != NULL)
      {
        pad_and_button_continuous_outlet.send(ConnectedManta->IsConnected());
        slider_outlet.send(ConnectedManta->GetSerialNumber());
      }
      // output.send(worked);
      return {};
    }

};

  message<> connect { this, "connect", "connect to manta",

    MIN_FUNCTION {
      Attach(inputArg);

      return {};
    }

  };


};


list<MantaMulti *> manta::ConnectedMantaList;
list<manta *> manta::listOfMantaObjects;

MIN_EXTERNAL(manta);
