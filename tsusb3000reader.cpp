#include "tsusb3000reader.h"

TSUsb3000Reader::TSUsb3000Reader(QObject *parent) :
    QObject(parent)
{
}
bool TSUsb3000Reader::initDevice(TSCurveBuffer *_bf){
    buffer=_bf;
    typedef DWORD(*GetDllVersion)();
    typedef LPVOID(*CreateInstance)(PCHAR const);
    QLibrary myLib("dll/Rtusbapi.dll");
    myLib.load();

    GetDllVersion RtGetDllVersion = (GetDllVersion) myLib.resolve("RtGetDllVersion");
    if (RtGetDllVersion) {
        if (RtGetDllVersion() == CURRENT_VERSION_RTUSBAPI){
            qDebug() << "Dll version is correct";
        }
        else{
            qDebug() << "Dll version isn`t correct";
            this->setLastError("Dll version isn`t correct");
            return false;
        }
    }

    CreateInstance RtCreateInstance = (CreateInstance) myLib.resolve("RtCreateInstance");
    if (RtCreateInstance) {
        pModule = static_cast<IRTUSB3000 *> (RtCreateInstance("usb3000"));
        qDebug() << "Create usb3000 instance";
    }
    else{
        qDebug() << "Can`t create usb3000 instance";
        this->setLastError("Can`t create usb3000 instance");
        return false;
    }

    WORD i;
    // �������� ������ ������������ ���������� Rtusbapi.dll
    if ((DllVersion = RtGetDllVersion()) != CURRENT_VERSION_RTUSBAPI) {
        char String[128];
        sprintf(String, " Rtusbapi.dll Version Error!!!\n   Current: %1u.%1u. Required: %1u.%1u",
                DllVersion >> 0x10, DllVersion & 0xFFFF,
                CURRENT_VERSION_RTUSBAPI >> 0x10, CURRENT_VERSION_RTUSBAPI & 0xFFFF);

        TerminateApplication(String);
    } else printf(" Rtusbapi.dll Version --> OK\n");

    // ������� ��������� �� ��������� ������ USB3000
    pModule = static_cast<IRTUSB3000 *> (RtCreateInstance("usb3000"));
    if (!pModule) TerminateApplication(" Module Interface --> Bad\n");
    else printf(" Module Interface --> OK\n");

    // ��������� ���������� ������ USB3000 � ������ 127 ����������� ������
    for (i = 0x0; i < MaxVirtualSoltsQuantity; i++) if (pModule->OpenDevice(i)) break;
    // ���-������ ����������?
    if (i == MaxVirtualSoltsQuantity) TerminateApplication(" Can't find module USB3000 in first 127 virtual slots!\n");
    else printf(" OpenDevice(%u) --> OK\n", i);

    // ��������� �������� ������������� ������
    if (!pModule->GetModuleName(ModuleName)) TerminateApplication(" GetModuleName() --> Bad\n");
    else printf(" GetModuleName() --> OK\n");

    // ��������, ��� ��� 'USB3000'
    if (strcmp(ModuleName, "USB3000")) TerminateApplication(" The module is not 'USB3000'\n");
    else printf(" The module is 'USB3000'\n");

    // ������ ������� �������� ������ ���� USB20
    if (!pModule->GetUsbSpeed(&UsbSpeed)) {
        printf(" GetUsbSpeed() --> Bad\n");
        exit(1);
    } else printf(" GetUsbSpeed() --> OK\n");
    // ������ ��������� ������ �������� AVR
    printf(" USB Speed is %s\n", UsbSpeed ? "HIGH (480 Mbit/s)" : "FULL (12 Mbit/s)");

    // ��������� �������� ����� ������
    if (!pModule->GetModuleSerialNumber(ModuleSerialNumber)) TerminateApplication(" GetModuleSerialNumber() --> Bad\n");
    else printf(" GetModuleSerialNumber() --> OK\n");
    // ������ ��������� �������� ����� ������
    printf(" Module Serial Number is %s\n", ModuleSerialNumber);

    // ��������� ������ �������� AVR
    if (!pModule->GetAvrVersion(AvrVersion)) TerminateApplication(" GetAvrVersion() --> Bad\n");
    else printf(" GetAvrVersion() --> OK\n");
    // ������ ��������� ������ �������� AVR
    printf(" Avr Driver Version is %s\n", AvrVersion);

    // ��� �������� DSP ������ �� ���������������� ������� ������� DLL ����������
    if (!pModule->LOAD_DSP()) TerminateApplication(" LOAD_DSP() --> Bad\n");
    else printf(" LOAD_DSP() --> OK\n");

    // �������� �������� ������
    if (!pModule->MODULE_TEST()) TerminateApplication(" MODULE_TEST() --> Bad\n");
    else printf(" MODULE_TEST() --> OK\n");

    // ������� ������ ������������ �������� DSP
    if (!pModule->GET_DSP_INFO(&di)) TerminateApplication(" GET_DSP_INFO() --> Bad\n");
    else printf(" GET_DSP_INFO() --> OK\n");
    // ������ ��������� ������ ������������ �������� DSP
    printf(" DSP Driver version is %1u.%1u\n", di.DspMajor, di.DspMinor);

    // ����������� ����������������� ���� size ��������� RTUSB3000::FLASH
    fi.size = sizeof (RTUSB3000::FLASH);
    // ������� ���������� �� ���� ������
    if (!pModule->GET_FLASH(&fi)) TerminateApplication(" GET_FLASH() --> Bad\n");
    else printf(" GET_FLASH() --> OK\n");

    // ����������� ����������������� ���� size ��������� RTUSB3000::INPUT_PARS
    ip.size = sizeof (RTUSB3000::INPUT_PARS);
    // ������� ������� ��������� ������ ���
    if (!pModule->GET_INPUT_PARS(&ip)) TerminateApplication(" GET_INPUT_PARS() --> Bad\n");
    else printf(" GET_INPUT_PARS() --> OK\n");

    // ��������� �������� ��������� ����� ������
    ip.CorrectionEnabled = true; // �������� ������������� ������
    ip.InputClockSource = RTUSB3000::INTERNAL_INPUT_CLOCK; // ����� ������������ ���������� �������� �������� ��� ����� ������
    //	ip.InputClockSource = RTUSB3000::EXTERNAL_INPUT_CLOCK;	// ����� ������������ ������� �������� �������� ��� ����� ������
    ip.SynchroType = RTUSB3000::NO_SYNCHRO; // �� ����� ������������ ������� ������������� ��� ����� ������
    //	ip.SynchroType = RTUSB3000::TTL_START_SYNCHRO;	// ����� ������������ �������� ������������� ������ ��� ����� ������
    ip.ChannelsQuantity = CHANNELS_QUANTITY; // ������ �������� ������
    for (i = 0x0; i < CHANNELS_QUANTITY; i++) ip.ControlTable[i] = (WORD) (i);
    ip.InputRate = ReadRate; // ������� ������ ��� � ���
    ip.InterKadrDelay = 0.0;
    // ����� ������������ ��������� ������������� ������������, ������� ��������� � ���� ������
    for (i = 0x0; i < 8; i++) {
        ip.AdcOffsetCoef[i] = fi.AdcOffsetCoef[i];
        ip.AdcScaleCoef[i] = fi.AdcScaleCoef[i];
    }
    // ��������� ��������� ��������� ������ ����� ������ � ������� DSP ������
    if (!pModule->SET_INPUT_PARS(&ip)) TerminateApplication(" SET_INPUT_PARS() --> Bad\n");
    else printf(" SET_INPUT_PARS() --> OK\n");

    // ��������� ��������� ������ �� ������ ��������
    printf(" \n");
    printf(" Module USB3000 (S/N %s) is ready ... \n", fi.SerialNumber);
    printf(" Adc parameters:\n");
    printf("   InputClockSource is %s\n", ip.InputClockSource ? "EXTERNAL" : "INTERNAL");
    printf("   SynchroType is %s\n", ip.SynchroType ? "TTL_START_SYNCHRO" : "NO_SYNCHRO");
    printf("   ChannelsQuantity = %2d\n", ip.ChannelsQuantity);
    printf("   AdcRate = %8.3f kHz\n", ip.InputRate);
    //printf("   InterKadrDelay = %2.4f ms\n", ip.InterKadrDelay);
    printf("   ChannelRate = %8.3f kHz\n", ip.ChannelRate);
    printf("\n Press any key to terminate this program...\n");
    // ���� ������������� ���������� ������� ADC_KADR �
    // ����������� ���������� ������ �� ������ ������

    //emit ������ � ��� ��� ����� ������
}
void TSUsb3000Reader::setLastError(QString last_error){
    this->LastError=last_error;
}
bool TSUsb3000Reader::readData(){
    if (pModule->READ_KADR(AdcBuffer)) {
        buffer->append(AdcBuffer[0]/100,AdcBuffer[1]/200,AdcBuffer[2]/200);
        return true;
    }
    else
    {
        this->setLastError("Can`t read from device");
        return false;
    }
}
void TSUsb3000Reader::TerminateApplication(char *ErrorString, bool TerminationFlag) {
    // ��������� ��������� ������
    if (pModule) {
        // ��������� ��������� ������
        if (!pModule->ReleaseInstance()) printf(" ReleaseInstance() --> Bad\n");
        else printf(" ReleaseInstance() --> OK\n");
        // ������� ��������� �� ��������� ������
        pModule = NULL;
    }

    // ������� ����� ���������
    if (ErrorString) printf(ErrorString);

    // ���� ����� - �������� ��������� ���������
    if (TerminationFlag) exit(1);
    else return;
}