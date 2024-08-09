#ifdef _CMOC_VERSION_

//#include <peekpoke.h>
#include <Joystick.h>
#include "joystick.h"

unsigned char readJoystick() {
  unsigned char result = 0;
  unsigned char btns;

  Joystick_selectAnalogMuxInputs(2);  // left joystick horizontal axis
  if (!Joystick_compareAxis(7))
  {
    result |= JOY_LEFT_MASK;
  }
  else if (Joystick_compareAxis(56))
  {
    result |= JOY_RIGHT_MASK;
  }  

  Joystick_selectAnalogMuxInputs(3);  // right joystick horizontal axis
  if (!Joystick_compareAxis(7))
  {
    result |= JOY_UP_MASK;
  }
  else if (Joystick_compareAxis(56))
  {
    result |= JOY_DOWN_MASK;
  }  
  btns = Joystick_getButtonStates();
  if (0==(btns & 0x4)) 
  {
    result |= JOY_BTN_1_MASK;
  }

  return result; 
}

#endif
