typedef struct
{
  double lat;
  double lon;
  double hgt;
  double integ;
  double integ2;
  double integ3;
  double dmax;
  double freq;
  double lofreq;
  double efflofreq;
  double vlsr;
  double start_time;
  double start_sec;
  double azlim1, ellim1;                       // minimal angles
  double azcounts_per_deg, elcounts_per_deg;   // how many impulses per degree
  double av, avc, glat, glon, intg, restfreq, bw, delay, azoff, eloff, scnazoff, scneloff, calcons, beamw, smax, rod1,rod2,rod3,rod4,rod5, azlim2, ellim2, tsys, secs, tant,tload, tspill, aznow, elnow, azcmd, elcmd, 
    pwroff, pwron, bswpwr, noisecal, avbsw, calpwr, yfac, f1, fc, f2, fbw, tcal, freqcorr, rfisigma, azprev, elprev, stowaz, stowel, rms;
  double rfi[25];
  double rfiwid[25];
  int speed_up;
  int nsou;
  int foutstatus;
  int ppos;
  int ptick;
  int nsecs;
  int plotsec;
  int displ;
  int printout;
  int debug;
  int nfreq;
  int record_int_sec;
  int record_spec;
  int record_clearint;
  int record;
  int newday;
  int rday;
  int freqchng;
  int track;
  int nsecstart;
  int azcount, elcount;   // celestial coordinates represented in hardware impulse system
  int secstop, clearint, noclearint, fstatus, radiosim, azelsim, mainten, domap, xmark, map, mancal, south, ptoler, countperstep,
    azelport, drift, scan, stow, slew, sourn, bsw, nbsw, bswint, calon, calmode, docal, caldone, rod, stopproc, comerr, limiterr, 
    cmdfl, cmdfline, year, nblk, nsam, unitid, run, wid, whgt, vwid, vwhgt, numon, numoff, psw, entry1, entry2, entry3, entry5, entry6, entry8,
    helpwindow, vwindow, plot, obsn, nrfi, dongle, npoly, stowatlim, rot2slp, rot2mode, lock, ver;
  int fftsim;
  unsigned int seed;
  int devices;
  int id;
  long tstart;
  char filname[256];
  char cmdfnam[256];
  char datadir[256];
  char catnam[64];
  char hlpnam[64];
  char statnam[32];
  char timsource[8];
  char recnote[256];
} d1type;