#!/bin/bash
CFLAGS=`pkg-config gtk+-2.0 --cflags`
LIBS=`pkg-config gtk+-2.0 --libs`
# vspectra.c dongle + amd FFT
# vspectra_pci.c SDAS-4020 + amd FFT
# vspectra_fftw.c dongle + fftW for intel
# vspectra_pci_fftw.c SDAS-4020 + fftW for intel
# vspectra_four.c dongle + c-coded FFT (which is slower)

#Warning_FLAGS="-Wall -Wextra -Wpedantic -Wformat=2 -Wno-unused-parameter -Wshadow -Wwrite-strings -Wstrict-prototypes -Wold-style-definition -Wredundant-decls -Wnested-externs -Wmissing-include-dirs -Wjump-misses-init -Wlogical-op"
Warning_FLAGS="-Wall -Wextra"

# All_FLAGS="${Warning_FLAGS} -Og -m64 -march=native $CFLAGS" # for debugging
All_FLAGS="${Warning_FLAGS} -O3 -m64 -march=native $CFLAGS"
# All_FLAGS="${Warning_FLAGS} -O3 -m32 -march=native $CFLAGS"  # milkyway jest 32-bitowy

# compile for dongle + amd FFT
#gcc ${All_FLAGS} main.c vspectra.c disp.c plot.c cat.c geom.c time.c outfile.c sport.c map.c cmdfl.c cal.c srthelp.c velspec.c four.c amdfft.c librtlsdr.c tuner_r820t.c $LIBS -lm -lacml /usr/lib/libgfortran.a `pkg-config --libs --cflags libusb-1.0` -I/opt/libmodbus/include/modbus/ -L/opt/libmodbus/lib/ -l modbus

# -g for debugger (not updated)
#gcc -g ${All_FLAGS} main.c vspectra.c disp.c plot.c cat.c geom.c time.c outfile.c sport.c map.c cmdfl.c cal.c srthelp.c velspec.c four.c amdfft.c librtlsdr.c tuner_r820t.c $LIBS -lm -lacml /usr/lib/libgfortran.a `pkg-config --libs --cflags libusb-1.0` -I/opt/libmodbus/include/modbus/ -L/opt/libmodbus/lib/ -l modbus

# compile for DAS-4020 + amd FFT
#gcc ${All_FLAGS} main.c vspectra_pci.c  disp.c plot.c cat.c geom.c time.c outfile.c sport.c map.c cmdfl.c cal.c srthelp.c velspec.c four.c amdfft.c $LIBS -lm -lacml /usr/lib/libgfortran.a -I/opt/libmodbus/include/modbus/ -L/opt/libmodbus/lib/ -l modbus

# compile for dongle + intel FFT
gcc ${All_FLAGS} main.c vspectra_fftw.c disp.c plot.c cat.c geom.c time.c outfile.c sport.c map.c cmdfl.c cal.c srthelp.c velspec.c four.c fftw2.c librtlsdr.c tuner_r820t.c encoder/encoder.c encoder/encoderRegisters.c $LIBS -lm -lfftw3f `pkg-config --libs --cflags libusb-1.0 gtk+-2.0` -I/opt/libmodbus/include/modbus/ -L/opt/libmodbus/lib/ -l modbus

# compile for DAS-4020 + intel FFT
#gcc ${All_FLAGS} main.c vspectra_pci_fftw.c  disp.c plot.c cat.c geom.c time.c outfile.c sport.c map.c cmdfl.c cal.c srthelp.c velspec.c four.c fftw2.c $LIBS -lm -lfftw3f -I/opt/libmodbus/include/modbus/ -L/opt/libmodbus/lib/ -l modbus

# compile for dongle + c-coded  FFT
#gcc ${All_FLAGS} main.c vspectra_four.c disp.c plot.c cat.c geom.c time.c outfile.c sport.c map.c cmdfl.c cal.c srthelp.c velspec.c four.c librtlsdr.c tuner_r820t.c $LIBS-lm `pkg-config --libs --cflags libusb-1.0` -I/opt/libmodbus/include/modbus/ -L/opt/libmodbus/lib/ -l modbus

if [ -e a.out ]
then
    cp a.out srtn
    rm a.out
else
    echo "Something went wrong, 'a.out' not produced"
fi