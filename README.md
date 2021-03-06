# Psensor

Psensor is a graphical hardware monitoring application for Linux.

It draws realtime charts and raises alerts about:

 * the temperature of the motherboard and CPU sensors (using
   lm-sensors).
 * the temperature of the NVidia GPUs (using XNVCtrl).
 * the temperature of ATI GPUs (using ATI ADL SDK).
 * the temperature of the Hard Disk Drives (using hddtemp, libatasmart
   or udisks2).
 * the rotation speed of the fans.
 * the temperature of a remote computer.
 * the CPU load.

Alerts are using Desktop Notification and a specific GTK+ status icon.

For Ubuntu users, Psensor is providing an Application Indicator which turns
red under alert and a menu for quickly check all sensors.

It is possible to monitor remotely a computer:

 * start `psensor-server` on the remote computer.

 * start `psensor` using: `psensor --url=http://hostname:3131`.

`psensor-server` is a minimal Web server, a Web browser can be used to
monitor the remote computer by opening the URL:
`http://hostname:3131`.

**WARNING**
`psensor-server` does not provide any way to restrict the connection
to the HTTP server, worst, no effort has been made against malicious
HTTP attacks.  You should make the +psensor-server+ port available
only to a network or computer you trust by using the usual network
security tools of the system (for example:
http://www.netfilter.org/projects/iptables/index.html).

## Installation Using Distribution Packages

### Debian

For Debian, the binary and source packages are available in the
Wheezy, unstable and testing repositories.

Debian package information is available at:
http://packages.qa.debian.org/p/psensor.html.

To do the installation:
```
sudo apt-get install psensor psensor-server
```

### Ubuntu

Since Ubuntu Oneiric (11.10), psensor is available in the universe
repository of Ubuntu.

To install +psensor+ or +psensor-server+:
```
sudo apt-get install psensor
sudo apt-get install psensor-server
```

Or search them in the Ubuntu Software Center.

## Installation From Source Archive

`psensor` and `psensor-server` can be compiled and installed on any
modern GNU/Linux distribution.

### Compilation Prerequisites

The compilation of `psensor` requires:

 * make
 * gcc (or llvm)
 * lm-sensors
 * library sensors4
 * library gtk3 >=3.4
 * help2man (optional, required to produce the manpage)
 * asciidoctor (optional, required to produce the html version of the
   faq)
 * cppcheck (optional, static source checker)
 * library libnotify (optional)
 * library libappindicator >= 3.0 (optional)
 * library libXNVCtrl (optional)
 * library json-c >= 0.11.99 and curl (optional, required for remote monitoring)
 * library unity (>=v3.4.2, optional)
 * library gtop2 (optional, required for CPU usage)
 * library atasmart (optional, for disk monitoring)
 * library udisk2 (optional, for disk monitoring)

The specific requirements for +psensor-server+:

 * library libmicrohttpd
 * library json-c >= 0.11.99

### Compilation and Installation Steps

1. download the source archive from: http://wpitchoune.net/psensor/files
2. Extract files from the source archive
3. Compilation: ``./configure;make clean all`
4. Installation:  +make install+ (require to have root permission)
5. Start the sensor detection script: +sensors-detect+ and follows the
instructions. Reboot or activate the kernel modules which have
been found.
6. Run: `psensor`

### ATI/AMD GPU Support

When the OpenSource ATI driver is used, the monitoring
information is available throw lm-sensors and does not require the
following instructions.

When the proprietary ATI driver(Catalyst) is used, the build is
requiring an additional library.

Download the
http://developer.amd.com/sdks/ADLSDK/Pages/default.aspx[ATI ADL SDK]
and extracts the files of the archive into a directory [dir].

Replace the step 3. of the above section by +./configure
--with-libatiadl=[dir]+ where +[dir]+ must be the directory where you
have extracted the ATI ADL SDK. Other steps are indentical.

## Contact

Bugs and comments can be sent to jeanfi@gmail.com.

Home page: http://wpitchoune.net/psensor
