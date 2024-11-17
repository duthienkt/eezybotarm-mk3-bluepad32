#include <Bluepad32.h>
//https://bluepad32.readthedocs.io/en/latest/supported_gamepads/
ControllerPtr myControllers[BP32_MAX_GAMEPADS];

#define FRAME_LIMIT 60

// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl) {
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
      // Additionally, you can get certain gamepad properties like:
      // Model, VID, PID, BTAddr, flags, etc.
      ControllerProperties properties = ctl->getProperties();
      Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id,
                    properties.product_id);
      myControllers[i] = ctl;
      foundEmptySlot = true;
      break;
    }
  }
  if (!foundEmptySlot) {
    Serial.println("CALLBACK: Controller connected, but could not found empty slot");
  }
}

void onDisconnectedController(ControllerPtr ctl) {
  bool foundController = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
      myControllers[i] = nullptr;
      foundController = true;
      break;
    }
  }

  if (!foundController) {
    Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
  }
}

void dumpGamepad(ControllerPtr ctl) {
  Serial.printf(
    "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
    "misc: 0x%02x, gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d\n",
    ctl->index(),        // Controller Index
    ctl->dpad(),         // D-pad
    ctl->buttons(),      // bitmask of pressed buttons
    ctl->axisX(),        // (-511 - 512) left X Axis
    ctl->axisY(),        // (-511 - 512) left Y axis
    ctl->axisRX(),       // (-511 - 512) right X axis
    ctl->axisRY(),       // (-511 - 512) right Y axis
    ctl->brake(),        // (0 - 1023): brake button
    ctl->throttle(),     // (0 - 1023): throttle (AKA gas) button
    ctl->miscButtons(),  // bitmask of pressed "misc" buttons
    ctl->gyroX(),        // Gyro X
    ctl->gyroY(),        // Gyro Y
    ctl->gyroZ(),        // Gyro Z
    ctl->accelX(),       // Accelerometer X
    ctl->accelY(),       // Accelerometer Y
    ctl->accelZ()        // Accelerometer Z
  );
}

/*********** GLOBAL VARIABLES *****************/
uint16_t buttons = 0;
uint16_t bake = 0;
uint16_t axisLX = 0;
uint16_t axisLY = 0;
uint16_t axisRX = 0;
uint16_t axisRY = 0;


/******* USER TEMPLATE ***************/
void onButtonPress(uint16_t bt) {
  Serial.printf("Press button %d\n", bt);
}



void onButtonRelease(uint16_t bt) {
  Serial.printf("Release button %d\n", bt);
}

//bake: 0-1023
void onBrakeChange(int32_t bake) {
  Serial.printf("Bake change %d\n", bake);
}

//x, y : (-511 - 512)
void onLeftAxisChange(int32_t x, int32_t y) {
  Serial.printf("Left axis  change %d %d\n", x, y);
}

//x, y : (-511 - 512)
void onRightAxisChange(int32_t x, int32_t y) {
  Serial.printf("Right axis change %d %d\n", x, y);
}



void update(long deltaTime) {
}



/*************************** SYSTEM *******************************************/

//detect level 0 event
void gamepadLoop() {
  uint16_t newButtons;
  uint32_t newBake;

  uint32_t newAxisLX, newAxisLY, newAxisRX, newAxisRY;


  if (!BP32.update()) return;
  bool found = false;
  for (auto ctl : myControllers) {
    if (found) break;
    if (!ctl) continue;
    if (!ctl->isConnected()) continue;
    if (!ctl->hasData()) continue;
    if (!ctl->isGamepad()) continue;
    found = true;
    newButtons = ctl->buttons();
    for (int i = 0; i < 6; ++i) {
      if ((newButtons & (1 << i))) {
        if (!(buttons & (1 << i))) {
          onButtonPress(i);
        }
      } else {
        if (buttons & (1 << i)) {
          onButtonRelease(i);
        }
      }
    }
    newBake = ctl->brake();
    if (newBake != bake) {
      onBrakeChange(newBake);
    }

    newAxisLX = ctl->axisX();
    newAxisLY = ctl->axisY();
    newAxisRX = ctl->axisRX();
    newAxisRY = ctl->axisRY();
    if (axisLX != newAxisLX || axisLY != newAxisLY) {
      onLeftAxisChange(newAxisLX, newAxisLY);
    }

    if (axisRX != newAxisRX || axisRY != newAxisRY) {
      onRightAxisChange(newAxisRX, newAxisRY);
    }

    //TODO: detect more event

    buttons = newButtons;
    bake = newBake;

    axisLX = newAxisLX;
    axisLY = newAxisLY;
    axisRX = newAxisRX;
    axisRY = newAxisRY;


    // dumpGamepad(ctl);
  }
  yield();
}


// Arduino setup function. Runs in CPU 1
void setup() {
  Serial.begin(115200);
  Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
  const uint8_t* addr = BP32.localBdAddress();
  Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

  // Setup the Bluepad32 callbacks
  BP32.setup(&onConnectedController, &onDisconnectedController);

  // "forgetBluetoothKeys()" should be called when the user performs
  // a "device factory reset", or similar.
  // Calling "forgetBluetoothKeys" in setup() just as an example.
  // Forgetting Bluetooth keys prevents "paired" gamepads to reconnect.
  // But it might also fix some connection / re-connection issues.
  BP32.forgetBluetoothKeys();

  // Enables mouse / touchpad support for gamepads that support them.
  // When enabled, controllers like DualSense and DualShock4 generate two connected devices:
  // - First one: the gamepad
  // - Second one, which is a "virtual device", is a mouse.
  // By default, it is disabled.
  BP32.enableVirtualDevice(false);
}


long lastExecTime = 0;
void loop() {
  long startTime = millis();
  gamepadLoop();

  long nextTime = startTime + 1000 / FRAME_LIMIT;

  update(max(1000l / FRAME_LIMIT, lastExecTime));
  long endTime = millis();
  lastExecTime = endTime - startTime;
  long paddingTime = nextTime - endTime;
  if (paddingTime <= 0) paddingTime = 1;  //overflow or low frame
  vTaskDelay(paddingTime);
}
