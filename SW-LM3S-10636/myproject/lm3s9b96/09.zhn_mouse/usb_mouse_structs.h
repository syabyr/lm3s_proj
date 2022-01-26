#ifndef _USB_MOUSE_STRUCTS_H_
#define _USB_MOUSE_STRUCTS_H_

extern unsigned long MouseHandler(void *pvCBData,
                                  unsigned long ulEvent,
                                  unsigned long ulMsgData,
                                  void *pvMsgData);

extern const tUSBDHIDMouseDevice g_sMouseDevice;

#endif
