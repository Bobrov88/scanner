
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/libhidapi-hidraw.so.0 - внесение LIBRARY в LIBRARY_PATH

https://winitpro.ru/index.php/2022/09/13/windows-probros-usb-ustrojstv-wsl-linux/ - для определени USB-портов в Linux
rpm --import https://www.elrepo.org/RPM-GPG-KEY-elrepo.org
rpm -ivh http://www.elrepo.org/elrepo-release-7.0-3.el7.elrepo.noarch.rpm


usbipd list // list of hid-devices
usbipd bind --busid 1-10   // 1-10 busid for sharind device

scp -P 5022 bobrov@ubuntu.farafonov.pro:~/project/scanner/build/bin/scanner /home/

dpkg-reconfigure console-setup
Choose UTF-8 > Latin, Cyrillic/Non Cyrillic > Terminus Bold > 8x14 > OK