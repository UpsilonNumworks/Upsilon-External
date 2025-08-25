#include "main.h"
#include "menu.h"
#include "qrcode.h"

#include <extapp_api.h>
#include <stdint.h>
#include <string.h>

void draw_qr(const char * data, struct TransmissionProfile profile) {
  // QR Code generator based on https://github.com/ricmoo/QRCode/blob/master/src/qrcode.c
  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(profile.qrVersion)];

  // Generate QRCode
  qrcode_initBytes(&qrcode, qrcodeData, profile.qrVersion, profile.qrEcc, (uint8_t*)data, profile.qrData);

  // Display the QRCode
  extapp_waitForVBlank();
  extapp_pushRectUniform(0, 0, LCD_WIDTH, LCD_HEIGHT, 0x0000);
  for (uint8_t y = 0; y < qrcode.size; y++) {
    // TODO: Bufferize to improve performance
    for (uint8_t x = 0; x < qrcode.size; x++) {
      extapp_pushRectUniform(x * profile.pixelSize, y * profile.pixelSize, profile.pixelSize, profile.pixelSize, qrcode_getModule(&qrcode, x, y) ? 0xFFFF : 0x0000);
    }
  }

  // Refresh the screen on the simulator
  #ifndef DEVICE
  extapp_scanKeyboard();
  #endif
}


const uint32_t * userlandAddress() {
  // On Upsilon, we are using N0110 only, so no model detection
  return (uint32_t *)0x20000008;
}

uint32_t get_storage_address() {
  return *(uint32_t *)((*userlandAddress()) + 0xC);
}

const uint32_t get_storage_size() {
  return *(uint32_t *)((*userlandAddress()) + 0x10);
}


void extapp_main() {
  // Find base storage address and size.
  // #ifndef DEVICE
  #ifndef false
  // On simulator, we can simply query the internal API
  // I don't want to use C++ and make symbols public, so I just use
  // mangled C++ symbols directly
  extern void * _ZNK12PlatformInfo15storage_addressEv();
  const char * storage_address = _ZNK12PlatformInfo15storage_addressEv();
  // TODO: Avoid hardcoding size.
  const size_t storage_size = 59992;
  #else
  // TODO: Check if this code is working
  const uint32_t * storage_address = (uint32_t *)get_storage_address();
  const size_t storage_size = get_storage_size();
  #endif

  struct TransmissionProfile profile = select_profile();

  // Exit if a null profile is returned
  if (profile.pixelSize == 0 && profile.qrVersion == 0 && profile.qrEcc == 0 &&
    profile.qrData == 0 && profile.profileName == 0) {
      return;
  }

  // const char * filecontent_to_write = "\0test\1\2\0@é…d0\255\1";
  // extapp_fileWrite("binary.sav", filecontent_to_write, 18, EXTAPP_RAM_FILE_SYSTEM);

  const struct TransferMetadata metadata = {
    .version = 0,
    .firmware = 0,
    // We don't actually need to compute the total transfer size, but only
    // storage size (to be shown to the user) and block count (to show
    // progression)
    .storageSize = storage_size,
    // Block count is storage_size / data_per_block without overhead
    .blockCount = storage_size / (profile.qrData - sizeof(uint16_t) - sizeof(struct TransferMetadata)),
  };

  // Start displaying as soon before the user released the launch key, but don't
  // exit on keydown until they released the keyboard first
  bool keyboardWasReleased = !extapp_scanKeyboard();
  bool running = true;

  while (running) {
    // Now, do the actual transfer
    uint16_t blockId = 0;
    uint16_t storageIndex = 0;
    while (blockId <= metadata.blockCount) {
      if (extapp_scanKeyboard()) {
        if (keyboardWasReleased) {
          running = false;
          break;
        }
      } else {
        keyboardWasReleased = true;
      }

      uint64_t start = extapp_millis();
      uint16_t currentIndex = 0;

      // Warning: indexed on 16 bits, so transferBuffer[1] means the third byte of
      // data.
      uint16_t transferBuffer[profile.qrData];
      memset(transferBuffer, 0x00, sizeof(transferBuffer));
      transferBuffer[currentIndex] = blockId;
      currentIndex++;

      // All blocks contains metadata so the transfer can be started from any
      // block with progress. It does add some overhead (3 bytes per block, 1.3%
      // of overhead for a 220 byte block)
      memcpy(transferBuffer + currentIndex, &metadata, sizeof(metadata));
      currentIndex += sizeof(metadata) / sizeof(uint16_t);

      // Fill rest of buffer with storage
      // TODO: Avoid reading out of bounds on last block
      memcpy(transferBuffer + currentIndex, storage_address + storageIndex, profile.qrData - currentIndex);
      storageIndex += profile.qrData - currentIndex * sizeof(uint16_t);


      draw_qr((const char *)transferBuffer, profile);

      blockId++;
      uint64_t duration = extapp_millis() - start;
      uint32_t timeToSleep = FRAME_DURATION - duration;
      // Prevent UINT32_MAX sleep time when duration is higher than 100 ms
      if (timeToSleep > FRAME_DURATION) {
        timeToSleep = 0;
      }
      extapp_msleep(timeToSleep);
    }
  }
}
