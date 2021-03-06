

#include <stdio.h>
#include "qextserialport.h"

/*!
Default constructor.  Note that the name of the device used by a QextSerialPort constructed with
this constructor will be determined by #defined constants, or lack thereof - the default behavior
is the same as _TTY_LINUX_.  Possible naming conventions and their associated constants are:

\verbatim

Constant         Used By         Naming Convention
----------       -------------   ------------------------
Q_OS_WIN        Windows         COM1, COM2
_TTY_IRIX_       SGI/IRIX        /dev/ttyf1, /dev/ttyf2
_TTY_HPUX_       HP-UX           /dev/tty1p0, /dev/tty2p0
_TTY_SUN_        SunOS/Solaris   /dev/ttya, /dev/ttyb
_TTY_DIGITAL_    Digital UNIX    /dev/tty01, /dev/tty02
_TTY_FREEBSD_    FreeBSD         /dev/ttyd0, /dev/ttyd1
_TTY_OPENBSD_    OpenBSD         /dev/tty00, /dev/tty01
_TTY_LINUX_      Linux           /dev/ttyS0, /dev/ttyS1
<none>           Linux           /dev/ttyS0, /dev/ttyS1
\endverbatim

This constructor assigns the device name to the name of the first port on the specified system.
See the other constructors if you need to open a different port.
*/
QextSerialPort::QextSerialPort(QextSerialPort::QueryMode mode)
 : QIODevice()
{

#ifdef Q_OS_WIN
    setPortName("COM1");

#elif defined(_TTY_IRIX_)
    setPortName("/dev/ttyf1");

#elif defined(_TTY_HPUX_)
    setPortName("/dev/tty1p0");

#elif defined(_TTY_SUN_)
    setPortName("/dev/ttya");

#elif defined(_TTY_DIGITAL_)
    setPortName("/dev/tty01");

#elif defined(_TTY_FREEBSD_)
    setPortName("/dev/ttyd1");

#elif defined(_TTY_OPENBSD_)
    setPortName("/dev/tty00");

#else
    setPortName("/dev/ttyS0");
#endif

    construct();
    setQueryMode(mode);
    platformSpecificInit();
}

/*!
Constructs a serial port attached to the port specified by name.
name is the name of the device, which is windowsystem-specific,
e.g."COM1" or "/dev/ttyS0".
*/
QextSerialPort::QextSerialPort(const QString & name, QextSerialPort::QueryMode mode)
    : QIODevice()
{
    construct();
    setQueryMode(mode);
    setPortName(name);
    platformSpecificInit();
}

/*!
Constructs a port with default name and specified settings.
*/
QextSerialPort::QextSerialPort(const PortSettings& settings, QextSerialPort::QueryMode mode)
    : QIODevice()
{
    construct();
    setBaudRate(settings.BaudRate);
    setDataBits(settings.DataBits);
    setParity(settings.Parity);
    setStopBits(settings.StopBits);
    setFlowControl(settings.FlowControl);
    setTimeout(settings.Timeout_Millisec);
    setQueryMode(mode);
    platformSpecificInit();
}

/*!
Constructs a port with specified name and settings.
*/
QextSerialPort::QextSerialPort(const QString & name, const PortSettings& settings, QextSerialPort::QueryMode mode)
    : QIODevice()
{
    construct();
    setPortName(name);
    setBaudRate(settings.BaudRate);
    setDataBits(settings.DataBits);
    setParity(settings.Parity);
    setStopBits(settings.StopBits);
    setFlowControl(settings.FlowControl);
    setTimeout(settings.Timeout_Millisec);
    setQueryMode(mode);
    platformSpecificInit();
}

/*!
Common constructor function for setting up default port settings.
(115200 Baud, 8N1, Hardware flow control where supported, otherwise no flow control, and 0 ms timeout).
*/
void QextSerialPort::construct()
{
    lastErr = E_NO_ERROR;
    Settings.BaudRate=BAUD115200;
    Settings.DataBits=DATA_8;
    Settings.Parity=PAR_NONE;
    Settings.StopBits=STOP_1;
    Settings.FlowControl=FLOW_HARDWARE;
    Settings.Timeout_Millisec=500;
    mutex = new QMutex( QMutex::Recursive );
    setOpenMode(QIODevice::NotOpen);
}

void QextSerialPort::setQueryMode(QueryMode mechanism)
{
    _queryMode = mechanism;
}

/*!
Sets the name of the device associated with the object, e.g. "COM1", or "/dev/ttyS0".
*/
void QextSerialPort::setPortName(const QString & name)
{
    #ifdef Q_OS_WIN
    port = fullPortNameWin( name );
    #else
    port = name;
    #endif
}

/*!
Returns the name set by setPortName().
*/
QString QextSerialPort::portName() const
{
    return port;
}

/*!
  Reads all available data from the device, and returns it as a QByteArray.
  This function has no way of reporting errors; returning an empty QByteArray()
  can mean either that no data was currently available for reading, or that an error occurred.
*/
QByteArray QextSerialPort::readAll()
{
    int avail = this->bytesAvailable();
    return (avail > 0) ? this->read(avail) : QByteArray();
}

/*!
Returns the baud rate of the serial port.  For a list of possible return values see
the definition of the enum BaudRateType.
*/
BaudRateType QextSerialPort::baudRate(void) const
{
    return Settings.BaudRate;
}

/*!
Returns the baud rate of the serial port in bps.  
*/
qint32 QextSerialPort::baudRateBps(void) const
{
    BaudRateType baudrate = baudRate();
    switch (baudrate) {
        case BAUD50:
             return 50;
        case BAUD75:
             return 75;
        case BAUD110:
             return 110;
        case BAUD134:
             return 134;
        case BAUD150:
             return 150;
        case BAUD200:
             return 200;
        case BAUD300:
             return 300;
        case BAUD600:
             return 600;
        case BAUD1200:
             return 1200;
        case BAUD1800:
             return 1800;
        case BAUD2400:
             return 2400;
        case BAUD4800:
             return 4800;
        case BAUD9600:
             return 9600;
        case BAUD14400:
             return 4400;
        case BAUD19200:
             return 19200;
        case BAUD38400:
             return 38400;
        case BAUD56000:
             return 56000;
        case BAUD57600:
             return 57600;
        case BAUD76800:
             return 76800;
        case BAUD115200:
             return 115200;
        case BAUD128000:
             return 128000;
        case BAUD256000:
             return 256000;
        case BAUD460800:
             return 460800;
        case BAUD500000:
             return 500000;
        case BAUD576000:
             return 576000;
    }
    return -1;
}

/*!
Returns the number of data bits used by the port.  For a list of possible values returned by
this function, see the definition of the enum DataBitsType.
*/
DataBitsType QextSerialPort::dataBits() const
{
    return Settings.DataBits;
}

/*!
Returns the type of parity used by the port.  For a list of possible values returned by
this function, see the definition of the enum ParityType.
*/
ParityType QextSerialPort::parity() const
{
    return Settings.Parity;
}

/*!
Returns the number of stop bits used by the port.  For a list of possible return values, see
the definition of the enum StopBitsType.
*/
StopBitsType QextSerialPort::stopBits() const
{
    return Settings.StopBits;
}

/*!
Returns the type of flow control used by the port.  For a list of possible values returned
by this function, see the definition of the enum FlowType.
*/
FlowType QextSerialPort::flowControl() const
{
    return Settings.FlowControl;
}

/*!
Returns true if device is sequential, otherwise returns false. Serial port is sequential device
so this function always returns true. Check QIODevice::isSequential() documentation for more
information.
*/
bool QextSerialPort::isSequential() const
{
    return true;
}

QString QextSerialPort::errorString()
{
    switch(lastErr)
    {
        case E_NO_ERROR: return "No Error has occurred";
        case E_INVALID_FD: return "Invalid file descriptor (port was not opened correctly)";
        case E_NO_MEMORY: return "Unable to allocate memory tables (POSIX)";
        case E_CAUGHT_NON_BLOCKED_SIGNAL: return "Caught a non-blocked signal (POSIX)";
        case E_PORT_TIMEOUT: return "Operation timed out (POSIX)";
        case E_INVALID_DEVICE: return "The file opened by the port is not a valid device";
        case E_BREAK_CONDITION: return "The port detected a break condition";
        case E_FRAMING_ERROR: return "The port detected a framing error (usually caused by incorrect baud rate settings)";
        case E_IO_ERROR: return "There was an I/O error while communicating with the port";
        case E_BUFFER_OVERRUN: return "Character buffer overrun";
        case E_RECEIVE_OVERFLOW: return "Receive buffer overflow";
        case E_RECEIVE_PARITY_ERROR: return "The port detected a parity error in the received data";
        case E_TRANSMIT_OVERFLOW: return "Transmit buffer overflow";
        case E_READ_FAILED: return "General read operation failure";
        case E_WRITE_FAILED: return "General write operation failure";
        case E_FILE_NOT_FOUND: return "The "+this->portName()+" file doesn't exists";
        default: return QString("Unknown error: %1").arg(lastErr);
    }
}

/*!
Standard destructor.
*/
QextSerialPort::~QextSerialPort()
{
    if (isOpen()) {
        close();
    }
    platformSpecificDestruct();
    delete mutex;
}
