#include "DumlLitePlugin.h"

#include <QtDebug>

#include "Helpers.h"

// Uncomment to enable debugging of this class
// #define DUML_LITE_FF_DEBUG

#ifdef DUML_LITE_FF_DEBUG
   #define dlDebug qDebug
   #define dlWarning qWarning
#else
   #define dlDebug if(0) qDebug
   #define dlWarning if(0) qWarning
#endif

#define REJECT_INVALID_CRC true

const uint8_t DumlLiteFrameFactory::theCrc8Table[256] = {
   0x00, 0x5E, 0xBC, 0xE2, 0x61, 0x3F, 0xDD, 0x83,
   0xC2, 0x9C, 0x7E, 0x20, 0xA3, 0xFD, 0x1F, 0x41,
   0x9D, 0xC3, 0x21, 0x7F, 0xFC, 0xA2, 0x40, 0x1E,
   0x5F, 0x01, 0xE3, 0xBD, 0x3E, 0x60, 0x82, 0xDC,
   0x23, 0x7D, 0x9F, 0xC1, 0x42, 0x1C, 0xFE, 0xA0,
   0xE1, 0xBF, 0x5D, 0x03, 0x80, 0xDE, 0x3C, 0x62,
   0xBE, 0xE0, 0x02, 0x5C, 0xDF, 0x81, 0x63, 0x3D,
   0x7C, 0x22, 0xC0, 0x9E, 0x1D, 0x43, 0xA1, 0xFF,
   0x46, 0x18, 0xFA, 0xA4, 0x27, 0x79, 0x9B, 0xC5,
   0x84, 0xDA, 0x38, 0x66, 0xE5, 0xBB, 0x59, 0x07,
   0xDB, 0x85, 0x67, 0x39, 0xBA, 0xE4, 0x06, 0x58,
   0x19, 0x47, 0xA5, 0xFB, 0x78, 0x26, 0xC4, 0x9A,
   0x65, 0x3B, 0xD9, 0x87, 0x04, 0x5A, 0xB8, 0xE6,
   0xA7, 0xF9, 0x1B, 0x45, 0xC6, 0x98, 0x7A, 0x24,
   0xF8, 0xA6, 0x44, 0x1A, 0x99, 0xC7, 0x25, 0x7B,
   0x3A, 0x64, 0x86, 0xD8, 0x5B, 0x05, 0xE7, 0xB9,
   0x8C, 0xD2, 0x30, 0x6E, 0xED, 0xB3, 0x51, 0x0F,
   0x4E, 0x10, 0xF2, 0xAC, 0x2F, 0x71, 0x93, 0xCD,
   0x11, 0x4F, 0xAD, 0xF3, 0x70, 0x2E, 0xCC, 0x92,
   0xD3, 0x8D, 0x6F, 0x31, 0xB2, 0xEC, 0x0E, 0x50,
   0xAF, 0xF1, 0x13, 0x4D, 0xCE, 0x90, 0x72, 0x2C,
   0x6D, 0x33, 0xD1, 0x8F, 0x0C, 0x52, 0xB0, 0xEE, 
   0x32, 0x6C, 0x8E, 0xD0, 0x53, 0x0D, 0xEF, 0xB1,
   0xF0, 0xAE, 0x4C, 0x12, 0x91, 0xCF, 0x2D, 0x73,
   0xCA, 0x94, 0x76, 0x28, 0xAB, 0xF5, 0x17, 0x49,
   0x08, 0x56, 0xB4, 0xEA, 0x69, 0x37, 0xD5, 0x8B,
   0x57, 0x09, 0xEB, 0xB5, 0x36, 0x68, 0x8A, 0xD4,
   0x95, 0xCB, 0x29, 0x77, 0xF4, 0xAA, 0x48, 0x16,
   0xE9, 0xB7, 0x55, 0x0B, 0x88, 0xD6, 0x34, 0x6A,
   0x2B, 0x75, 0x97, 0xC9, 0x4A, 0x14, 0xF6, 0xA8,
   0x74, 0x2A, 0xC8, 0x96, 0x15, 0x4B, 0xA9, 0xF7,
   0xB6, 0xE8, 0x0A, 0x54, 0xD7, 0x89, 0x6B, 0x35
};

const uint16_t DumlLiteFrameFactory::theCrc16Table[256] = {
   0x0000, 0x1189, 0x2312, 0x329B, 0x4624, 0x57AD, 0x6536, 0x74BF,
   0x8C48, 0x9DC1, 0xAF5A, 0xBED3, 0xCA6C, 0xDBE5, 0xE97E, 0xF8F7,
   0x1081, 0x0108, 0x3393, 0x221A, 0x56A5, 0x472C, 0x75B7, 0x643E,
   0x9CC9, 0x8D40, 0xBFDB, 0xAE52, 0xDAED, 0xCB64, 0xF9FF, 0xE876,
   0x2102, 0x308B, 0x0210, 0x1399, 0x6726, 0x76AF, 0x4434, 0x55BD,
   0xAD4A, 0xBCC3, 0x8E58, 0x9FD1, 0xEB6E, 0xFAE7, 0xC87C, 0xD9F5,
   0x3183, 0x200A, 0x1291, 0x0318, 0x77A7, 0x662E, 0x54B5, 0x453C,
   0xBDCB, 0xAC42, 0x9ED9, 0x8F50, 0xFBEF, 0xEA66, 0xD8FD, 0xC974,
   0x4204, 0x538D, 0x6116, 0x709F, 0x0420, 0x15A9, 0x2732, 0x36BB,
   0xCE4C, 0xDFC5, 0xED5E, 0xFCD7, 0x8868, 0x99E1, 0xAB7A, 0xBAF3,
   0x5285, 0x430C, 0x7197, 0x601E, 0x14A1, 0x0528, 0x37B3, 0x263A,
   0xDECD, 0xCF44, 0xFDDF, 0xEC56, 0x98E9, 0x8960, 0xBBFB, 0xAA72,
   0x6306, 0x728F, 0x4014, 0x519D, 0x2522, 0x34AB, 0x0630, 0x17B9,
   0xEF4E, 0xFEC7, 0xCC5C, 0xDDD5, 0xA96A, 0xB8E3, 0x8A78, 0x9BF1,
   0x7387, 0x620E, 0x5095, 0x411C, 0x35A3, 0x242A, 0x16B1, 0x0738,
   0xFFCF, 0xEE46, 0xDCDD, 0xCD54, 0xB9EB, 0xA862, 0x9AF9, 0x8B70,
   0x8408, 0x9581, 0xA71A, 0xB693, 0xC22C, 0xD3A5, 0xE13E, 0xF0B7,
   0x0840, 0x19C9, 0x2B52, 0x3ADB, 0x4E64, 0x5FED, 0x6D76, 0x7CFF,
   0x9489, 0x8500, 0xB79B, 0xA612, 0xD2AD, 0xC324, 0xF1BF, 0xE036,
   0x18C1, 0x0948, 0x3BD3, 0x2A5A, 0x5EE5, 0x4F6C, 0x7DF7, 0x6C7E,
   0xA50A, 0xB483, 0x8618, 0x9791, 0xE32E, 0xF2A7, 0xC03C, 0xD1B5,
   0x2942, 0x38CB, 0x0A50, 0x1BD9, 0x6F66, 0x7EEF, 0x4C74, 0x5DFD,
   0xB58B, 0xA402, 0x9699, 0x8710, 0xF3AF, 0xE226, 0xD0BD, 0xC134,
   0x39C3, 0x284A, 0x1AD1, 0x0B58, 0x7FE7, 0x6E6E, 0x5CF5, 0x4D7C,
   0xC60C, 0xD785, 0xE51E, 0xF497, 0x8028, 0x91A1, 0xA33A, 0xB2B3,
   0x4A44, 0x5BCD, 0x6956, 0x78DF, 0x0C60, 0x1DE9, 0x2F72, 0x3EFB,
   0xD68D, 0xC704, 0xF59F, 0xE416, 0x90A9, 0x8120, 0xB3BB, 0xA232,
   0x5AC5, 0x4B4C, 0x79D7, 0x685E, 0x1CE1, 0x0D68, 0x3FF3, 0x2E7A,
   0xE70E, 0xF687, 0xC41C, 0xD595, 0xA12A, 0xB0A3, 0x8238, 0x93B1,
   0x6B46, 0x7ACF, 0x4854, 0x59DD, 0x2D62, 0x3CEB, 0x0E70, 0x1FF9,
   0xF78F, 0xE606, 0xD49D, 0xC514, 0xB1AB, 0xA022, 0x92B9, 0x8330,
   0x7BC7, 0x6A4E, 0x58D5, 0x495C, 0x3DE3, 0x2C6A, 0x1EF1, 0x0F78
};

DumlLiteFrameFactory::DumlLiteFrameFactory():
   theFrameCount(0),
   theTotalBytesReceived(0),
   theTotalBytesFramed(0),
   theCrc8Rejections(0),
   theCrc16Rejections(0),
   theFrameLengthRejections(0)
{
   dlDebug() << "Duml Lite frame factory created";
}

DumlLiteFrameFactory::~DumlLiteFrameFactory()
{
   dlDebug() << "Duml Lite frame factory deleted";
}

/**
 *
 * DUML Framing Format
 * Byte 0 = Start of Frame = 0x55
 * Byte 1/2 = DUML Version and Message Length.
 *   Bits 0xFC00 = DUML version
 *   Bits 0x03FF = Length (max 1024 bytes) (Number of bytes including SOF and ending checksum)
 * Byte 3 = crc8 (of bytes 0-2)
 *
 *   Bytes 4 - N-2 = DUML Framed Data
 *
 * Bytes N-2/N-1 = crc16
 *
 * DUML Payload bytes
 *
 * Byte 4 = Source and index
 *   Bits 0xE0 = source index
 *   Bits 0x1F = source
 * Byte 5 = Target and index
 *   Bits 0xE0 = target index
 *   Bits 0x1F = target
 * Bytes 6/7 = Sequence Number
 *
 * Byte 8 = Flags
 * Byte 9 = Command Set Number:
 * Byte 10 = Command ID
 */
void DumlLiteFrameFactory::pushMsgBytes(QByteArray msgData)
{
   theTotalBytesReceived += msgData.length();
   theDataBytes.append(msgData);

   dlDebug() << __PRETTY_FUNCTION__ << " received " << msgData.length() << "bytes of data";
   // dlDebug() << "WHOLE RAW: " << Helpers::qbyteToHexString(msgData);

   findFrames();

}

void DumlLiteFrameFactory::findFrames()
{
   while(findNextFrame())
   {
      theFrameCount++;
   }
}

bool DumlLiteFrameFactory::findNextFrame()
{
   // Find the start of frame
   RESTART_SOF_SEARCH:
   int startOfFrame = -1;
   for(int i = 0; i < theDataBytes.length(); i++)
   {
      // Search for the start of frame marker
      if (theDataBytes.at(i) == 0x55)
      {
         // We found the start of frame marker
         startOfFrame = i;
         break;
      }

      if (i > 512)
      {
         // If we haven't found SOF yet, discard the chunk of data
         theDataBytes = theDataBytes.mid(512);
         i = 0;
      }
   }

   if (startOfFrame == -1)
   {
      // Could not find any start of frame marker, flushing the data
      dlDebug() << "Discarding all received data: " << theDataBytes.length() << " bytes";
      theDataBytes.clear();
      return false;
   }

   if (startOfFrame != 0)
   {
      // There was some unframed data on the beginning of the buffer, discard
      dlDebug() << "Discarding received data before SOF: " << startOfFrame << " bytes";
      theDataBytes.remove(0, startOfFrame);
   }

   // At this point, the first byte of data in the frame is SOF
   if (theDataBytes.length() < 13)
   {
      dlDebug() << "We have the beginning of the frame, but it is not complete yet, only "
                << theDataBytes.length() << " bytes received so far";
      return false;
   }

   // Verify the CRC-8 field
   if (REJECT_INVALID_CRC)
   {
      uint8_t calculatedCrc = calcCrc8( (uint8_t*) theDataBytes.data(), 3);
      // dlDebug() << "Calc CRC = " << (int) calculatedCrc << ", framed CRC = " << (int) theDataBytes[3];

      if ( (int) calculatedCrc != (int) theDataBytes[3])
      {
         // Reject the SOF byte
         dlDebug() << "Calc CRC = " << (int) calculatedCrc << " does not match framed CRC = "
                   << (int) theDataBytes[3];

         theCrc8Rejections++;
         theDataBytes = theDataBytes.mid(1);
         goto RESTART_SOF_SEARCH;
      }
   }

   int dumlFrameLengthField = ( theDataBytes[2] << 8) | theDataBytes[1];
   int dumlFrameLength = dumlFrameLengthField & 0x03ff;

   if (dumlFrameLength < 13)
   {
      dlDebug() << "DUML Length Invalid: " << dumlFrameLength;
      theFrameLengthRejections++;
      theDataBytes = theDataBytes.mid(1);
      goto RESTART_SOF_SEARCH;
   }

//   if (dumlFrameLength < 13)
//   {
//      dlDebug() << "Begining of frame found, but total frame length too short";
//      return false;
//   }

   if (dumlFrameLength > theDataBytes.length())
   {
      dlDebug() << "We have DUML Frame Header, DUML length = " << dumlFrameLength
                << ", but we only have " << theDataBytes.length() << " bytes in buffer";
      return false;
   }

   dlDebug() << "DUML Frame Length = " << dumlFrameLength;

   // Check CRC-16
   if (REJECT_INVALID_CRC)
   {
      uint16_t calculatedCrc16 = calcCrc16( (uint8_t*) theDataBytes.data(), dumlFrameLength - 2);
      QByteArray crc16BytesFromFrame = theDataBytes.mid(dumlFrameLength - 2, 2);
      uint16_t* crcFromFrame = (uint16_t*) crc16BytesFromFrame.data();
      bool crc16Match = (*crcFromFrame == calculatedCrc16);

      QString matchResult = (crc16Match ? "MATCH" : "DOES NOT MATCH");

      // dlDebug() << "Calc CRC16 = " << QString::number((int) calculatedCrc16, 16) << matchResult
      //           << "Frame CRC16 = " << Helpers::qbyteToHexString(crc16BytesFromFrame);

      if (!crc16Match)
      {
         // Reject the SOF byte
         dlDebug() << "Calc CRC16 = " << QString::number((int) calculatedCrc16, 16) << matchResult
                   << "Frame CRC16 = " << Helpers::qbyteToHexString(crc16BytesFromFrame);

         theCrc16Rejections++;
         theDataBytes = theDataBytes.mid(1);
         goto RESTART_SOF_SEARCH;
      }
   }

   QByteArray framedData = theDataBytes.left(dumlFrameLength);
   theDataBytes.remove(0, dumlFrameLength);

   dlDebug() << "RAW: " << Helpers::qbyteToHexString(framedData);

   DumlFrame* df = new DumlFrame(framedData);
   theFrames.append(df);

   theTotalBytesFramed += dumlFrameLength;

   return true;
}

uint8_t DumlLiteFrameFactory::calcCrc8(uint8_t* buffer, int length)
{
   uint8_t retVal = 0x77;
   for(int i = 0; i < length; i++)
   {
      retVal = theCrc8Table[buffer[i] ^ retVal];
   }

   return retVal;
}

uint16_t DumlLiteFrameFactory::calcCrc16(uint8_t* buffer, int length)
{
   uint16_t retVal = 0x3692;
   for(int i = 0; i < length; i++)
   {
      uint8_t lowerFeedback = (retVal >> 8);
      uint8_t upperFeedback = (retVal & 0xff);

      retVal = theCrc16Table[buffer[i] ^ upperFeedback] ^ lowerFeedback;
   }

   return retVal;
}

bool DumlLiteFrameFactory::isFrameReady()
{
   dlDebug() << __PRETTY_FUNCTION__ << ( theFrames.empty() ? ": NO" : ": YES");
   return !theFrames.empty();

}

DataFrame* DumlLiteFrameFactory::getNextFrame()
{
   dlDebug() << __PRETTY_FUNCTION__ << " called";

   if (isFrameReady())
   {
      DataFrame* retVal = theFrames.front();
      theFrames.pop_front();
      return retVal;
   }
   else
   {
      dlWarning() << "Asked for a frame, but we have none!";
      return nullptr;
   }
}

QString DumlLiteFrameFactory::statusToString()
{
   QString retVal;
   retVal += "BytesRxed=";
   retVal += QString::number(theTotalBytesReceived);
   retVal += ", BytesFramed=";
   retVal += QString::number(theTotalBytesFramed);
   retVal += ", FrameCount=";
   retVal += QString::number(theFrameCount);
   retVal += ", BytesBuffered=";
   retVal += QString::number(theDataBytes.length());
   retVal += ", BytesDiscarded=";
   retVal += QString::number(theTotalBytesReceived - theTotalBytesFramed - theDataBytes.length());
   retVal += ", Crc8Rejections=";
   retVal += QString::number(theCrc8Rejections);
   retVal += ", Crc16Rejections=";
   retVal += QString::number(theCrc16Rejections);
   retVal += ", FrameLenRejections=";
   retVal += QString::number(theFrameLengthRejections);

   return retVal;
}

QString DumlLiteFrameFactory::protocolName()
{
   return "DUML Lite";
}
