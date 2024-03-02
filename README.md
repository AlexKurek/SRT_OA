[SRT](https://www.haystack.mit.edu/haystack-public-outreach/srt-the-small-radio-telescope-for-education/) fork

## Installation:
### Prerequisites:

```
sudo apt-get install gtk2.0 libusb-1.0-0-dev libfftw3-dev
```
Libmodbus:
```
sudo apt-get install -y libmodbus-dev
```
or [Libmodbus](https://libmodbus.org/) manually:
```
export INSTALLDIR=/opt/
mkdir -p ${INSTALLDIR}/libmodbus/build/
cd ${INSTALLDIR}/libmodbus/build/
git clone git://github.com/stephane/libmodbus src/
cd src/
./autogen.sh
./configure --enable-silent-rules --without-documentation --prefix=${INSTALLDIR}/libmodbus/
make -j 4
sudo make install
cd ../../
rm -rf ${INSTALLDIR}/libmodbus/build/
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${INSTALLDIR}/libmodbus/lib/
```

### Example compilation commands:
```
cd ~/C/srt/srtnver10/
chmod +x pswritermake.sh srtnmake.sh
cd src/
../pswritermake.sh
../srtnmake.sh  2>&1 | tee srtnmake.log
```
## Run:
E.g.:
```
./srtn
```
