#include <gtk/gtk.h>
#include <sys/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <sched.h>
#include <fcntl.h>
#include <errno.h>
#include "d1cons.h"
#include "d1proto.h"
#include "d1typ.h"
#include "d1glob.h"

/* -- Encoder related -- */
#include "encoder/encoder.h"


void azel(double az, double el)   // command antenna movement
{
    /* -- Encoder related -- */
    angle      angle;

    int        n, ix, iy, midxr, ixe, yr, da, hr, mn, sc;
    static int kk;
    double     azz, ell, ra, dec, x, y;
    char       str[80], recv[256], txt[80];
    GdkColor   color;

    azz = ell = 0;
    d1.slew   = 0;

    ix    = midx * 1.55;
    ixe   = midx * 0.25;
    midxr = midx * 2 - ix;

    if (d1.lat >= 0.0)
        sprintf (txt, "%s %4.1fN %5.1fW", d1.statnam,  d1.lat * 180.0 / PI, d1.lon * 180.0 / PI);
    else
        sprintf (txt, "%s %4.1fS %5.1fW", d1.statnam, -d1.lat * 180.0 / PI, d1.lon * 180.0 / PI);
    iy = midy * 0.05;
    if (d1.displ)
    {
        gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ix, iy - midy * 0.04, midxr, midy * 0.05);
        gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
    }
    sprintf (txt, "cmd  %5.1f %4.1f deg", az, el);
    iy = midy * 0.15;
    if (d1.displ)
        gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
    else if (d1.debug)
    {
        toyrday(d1.secs, &yr, &da, &hr, &mn, &sc);
        printf ("%4d:%03d:%02d:%02d:%02d %3s ", yr, da, hr, mn, sc, d1.timsource);
        printf ("%s\n", txt);
    }
    // sprintf (txt, "offsets %5.1f %4.1f deg", d1.azoff, d1.eloff);
    
    /* -- Encoder related -- */
    angle = EncoderGetSTAngle(127);
    sprintf (txt, "enc %5.1f deg, offset %5.1f ", angle.deg, (angle.deg - d1.aznow));

    iy = midy * 0.25;
    if (d1.displ)
        gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));

    if (d1.ellim2 > 90.0)    // to support going over 90 deg
    {
       if (az < d1.azlim2 - 180.0)
       {
           az += 180.0;
           el = 180.0 - el;
       }
       if (az > d1.azlim1 + 180.0)
       {
           az += -180.0;
           el = 180.0 - el;
       }
       if (d1.debug && el > 90.0)
           printf ("Antenna el over 90 deg az %f el %f\n",az,el);
    }
    azz = az;
    ell = el;

    if (!d1.azelsim && (fabs(d1.aznow - d1.azcmd) > 0.5 || fabs(d1.elnow - d1.elcmd) > 0.5))
    {
        if (d1.rot2mode < 10)
            d1.comerr = rot2(&azz, &ell, 1, recv); // initial read return if antenna at correct position
        else
            d1.comerr = h180(&azz, &ell, 1, recv); // initial read return if antenna at correct position
        if (d1.comerr == -1)
        {
            printf ("Can't talk to antenna controller\n");
            return;
        }
    }
    else
    {
        azz = d1.azprev;
        ell = d1.elprev;
    }
    d1.aznow = azz;
    d1.elnow = ell;
    if (d1.debug)
        printf ("aznow_after_read %3.0f elnow %3.0f\n", d1.aznow, d1.elnow);

//    if ((fabs(d1.aznow - d1.azcmd) > 0.5 || fabs(d1.elnow - d1.elcmd) > 0.5) // was 1 should get within 0.5 deg
    if ( (fabs(d1.aznow - d1.azcmd) >= 0.5 || fabs(d1.elnow - d1.elcmd) >= 0.5) && (az >= d1.azlim1 && az < d1.azlim2 && el >= d1.ellim1 && el < d1.ellim2) ) // from steve.black@washburn.edu 27Nov17
        {
        azz = az;
        ell = el;
        if (!d1.azelsim)
        {
            if (d1.rot2mode < 10)
            {
               d1.comerr = rot2(&azz, &ell, 2, recv); // command move
               d1.comerr = rot2(&azz, &ell, 1, recv); // read
            }
            else
            {
               d1.comerr = h180(&azz, &ell, 2, recv); // command move
               d1.comerr = h180(&azz, &ell, 1, recv); // read
            }
            if (d1.comerr == -1)
            {
                printf ("Can't talk to antenna controller\n");
                return;
            }
        }
        else
        {
            azz = (d1.azcmd + d1.azprev) * 0.5;  // this angle is calculated after move command was sent
            if (!d1.south)
            {
               if (d1.azcmd < d1.azlim2 -360.0)  azz += 180.0;   
               if (d1.azprev < d1.azlim2 -360.0) azz += 180.0;   
               if (azz >= 360.0) azz += -360.0;
               if (azz >= 360.0) azz += -360.0;
            }
            ell = (d1.elcmd + d1.elprev) * 0.5;  // this angle is calculated after move command was sent
            if (fabs(d1.aznow - d1.azcmd) < 2)
                azz = d1.azcmd;
            if (fabs(d1.elnow - d1.elcmd) < 2)
                ell = d1.elcmd;
        }
        d1.azprev = d1.aznow = azz;
        d1.elprev = d1.elnow = ell;
        if (d1.debug)
            printf ("aznow_after_cmd %3.0f elnow %3.0f cmd %3.0f %3.0f %3.0f %3.0f\n", d1.aznow, d1.elnow, d1.azcmd, d1.elcmd, az, el);
    }

    if (d1.azelsim)
        sprintf (str, "antenna drive status:");
    else
        sprintf (str, "antenna drive simulated:");
    if (d1.comerr > 0)
    {
        sprintf (txt, " comerr= %d", d1.comerr);
        iy = midy * 0.1;
        if (d1.displ)
        {
            gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ixe, iy - midy * 0.04, midxr, midy * 0.05);
            gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ixe, iy, txt, strlen(txt));
        }
    }
    if ( ((az < d1.azlim1 || az > d1.azlim2 || el < d1.ellim1 || el > d1.ellim2) && d1.south) || (((az < d1.azlim1 && az > d1.azlim2-360.0) || el < d1.ellim1 || el > d1.ellim2) && !d1.south) )
    {
        sprintf (txt, "cmd out of limits");
//        printf ("cmd out of limits\n");
        iy = midy * 0.10;
        if (d1.displ)
        {
            color.red = 0xffff;
            color.green = color.blue = 0;
            gdk_color_parse("red", &color);
            gtk_widget_modify_fg(drawing_area, GTK_STATE_NORMAL, &color);

            gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ix, iy - midy * 0.04, midxr, midy * 0.05);
            gdk_draw_text(pixmap, fixed_font, drawing_area->style->fg_gc[GTK_STATE_NORMAL], ix, iy, txt, strlen(txt));
        }
        d1.track = 0;
        if (d1.printout)
        {
            toyrday(d1.secs, &yr, &da, &hr, &mn, &sc);
            printf ("%4d:%03d:%02d:%02d:%02d %3s ", yr, da, hr, mn, sc, d1.timsource);
            printf ("cmd out of limits az %f el %f\n", az, el);
        }
        sprintf (d1.recnote, "* cmd out of limits az %f el %f\n", az, el);
        outfile(d1.recnote);
        if (d1.stow != -1)
        {
            d1.stow = 1;
            if (d1.stowatlim)
            {
                d1.elcmd = d1.ellim1;
                d1.azcmd = d1.azlim1;
            }
            else
            {
                d1.elcmd = d1.stowel;
                d1.azcmd = d1.stowaz;
            }
        }
        return;
    }
    if ( (fabs(d1.aznow - d1.azcmd) > 1.0 || fabs(d1.elnow - d1.elcmd) > 1.0) && d1.track != -1 )
    {
        if ( (fabs(d1.aznow - d1.azcmd) > 1.0 || fabs(d1.elnow - d1.elcmd) > 1.0) && d1.stow != -1 )
        {
            d1.slew = 1;
            sprintf (txt, "ant slewing");
            if (d1.printout)
            {
                toyrday(d1.secs, &yr, &da, &hr, &mn, &sc);
                printf ("%4d:%03d:%02d:%02d:%02d %3s ", yr, da, hr, mn, sc, d1.timsource);
                if (soutrack[0])
                    printf ("ant slewing to %s\n", soutrack);
                if (d1.stow == 1)
                    printf ("ant slewing to stow\n");
            }
            iy = midy * 0.1;
            if (d1.displ)
            {
                gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ix, iy - midy * 0.04, midxr, midy * 0.05);
                gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
            }
        }

        if (d1.displ)
        {
            x = d1.azcmd * midx / 180.0;
            if (d1.south == 0)
                x -= midx;
            if (x < 0)
                x += midx * 2;
            if (x > midx * 2)
                x -= midx * 2;
            y = midy * 2 - d1.elcmd * midy * 2.0 / 180.0;
            color.red = 0;
            color.green = 0;
            color.blue = 0xffff;
            gdk_color_parse("blue", &color);
            gtk_widget_modify_fg(drawing_area, GTK_STATE_NORMAL, &color);
            gdk_draw_line(pixmap, drawing_area->style->fg_gc[GTK_STATE_NORMAL], x - 4, y, x + 4, y);
            gdk_draw_line(pixmap, drawing_area->style->fg_gc[GTK_STATE_NORMAL], x, y - 4, x, y + 4);
        }
        n = 0;
        kk = 0;
        while (kk < 100 && n == 0)
        {
            azz = d1.aznow;
            ell = d1.elnow;
            if (!d1.azelsim)
            {
                if (d1.rot2mode < 10)
                    d1.comerr = rot2(&azz, &ell, 1, recv);
                else
                {             
                    if (kk) d1.comerr = h180(&azz, &ell, 2, recv);
                    d1.comerr = h180(&azz, &ell, 1, recv);
                }
                if (d1.comerr == -1)
                {
                    printf ("Can't talk to antenna controller\n");
                    return;
                }
            }
            else
            {
                azz = (d1.azcmd + d1.azprev) * 0.5;
            if (!d1.south)
                { 
                   if (d1.azcmd < d1.azlim2 -360.0) azz += 180.0;
                   if (d1.azprev < d1.azlim2 -360.0) azz += 180.0;
                   if (azz >= 360.0) azz += -360.0;
                   if (azz >= 360.0) azz += -360.0;
                }
                ell = (d1.elcmd + d1.elprev) * 0.5;
                if (fabs(d1.aznow - d1.azcmd) < 2)
                    azz = d1.azcmd;
                if (fabs(d1.elnow - d1.elcmd) < 2)
                    ell = d1.elcmd;
            }
            d1.azprev = d1.aznow = azz;
            d1.elprev = d1.elnow = ell;
            if (d1.printout)
                printf ("aznow %3.0f elnow %3.0f k %d\n", d1.aznow, d1.elnow, kk);
            if (fabs(d1.aznow - d1.azcmd) > 1.0 || fabs(d1.elnow - d1.elcmd) > 1.0)
            {
                if (d1.printout)
                    printf ("waiting on antenna cmd %3.0f %3.0f now %3.0f %3.0f kk %d\n", d1.azcmd, d1.elcmd, d1.aznow, d1.elnow, kk);
                sprintf (txt, "waiting on antenna %d ", kk);
                d1.slew = 1;
                if (d1.displ)
                {
                    ix = midx * 1.55;

                    midxr = midx * 2 - ix;
//                            cleararea();
                    x = d1.azcmd * midx / 180.0;
                    if (d1.south == 0)
                        x -= midx;
                    if (x < 0)
                        x += midx * 2;
                    if (x > midx * 2)
                        x -= midx * 2;
                    y = midy * 2 - d1.elcmd * midy * 2.0 / 180.0;
                    color.red = 0;
                    color.green = 0;
                    color.blue = 0xffff;
                    gdk_color_parse("blue", &color);

                    gtk_widget_modify_fg(drawing_area, GTK_STATE_NORMAL, &color);
                    gdk_draw_line(pixmap, drawing_area->style->fg_gc[GTK_STATE_NORMAL], x - 4, y, x + 4, y);
                    gdk_draw_line(pixmap, drawing_area->style->fg_gc[GTK_STATE_NORMAL], x, y - 4, x, y + 4);

                    iy = midy * 0.1;
//                        gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ix, iy - midy * 0.04, midxr, midy * 0.05);
                    gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ix, iy - midy * 0.04, midxr, midy * 0.25);
                    gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
                    sprintf (txt, "cmd  %5.1f %4.1f deg", d1.azcmd, d1.elcmd);
                    iy = midy * 0.15;
                    gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
                    sprintf (txt, "azel %5.1f %4.1f deg", d1.aznow, d1.elnow);
                    iy = midy * 0.20;
                    gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
                    // sprintf (txt, "offsets %5.1f %4.1f deg", d1.azoff, d1.eloff);
                    
                    /* -- Encoder related -- */
                    angle = EncoderGetSTAngle(127);
                    sprintf (txt, "enc %5.1f deg, offset %5.1f ", angle.deg, (angle.deg - d1.aznow));
    
                    iy = midy * 0.25;
                    gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
                    azel_to_radec(d1.secs, d1.aznow, d1.elnow, &ra, &dec);
                    sprintf (txt, "ra %5.1f hr %4.1f deg", ra * 12.0 / PI, dec * 180.0 / PI);
                    iy = midy * 0.30;
                    gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
                    color.green = color.red = color.blue = 0xffff;
                    gtk_widget_modify_bg(button_stow, GTK_STATE_NORMAL, &color);
//                            if (!d1.plot) {
                    Repaint();
//                            }
//                            cleararea();
                    while (gtk_events_pending() || d1.stopproc)
                    {
                        gtk_main_iteration();
//                                 d1.plot = 0;
                    }

                }
                else if (d1.debug)
                    printf ("%s\n", txt);
                sleep(1);
            }
            else
                n = 1;
            kk++;
        }
        if (d1.debug)
            printf ("recv %s\n", recv);
        if (d1.comerr)
        {
            sprintf (txt, "comerr %d", d1.comerr);
            iy = midy * 0.1;
            if (d1.displ)
            {
                gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ixe, iy - midy * 0.04, midxr, midy * 0.05);
                gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ixe, iy, txt, strlen(txt));
            }
            if (d1.mainten == 0)
                d1.stow = 1;
            return;
        }
        sprintf (txt, "recv %s", recv);
        iy = midy * 0.05;
        if (d1.displ)
        {
            gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ix, iy - midy * 0.04, midxr, midy * 0.05);
            gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt) - 1);
        }
        if (fabs(d1.aznow - d1.azcmd) > 1.0 || fabs(d1.elnow - d1.elcmd) > 1.0)
        {
//      printf ("lost count\n");

            sprintf (txt, "lost count goto Stow");
            d1.limiterr = 1;
            soutrack[0] = 0;
            iy = midy * 0.1;
            if (d1.displ)
            {
                gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ixe, iy - midy * 0.04, midxr, midy * 0.05);
                gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ixe, iy, txt, strlen(txt));
                sprintf (txt, "ERROR:  stopped at %3.0f %2.0f", d1.aznow, d1.elnow);
                iy = midy * 0.15;
                gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ixe, iy - midy * 0.04, midxr, midy * 0.05);
                gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ixe, iy, txt, strlen(txt));
                sprintf (txt, "while slewing");
                iy = midy * 0.20;
                gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ixe, iy - midy * 0.04, midxr, midy * 0.05);
                gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ixe, iy, txt, strlen(txt));
                sprintf (txt, "motor stalled or limit prematurely reached");
                iy = midy * 0.25;
                gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ixe, iy - midy * 0.04, midxr, midy * 0.05);
                gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ixe, iy, txt, strlen(txt));
            }
            if (d1.mainten == 0)
            {
                if (d1.azcmd < d1.azlim1 + 1 || d1.elcmd < d1.ellim1 + 1) // could hit limit at source set
                    d1.elnow = d1.ellim1;
                d1.stow = 1;
                d1.azcmd = d1.azlim1;
                d1.elcmd = d1.ellim1;
            }
            return;
        }
        if (d1.comerr == -1)
        {
            printf ("timeout from antenna\n");
            sprintf (txt, "timeout from antenna");
            iy = midy * 0.1;
            if (d1.displ)
            {
                gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ix, iy - midy * 0.04, midxr, midy * 0.05);
                gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
            }
        }
        if (d1.comerr == 0) {}  // normal response
        if (d1.slew)
            sleep(1);
    }
    if (d1.track != -1)
    {
        if (d1.slew == 1)
            d1.track = 0;
        else
            d1.track = 1;
    }
    x = (int) (d1.aznow * 640.0 / 360.0);
    if (d1.south == 0)
    {
        x -= 320;
        if (x < 0)
            x += 640;
    }
    if ((fabs(d1.aznow - d1.azlim1) < 0.2 && fabs(d1.elnow - d1.ellim1) < 0.2 && d1.stowatlim) ||   // needs test
        (fabs(d1.aznow - d1.stowaz) < 0.2 && fabs(d1.elnow - d1.stowel) < 0.2 && !d1.stowatlim))
        {
            if (d1.displ)
            {
                color.green = 0xffff;
                color.red = color.blue = 0;
                gdk_color_parse("green", &color);
                gtk_widget_modify_bg(button_stow, GTK_STATE_NORMAL, &color);
                gtk_tooltips_set_tip(tooltips, button_stow, "antenna at stow", NULL);
                gtk_tooltips_set_tip(tooltips, button_exit, "click to exit program", NULL);
                //  printf ("in green\n");  
            }
            d1.stow = -1;    // at stow

    }
    else
    {
        if (d1.displ)
        {
            color.green = color.red = color.blue = 0xffff;
            gtk_widget_modify_bg(button_stow, GTK_STATE_NORMAL, &color);
            gtk_tooltips_set_tip(tooltips, button_stow, "click to stow antenna", NULL);
            if (d1.track != -1)
                gtk_tooltips_set_tip(tooltips, button_exit, "go to stow first", NULL);
        }
        if (d1.stow == -1)
            d1.stow = 0;
    }
    if (d1.stow != 0)
        d1.track = 0;
    if (d1.displ)     // display text at right hand side of the main window
    {
        sprintf (txt, "azel %5.1f %4.1f deg", d1.aznow, d1.elnow);
        iy = midy * 0.20;
        gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
        azel_to_radec(d1.secs, d1.aznow, d1.elnow, &ra, &dec);
        sprintf (txt, "ra %5.1f hr %4.1f deg", ra * 12.0 / PI, dec * 180.0 / PI);
        iy = midy * 0.30;
        gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
        sprintf (txt, "cmd  %5.1f %4.1f deg", d1.azcmd, d1.elcmd);
        iy = midy * 0.15;
        gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
        // sprintf (txt, "offsets %5.1f %4.1f deg", d1.azoff, d1.eloff);

        /* -- Encoder related -- */
        angle = EncoderGetSTAngle(127);
        sprintf (txt, "enc %5.1f deg, offset %5.1f ", angle.deg, (angle.deg - d1.aznow));

        iy = midy * 0.25;
        gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
    }
    else if (d1.debug)
    {
        toyrday(d1.secs, &yr, &da, &hr, &mn, &sc);
        printf ("%4d:%03d:%02d:%02d:%02d %3s ", yr, da, hr, mn, sc, d1.timsource);
        printf ("now %s tant %5.1f Source: %s\n", txt, d1.tant, soutrack);
    }
    return;
}


/**
*  Here the angles are sotred in the driver
*/
int rot2(double *az, double *el, int cmd, char *resp)
{
    int    usbDev, status, rstatus;
    double azz, ell;
    char   command[14];
    if (d1.stowatlim)
    {
        azz = *az - d1.azlim1;
        ell = *el - d1.ellim1;
    }                           // allows controller to be set to az=0 el=0 at stow
    else
    {
        azz = *az;
        ell = *el;
    }                           // if controller is set to read correct az and el 
// for perm add to dialout group
    rstatus = 0;
    if (cmd == -1)
    {
        if (system("stty -F /dev/ttyUSB0 600 raw -echo -icanon min 0 time 20")){}; // needed to make timeout work. if (){} to avoid warning
//  system("stty -F /dev/ttyUSB0 600 raw -echo -icanon min 12 time 20"); // reads min of 12 chars 10 = up 10x100ms to between chars BUT timeout doesn't work
        return 0;
    }
    usbDev = 0;                 // cmd  0x0f=stop 0x1f=status 0x2f=set
    usbDev = open("/dev/ttyUSB0", O_RDWR, O_NONBLOCK);
    cmd = cmd * 16 + 0xf;
    if (!d1.rot2mode)
      sprintf (command, "W%04d%c%04d%c%c ", (int) (azz + 360.5), 1, (int) (ell + 360.5), 1, cmd);                 // round to nearest degree
    else
      sprintf (command, "W%04d%c%04d%c%c ", (int) (2*(azz + 360.0)+0.5), 2, (int) (2*(ell + 360.0)+0.5), 2, cmd); // round to nearest half degree
//    printf ("sent to azz %f ell %f\n",azz,ell);
//  for (i=0;i<13;i++) printf ("isend=%d ch=%2x\n",i,command[i]);
    status = write(usbDev, command, 13);
//    if (d1.debug)
        printf ("write status %d cmd %x usbDev %d az %7.2f el %7.2f\n", status, cmd, usbDev,azz,ell);
    sleep(d1.rot2slp);                   // sleep needed
    if (cmd != 0x1f)
        sleep(d1.rot2slp);               // extra sleep needed
    if (cmd == 0x1f)
    {
        rstatus = read(usbDev, resp, 12);
        if (d1.debug)
            printf ("read status %d\n", rstatus);
//   for (i=0;i<12;i++) printf ("irec=%d ch=%2x\n",i,resp[i]);
     if (!d1.rot2mode)
     {
        azz = resp[1] * 100 + resp[2] * 10 + resp[3] - 360;    // response from the motor (angle)
        ell = resp[6] * 100 + resp[7] * 10 + resp[8] - 360;
     }
      else
      {
        azz = resp[1] * 100 + resp[2] * 10 + resp[3] + resp[4] / 10.0 - 360;
        ell = resp[6] * 100 + resp[7] * 10 + resp[8] + resp[9] / 10.0 - 360;
      }
//   printf ("now at azz %f  ell %f\n",azz,ell);
        if (d1.stowatlim)
        {
            *az = azz + d1.azlim1;
            *el = ell + d1.ellim1;
        }
        else
        {
            *az = azz;
            *el = ell;
        }
    }
    close (usbDev);
    if (rstatus != 12)
        return -1;
    else
        return 0;
}


/**
*  Executes motor movement
*
* d1.azcount d1.elcount are stroing positions as an impuse count.
* After command "move" is sent, angles azz and ell are computed and added to the
* minimal angles d1.azlim1 and d1.ellim1. Results are stored in *az *el.
* 
* Next form given angles of antena movement - minimal angles d1.azlim1 and d1.ellim1
* are subtracted. New angles azz and ell are computed and multiplied by the count of
* impulses per degree for az and el d1.azcounts_per_deg d1.elcounts_per_deg. Next the
* initial impulse count d1.azcount d1.elcount is subtracted and they are storred to
* acount. Next, depending on rotation angle mm (o and 1 for one axis, 2 and 3 for the
* other) are modified and sent to the driver as count.
* 
* Next the driver status is read. It contains impulse count that are alter added/subtracted
* from initial values d1.azcount d1.elcount.
* 
* If stow is executed, initial counts are zeroed.
*
*/
int h180(double *az, double *el, int cmd, char *resp)
{
  int    usbDev;
  int    mm;    // direction of rotation
  int    status, rstatus, count, axis, i, j, im, ccount;
  double azz, ell, acount;
  char   command[16], ch;

  if (cmd == 1)
  {
     azz = d1.azcount / d1.azcounts_per_deg;
     ell = d1.elcount / d1.elcounts_per_deg;
     *az = azz + d1.azlim1;
     *el = ell + d1.ellim1;
     return 0;
  }
 
  if (cmd == -1)
  {
//  printf ("here cmd = -1\n");
   int unused __attribute__((unused));   // to avoid warning
   unused = system("stty -F /dev/ttyUSB0 2400 cs8 -cstopb -parenb -icanon min 1 time 20");
   usbDev = open("/dev/ttyUSB0", O_RDWR, O_NONBLOCK);
   sleep(1);
   status = close(usbDev);
   if (d1.debug)
       printf ("here stty usbDev %d status %d\n", usbDev, status);
   if (usbDev<0)
       return -1;
   else
       return 0;
  }
  usbDev = status = count = 0; 
  usbDev = open("/dev/ttyUSB0", O_RDWR, O_NONBLOCK);
  printf ("usbDev %d\n", usbDev);
  sleep(1);
  azz = d1.azcmd - d1.azlim1;
  ell = d1.elcmd - d1.ellim1;
  for (axis=0; axis<2; axis++)
  {
      mm = -1;
      if (axis==0)
      {
          acount = azz * d1.azcounts_per_deg - d1.azcount;
          if (d1.countperstep && acount > d1.countperstep)
              acount = d1.countperstep;
          if (d1.countperstep && acount < -d1.countperstep)
              acount = -d1.countperstep;
        // printf ("acount %f azz %f d1.count %d %f usbDev %d\n",acount,azz,d1.azcount,azz * d1.azcounts_per_deg - d1.azcount,usbDev);
          if (acount > 0) count = acount + 0.5; else count = acount - 0.5; 
          if (count > 0) mm = 1; 
          if (count < 0) mm = 0;
      }
      if (axis == 1)
      {
          acount = ell * d1.elcounts_per_deg - d1.elcount;
          if (d1.countperstep && acount > d1.countperstep)
              acount = d1.countperstep;
          if (d1.countperstep && acount < -d1.countperstep)
              acount = -d1.countperstep;
          if (acount > 0)
              count = acount + 0.5;
          else
              count = acount - 0.5;
          if (count > 0) mm = 3;
          if (count < 0) mm = 2;
      }
      if (count < 0)
          count = -count;
      if (d1.stow == 1)
      {
          if (axis == 0)
              mm = 0;
          else
              mm = 2;
          count = 8000;
      }
      if (cmd == 2 && mm >= 0 && count && usbDev > 0)
      {
          if (d1.debug)
              printf (" move %d count %d %d\n", mm, count, '\n');
          sprintf (command, " move %d %d%1c", mm, count, 13);    // constructing a command
          status = write(usbDev, command, strlen(command));      // here the command goes to the device
          //  printf ("write status %d usbDev %d\n",status,usbDev);
          for (i=0; i<32; i++)
              resp[i]=0;
          usleep(10000);
          im = 0;
          //  sleep(1);
          i = j = 0;
          while (j==0 && i < 32)
          {
              rstatus = read(usbDev, &ch, 1);
              if (d1.debug)
                   printf ("here axis %d status %d ch %1c %x i = %d usbDev %d rstatus %d\n", axis, status, ch, ch, i, usbDev, rstatus);
              usleep(10000);
              if (status && i < 32)
                  resp[i] = ch;
              i++;
              if (ch == 13 || ch == 10)
                  j=99; // end 
          }
          status = i;
          usleep(100000);
          for (i=0; i<status; i++)
              if (resp[i]=='M' || resp[i]=='T')
                im = i;
          //  for (i=0;i<status;i++) printf ("i %d %c\n",i,resp[i]);
          sscanf(&resp[im], "%*s %d", &ccount);
          if (d1.debug)
              printf ("status %d im %d move mm %d sent %d recvd %d %12s rstatus %d azz %f ell %f\n", status, im, mm, count, ccount, &resp[im], rstatus, *az, *el);
          if (resp[im] == 'M')
          {
            if (mm==1) d1.azcount += ccount; 
            if (mm==0) d1.azcount -= ccount; 
            if (mm==3) d1.elcount += ccount; 
            if (mm==2) d1.elcount -= ccount; 
          }
          if (resp[im] == 'T')
          {
            if (mm==1) d1.azcount += count;
            if (mm==0) d1.azcount -= count;
            if (mm==3) d1.elcount += count;
            if (mm==2) d1.elcount -= count;
          }
      }
  }
  if (d1.stow == 1) d1.stow = d1.azcount = d1.elcount = 0;
     status = close(usbDev);
//  printf ("here close usb status %d\n",status);
//     sleep(2);
  return 0;
}
