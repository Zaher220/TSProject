#ifndef TSUSBDATAREADER_H
#define TSUSBDATAREADER_H

#include <QObject>
#include <stdlib.h>
#include <conio.h>
#include "QLibrary"
#include "Rtusbapi.h"
#include "QDebug"
#include "tscurvebuffer.h"

class TSUsbDataReader : public QObject
{
    Q_OBJECT
public:
    explicit TSUsbDataReader(QObject *parent = 0);
    bool initDevice(TSCurveBuffer *_bf);
    bool closeReader();

    void stopRead();
    void setReadingType(TSUsbReadingType type);
signals:
    void done();
    void changeProgress(int val);
public slots:
    void read();
private:
    void TerminateApplication(char *ErrorString, bool TerminationFlag = false);
    void setLastError(QString);
    SHORT* readData();

    TSCurveBuffer *buffer;
    DWORD DllVersion;
    IRTUSB3000 *pModule; // ��������� �� ��������� ������
    char ModuleName[10]; // �������� ������
    BYTE UsbSpeed; // �������� ������ ���� USB
    char ModuleSerialNumber[9]; // �������� ����� ������
    char AvrVersion[5]; // ������ �������� AVR
    RTUSB3000::DSP_INFO di; // ���������, ���������� ���������� � ������ �������� DSP
    RTUSB3000::FLASH fi; // ��������� ���������� � ���� ������
    RTUSB3000::INPUT_PARS ip; // ��������� ���������� ������ ���
    const static WORD CHANNELS_QUANTITY = 0x4; // ���-�� ������������ ������� ������
    const static double ReadRate = 1.0; // �������  ����� ������
    const static WORD MaxVirtualSoltsQuantity = 4;
    SHORT AdcBuffer[CHANNELS_QUANTITY]; // ����� ������ ��� ����� ��������

    TSUsbReadingType readingType;
    bool ReadingStarted;
};

#endif // TSUSBDATAREADER_H
