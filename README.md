# SRT
[SRT](https://www.haystack.mit.edu/haystack-public-outreach/srt-the-small-radio-telescope-for-education/) fork

## Installation:
Prerequisites:

GTK 2:
```
sudo apt-get install gtk2.0
```
Libmodbus:
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

Example installation commands:
```
cd ~/C/srt/srtnver10/src/
../pswritermake.sh
../srtnmake.sh  2>&1 | tee srtnmake.log
```
## Run:
E.g.:
```
./srtn
```
