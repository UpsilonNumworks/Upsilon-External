#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>

// 0x7D19 for Upsilon, 0xFDA6 for Epsilon
#define PRIMARY_COLOR 0x7D19
#define WEBSITE_URL "https://yaya-cout.github.io/Upsilon-Workshop/calculator?qr=1"
#define FRAME_DURATION 100

struct TransmissionProfile {
  // Size of a QR Code pixel/module on the calculator
  uint8_t pixelSize;

  // Version of the QR Code (determine its size, it's the QR Code resolution)
  uint8_t qrVersion;

  // Error correction code level (0=L/7%,1=M/15%,2=Q/25%,3=H/30%)
  uint8_t qrEcc;

  // Maximum data storable in a QR Code
  // TODO: Compute this automatically
  // This website seems to be a good ressource with its table, with our values
  // being 3 bytes less than what's written there
  // Currently, it was found using dichotomy (trying to scan, if scan failed,
  // then the QR Code isn't valid and value is too big, otherwise it is correct/
  // too low)
  // https://www.nayuki.io/page/creating-a-qr-code-step-by-step
  // Implementations are found here: https://gitlab.com/Nayuki/QR-Code-generator/-/blob/master/rust/src/lib.rs?ref_type=heads (with multiple languages)
  // We aren't using this library for this app.
  uint16_t qrData;

  // Name of the profile displayed in the UI
  char * profileName;
};

struct TransferMetadata {
  /// Protocol version, currently 0, used to keep compatibility with future versions
  uint8_t version;
  /// Firmware version. 0 for Upsilon and 1 for Epsilon
  uint8_t firmware;
  /// The size of the calculator storage in bytes
  uint16_t storageSize;
  // The number of blocks to be transferred, determined from storage size.
  // Used on the receiver side to know when the transfer is finished and show
  // the current progress.
  uint16_t blockCount;

  // Pack to prevent padding
} __attribute__((packed));

void draw_qr(const char * data, struct TransmissionProfile profile);
void extapp_main();

#endif
