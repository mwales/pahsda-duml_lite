#ifndef DUML_LITE_FRAME_FACTORY_H
#define DUML_LITE_FRAME_FACTORY_H

#include <QObject>
#include <QList>

#include "DataFrameFactoryInterface.h"
#include "DumlFrame.h"
/**
 * Plugin for PAHSDA
 *
 * Decodes data stream into DUML Frames
 */
class DumlLiteFrameFactory: public QObject, public DataFrameFactoryInterface
{
   Q_OBJECT
   Q_PLUGIN_METADATA(IID DataFactoryInterface_iid)
   Q_INTERFACES(DataFrameFactoryInterface)

public:
   DumlLiteFrameFactory();

   virtual ~DumlLiteFrameFactory();

   virtual void pushMsgBytes(QByteArray msgData);

   virtual bool isFrameReady();

   virtual DataFrame* getNextFrame();

   virtual QString statusToString();

   virtual QString protocolName();

protected:

   static const uint8_t theCrc8Table[256];

   static const uint16_t theCrc16Table[256];

   void findFrames();

   bool findNextFrame();

   uint8_t calcCrc8(uint8_t* buffer, int length);

   uint16_t calcCrc16(uint8_t* buffer, int length);

   QByteArray theDataBytes;

   int theFrameCount;

   int theTotalBytesReceived;

   int theTotalBytesFramed;

   int theCrc8Rejections;

   int theCrc16Rejections;

   int theFrameLengthRejections;

   QList<DumlFrame*> theFrames;
};

#endif // DUML_LITE_FRAME_FACTORY_H
