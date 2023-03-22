// cat.c: Parse configuration file srt.cat

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "d1cons.h"
#include "d1proto.h"
#include "d1typ.h"
#include "d1glob.h"


int catfile(void)
{
    FILE *file1;
    char buf[256], decsign[80];
    double rah, ram, rass, decd, decm, decss, glat, glon;
    int i;
    int yr, dy, hr, mn, sc;
    d1.nsou   = 0;
    d1.track  = 0;
    d1.newday = 1;
    d1.unitid = 0;
    d1.record_spec = 0;
    d1.noisecal    = 300.0;
    if ((file1 = fopen(d1.catnam, "r")) == NULL)
    {
        printf(" Unable to open %s\n", d1.catnam);
        return 0;
    }
    while (fgets(buf, 256, file1) != 0)
    {
        if (buf[0] != '*' && buf[0] != '#')
        {
            if (kmatch(buf, (char*)"STATION ")) // space forces exact match
            {
                sscanf(buf, "%*s %lf %lf %24s %lf", &d1.lat, &d1.lon, d1.statnam, &d1.hgt);
                d1.lat = d1.lat * PI / 180.0;
                d1.lon = d1.lon * PI / 180.0;
            }
            if (kmatch(buf, (char*)"AZLIMITS "))
                sscanf(buf, "%*s %lf %lf", &d1.azlim1, &d1.azlim2);
            if (kmatch(buf, (char*)"ELLIMITS "))
                sscanf(buf, "%*s %lf %lf", &d1.ellim1, &d1.ellim2);
            if (kmatch(buf, (char*)"STOWPOS "))
            {
                sscanf(buf, "%*s %lf %lf", &d1.stowaz, &d1.stowel);
                d1.stowatlim = 0;
            }
            if (kmatch(buf, (char*)"NOISECAL "))
                sscanf(buf, "%*s %lf", &d1.noisecal);
            if (kmatch(buf, (char*)"TCAL "))
                sscanf(buf, "%*s %lf", &d1.tcal);
            if (kmatch(buf, (char*)"CALMODE "))
                sscanf(buf, "%*s %d", &d1.calmode); // 0-normal
            if (kmatch(buf, (char*)"SIMULATE"))
            {
                if (strstr(buf, (char*)"ANTENNA"))
                    d1.azelsim = 1;
                if (strstr(buf, (char*)"RECEIVER"))
                    d1.radiosim = 1;
                if (strstr(buf, (char*)"FFT"))
                    d1.fftsim = 1;
            }
            if (kmatch(buf,(char*) "MAINTENANCE"))
                d1.mainten = 1;
            if (kmatch(buf, (char*)"RMSCALC"))
                d1.rms = 0;
            if (kmatch(buf, (char*)"PLOTSEC "))
                sscanf(buf, "%*s %d", &d1.plotsec);
            if (kmatch(buf, (char*)"NBSW "))
                sscanf(buf, "%*s %d", &d1.nbsw);
            if (kmatch(buf, (char*)"NOCLEARINT "))
                d1.noclearint = 1;
            if (kmatch(buf, (char*)"NBLOCK "))
                sscanf(buf, "%*s %d", &d1.nblk);
            if (kmatch(buf, (char*)"NUMPOLY "))
                sscanf(buf, "%*s %d", &d1.npoly);
            if (kmatch(buf, (char*)"BEAMWIDTH "))
                sscanf(buf, "%*s %lf", &d1.beamw);
            if (kmatch(buf, (char*)"TOLERANCE "))
                sscanf(buf, "%*s %d", &d1.ptoler);
            if (kmatch(buf, (char*)"CASSIMOUNT"))
            {
                d1.rod = 1;
                d1.azcounts_per_deg = 8.0 * 32.0 * 60.0 / (360.0 * 9.0);
                sscanf(buf, "%*s %lf %lf %lf %lf %lf", &d1.rod1, &d1.rod2, &d1.rod3, &d1.rod4, &d1.rod5);
            }
            if (kmatch(buf, (char*)"H180MOUNT"))
            {
                d1.rod = 0;
                d1.azcounts_per_deg = (52.0 * 27.0 / 120.0); // for H-180
                d1.elcounts_per_deg = (52.0 * 27.0 / 120.0); // for H-180
                d1.rot2mode = 10; // for old SRT controller
            }
            if (kmatch(buf, (char*)"ALFASPID"))
            {
                d1.rod = 0;
                d1.azcounts_per_deg = 1.0; // for SPID
                d1.elcounts_per_deg = 1.0; // for SPID
            }
            if (kmatch(buf, (char*)"BIGRAS"))
            {
                d1.rot2mode = 1; // for BIG RAS
            }
            if (kmatch(buf, (char*)"ROT2SLP"))
                sscanf(buf, "%*s %d", &d1.rot2slp);   // sleep time in seconds
            if (kmatch(buf, (char*)"AZCOUNTS "))
                sscanf(buf, "%*s %lf", &d1.azcounts_per_deg);
            if (kmatch(buf, (char*)"ELCOUNTS "))
            {
                sscanf(buf, "%*s %lf", &d1.elcounts_per_deg);
                d1.rod = 0;
            }
            if (kmatch(buf, (char*)"AZELPORT "))
                sscanf(buf, "%*s %x", (unsigned int*)&d1.azelport);
            if (kmatch(buf, (char*)"COMMAND"))
            {
                d1.cmdfl = 1;
                d1.cmdfline = 0;
                d1.secstop = d1.secs + 4; // time to get to stow
                if (strstr(buf, (char*)"COMMAND "))
                    sscanf(buf, "%*s %255s", d1.cmdfnam);
            }
            if (kmatch(buf, (char*)(char*)"TSYS "))
                sscanf(buf, "%*s %lf", &d1.tsys);
            if (kmatch(buf, (char*)"FREQUENCY "))
                sscanf(buf, "%*s %lf", &d1.freq);
            if (kmatch(buf, (char*)"RESTFREQ "))
                sscanf(buf, "%*s %lf", &d1.restfreq);
            if (kmatch(buf, (char*)"FREQCORR ")) // frequency correction for dongle in MHz
                sscanf(buf, "%*s %lf", &d1.freqcorr);
            if (kmatch(buf, (char*)"BANDWIDTH ")) {
                sscanf(buf, "%*s %lf", &d1.fbw);
            }
            if (kmatch(buf, (char*)"NUMFREQ "))
            {
                sscanf(buf, "%*s %d", &d1.nfreq);
                if (d1.nfreq > NSPEC || d1.nfreq < 1)
                    d1.nfreq = NSPEC;
            }
            if (kmatch(buf, (char*)"DATADIR"))
            {
                d1.datadir[0] = 0;
                if (strstr(buf, (char*)"DATADIR "))
                    sscanf(buf, "%*s %255s", d1.datadir);
            }
            if (kmatch(buf, (char*)"COUNTPERSTEP "))
                sscanf(buf, "%*s %d", &d1.countperstep);
            if (kmatch(buf, (char*)"RECORD"))
            {
                sscanf(buf, "%*s %d", &d1.record_int_sec);
                if (strstr(buf, (char*)"SPEC"))
                    d1.record_spec = 1;
                if (strstr(buf, (char*)"RCLR"))
                    d1.record_clearint = 1; // clears integration each output
            }
            if (kmatch(buf, (char*)"NODISPLAY"))
                d1.displ = 0;
            if (kmatch(buf, (char*)"LOCKSRT"))
                d1.lock = 1;
            if (kmatch(buf, (char*)"NOPRINTOUT"))
                d1.printout = 0;
            if (kmatch(buf, (char*)"DEBUG"))
                d1.debug = 1;
            if (kmatch(buf, (char*)"SPEED_UP "))
                sscanf(buf, "%*s %d", &d1.speed_up);
            if (kmatch(buf, (char*)"START_DATE ")) {
                sscanf(buf, "%*s %d:%d:%d:%d:%d", &yr, &dy, &hr, &mn, &sc);
                d1.start_sec = tosecs(yr, dy, hr, mn, sc);
            }
            if (kmatch(buf, (char*)"SOU "))
            {
                epoc[d1.nsou] = 2000.0; // default
                sscanf(buf, "%*s %lf %lf %lf %lf %lf %lf %24s %lf", &rah, &ram, &rass, &decd, &decm, &decss, sounam[d1.nsou], &epoc[d1.nsou]);
                ras[d1.nsou] = (rah + ram / 60.0 + rass / 3600.0) * TWOPI / 24.0;
                decs[d1.nsou] = (fabs(decd) + decm / 60.0 + decss / 3600.0) * TWOPI / 360.0;
                sscanf(buf, "%*s %*s %*s %*s %79s", decsign);
                if (strstr(decsign, "-"))
                    decs[d1.nsou] = -decs[d1.nsou];
                soutype[d1.nsou] = 0;
                if (d1.nsou < NSOU)
                    d1.nsou++;
            }
            if (kmatch(buf, (char*)"GALACTIC "))
            {
                epoc[d1.nsou] = 2000.0;
                sscanf(buf, "%*s %lf %lf %24s", &glon, &glat, sounam[d1.nsou]);
                GalactictoRadec(glat, glon, &ras[d1.nsou], &decs[d1.nsou]);
                soutype[d1.nsou] = 0;
                if (d1.nsou < NSOU)
                    d1.nsou++;
            }
            if (kmatch(buf, (char*)"AZEL "))
            {
                sscanf(buf, "%*s %lf %lf %24s", &ras[d1.nsou], &decs[d1.nsou], sounam[d1.nsou]);
                soutype[d1.nsou] = 1;
                if (d1.nsou < NSOU)
                    d1.nsou++;
            }
            if (kmatch(buf, (char*)"RFISIGMA ")) // level at which to report RFI default = 6 
                sscanf(buf, "%*s %lf", &d1.rfisigma);
            if (kmatch(buf, (char*)"RFI "))
            {
                d1.rfiwid[d1.nrfi] = 0;
                sscanf(buf, "%*s %lf %lf", &d1.rfi[d1.nrfi], &d1.rfiwid[d1.nrfi]);
                if (d1.nrfi < NRFI)
                    d1.nrfi++;
            }
        }
    }
    for (i = 0; i < d1.nsou; i++)
        sprintf(sounam[i], "%s", strncat(sounam[i], " ", 1));
    fclose(file1);
    return 1;
}

char *kmatch(char *buf, char *ct)
// requires keyword to be at start of line
{
    int i;
    char *p, *q;
    i = 0;
    p = 0;
    while ((unsigned) i <= strlen(buf) && !p)
    {
        if (buf[i] != ' ')
            p = &buf[i];
        i++;
    }
    q = strstr(buf, ct);
    if (q == p)
        return p;
    else
        return 0;
}
