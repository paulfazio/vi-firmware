#include "serialutil.h"
#include "buffers.h"
#include "log.h"
#include "WProgram.h"

// TODO see if we can do this with interrupts on the chipKIT
// http://www.chipkit.org/forum/viewtopic.php?f=7&t=1088
void readFromSerial(SerialDevice* serial, bool (*callback)(uint8_t*)) {
    int bytesAvailable = ((HardwareSerial*)serial->device)->available();
    if(bytesAvailable > 0) {
        for(int i = 0; i < bytesAvailable && !QUEUE_FULL(uint8_t,
								&serial->receiveQueue); i++) {
            char byte = ((HardwareSerial*)serial->device)->read();
            QUEUE_PUSH(uint8_t, &serial->receiveQueue, (uint8_t) byte);
        }
        processQueue(&serial->receiveQueue, callback);
    }
}

void initializeSerial(SerialDevice* serial) {
	serial->device = &Serial1;
    ((HardwareSerial*)serial->device)->begin(115200);
    QUEUE_INIT(uint8_t, &serial->receiveQueue);
    QUEUE_INIT(uint8_t, &serial->sendQueue);
}

// The chipKIT version of this function is blocking. It will entirely flush the
// send queue before returning.
void processSerialSendQueue(SerialDevice* device) {
    int byteCount = 0;
    char sendBuffer[MAX_MESSAGE_SIZE];
    while(!QUEUE_EMPTY(uint8_t, &device->sendQueue) &&
					byteCount < MAX_MESSAGE_SIZE) {
        sendBuffer[byteCount++] = QUEUE_POP(uint8_t, &device->sendQueue);
    }

    ((HardwareSerial*)device->device)->write((const uint8_t*)sendBuffer,
			byteCount);
}
