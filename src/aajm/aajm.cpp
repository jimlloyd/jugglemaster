/*
 * AAJM - AA JuggleMaster
 * (C) Gary Briggs 2003
 *
 * JuggleMaster is  free software; you can redistribute  it and/or modify
 * it under the  terms of the GNU General  Public License as published
 * by the Free  Software Foundation; either version 2  of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the GNU
 * General Public License for more details.
 */ 

#include "./aajm.h"

void errorCB(char* msg) {
	aa_printf(context, 1, 1, AA_NORMAL, "%s", msg);
	aa_flush(context);
	sleep(2);
}

void draw_juggler(void) {
	int color;
	int i;

	arm* ap = &(jmlib->ap);
	ball* rhand = &(jmlib->rhand);
	ball* lhand = &(jmlib->lhand);
	hand* handp = &(jmlib->handpoly);

	color = 255;
	// draw head
	aa_drawcircle(context,ap->hx, ap->hy,
			ap->hr, color);

	aa_drawline(context, 0, 0, AAWIDTH(context)/2, AAHEIGHT(context)/2, 1);
	// draw juggler
	for (i=0;i<5;i++) {
		aa_drawline(context, ap->rx[i], ap->ry[i],
			ap->rx[i+1], ap->ry[i+1], color);
		aa_drawline(context, ap->lx[i], ap->ly[i],
			ap->lx[i+1], ap->ly[i+1], color);
	}

	// hands
	for (i=0; i <= 8; i++) {
		aa_drawline(context, rhand->gx + handp->rx[i],
			rhand->gy + handp->ry[i],
			rhand->gx + handp->rx[i+1],
			rhand->gy + handp->ry[i+1], color);
		aa_drawline(context, lhand->gx + handp->lx[i],
			lhand->gy + handp->ly[i],
			lhand->gx + handp->lx[i+1],
			lhand->gy + handp->ly[i+1], color);
	}
	aa_drawline(context, rhand->gx + handp->rx[9],
		rhand->gy + handp->ry[9],
		rhand->gx + handp->rx[0],
		rhand->gy + handp->ry[0], color);
	aa_drawline(context, lhand->gx + handp->lx[9],
		lhand->gy + handp->ly[9],
		lhand->gx + handp->lx[0],
		lhand->gy + handp->ly[0], color);

	// draw balls
	int diam = (11*jmlib->dpm/DW);
	for(i=jmlib->balln-1;i>=0;i--) {
		aa_drawcircle(context, jmlib->b[i].gx + diam,
				jmlib->b[i].gy + diam,
				diam, color);
	}


	aa_render(context, params, 0, 0,
		AAWIDTH(context), AAHEIGHT(context));

	aa_printf(context, 0, 0, AA_SPECIAL,
		"Site: %s    Style: %s    Balls: %i",
		jmlib->getSite(), jmlib->getStyle(), jmlib->balln);

	if(jmlib->getStatus() == ST_PAUSE) {
		aa_puts(context, 0, 1, AA_SPECIAL, "Paused");
	}
	aa_flush(context);

	memset(context->imagebuffer,0,AAWIDTH(context)*AAHEIGHT(context));
}

void resizehandler(aa_context *resized_context) {
	jmlib->setWindowSize(AAWIDTH(resized_context),
			AAHEIGHT(resized_context));
	context = resized_context;
}

void main_loop(void) {
	char c;
	int i;
	char newsite[JML_MAX_SITELEN];
	char newstyle[2];
	int newstyle_index;
	int numstyles = sizeof(possible_styles)/sizeof(possible_styles[0]);

	while (1) {
		jmlib->doJuggle();
		draw_juggler();
		c=aa_getkey(context,0);
		if(c=='s' || c=='S') {
			/* Change SiteSwap */
			memset(newsite,0,JML_MAX_SITELEN);
			strncpy(newsite, jmlib->getSite(), JML_MAX_SITELEN);
			aa_puts(context, 1, 4, AA_SPECIAL,
				"Enter New SiteSwap...");
			aa_edit(context, 1, 5, 20,
				newsite, JML_MAX_SITELEN);
			if(newsite[0]!=0) {
				jmlib->setPattern("Something",newsite,
					HR_DEF, DR_DEF);
				jmlib->setStyleDefault();
				jmlib->startJuggle();
			}
		} else if(c=='q' || c=='Q' || c==27) {
			/* Quit */
			/* 27 == Escape
			Don't complain. This was a hack before it started */
			return;
		} else if(c==' ' || c=='p' || c=='P') {
			/* Toggle Pause */
			jmlib->togglePause();
		} else if(c=='t' || c=='T') {
			/* Change Style */
			aa_puts(context, 3, 4, AA_SPECIAL,
				"Choose New Style...");
			for (i=0;i<numstyles;i++) {
				aa_printf(context, 3, 5+i, AA_SPECIAL,
					"%i: %s",i+1,possible_styles[i]);
			}
			aa_flush(context);
			memset(newstyle,0,JML_MAX_SITELEN);
			aa_edit(context, 3, 5+i, 2, newstyle, 2);
			if(newstyle[0]!=0) {
				newstyle_index=atoi(newstyle)-1;
				if(newstyle_index>=0
				  && newstyle_index<numstyles) {
					jmlib->setStyle(possible_styles[newstyle_index]);
				}
			}
		} else if(c=='h' || c=='H') {
			aa_puts(context, 3, 4, AA_SPECIAL,
				"Key Help");
			aa_puts(context, 3, 6, AA_SPECIAL,
				"h - This screen");
			aa_puts(context, 3, 7, AA_SPECIAL,
				"s - Change Siteswap");
			aa_puts(context, 3, 8, AA_SPECIAL,
				"t - Change Style");
			aa_puts(context, 3, 9, AA_SPECIAL,
				"q - Quit");
			aa_puts(context, 3, 10, AA_SPECIAL,
				"space - Pause");
			aa_puts(context, 3, 12, AA_SPECIAL,
				"Press any key to remove this menu");
			aa_flush(context);
			aa_getkey(context, 1);
		}
		usleep(35);
	}
}

int main(int argc, char **argv) {
	char options[] = "ahs:";
	int help_flag = 0;
	int aahelp_flag = 0;
	static struct option long_options[] =
        {
		{"help", no_argument, &help_flag, 1},
		{"aahelp", no_argument, &aahelp_flag, 1},
		{"siteswap", required_argument, 0, 's'},
	};

	char optch;
	int option_index = 0;

	jmlib = new JMLib(errorCB);
	jmlib->setPatternDefault();
	jmlib->setStyleDefault();
	jmlib->startJuggle();

	aa_parseoptions(NULL, NULL, &argc, argv);

	while( (optch = getopt_long(argc,argv,options,
			long_options,&option_index)) != -1)
		switch(optch) {
			case 's':
				jmlib->setPattern("Something",optarg,
					HR_DEF, DR_DEF);
				jmlib->startJuggle();
				break;
			case 'h':
				help_flag=1;
				break;
			case 'a':
				aahelp_flag=1;
				break;
		}

	if(aahelp_flag || help_flag) {
		printf("AAJM, An ASCII Art Juggling program\n");
		printf("Usage: %s [OPTIONS]\n",argv[0]);
	}
	if(help_flag) {
		printf("Jugglemaster Options:\n");
		printf("  -s, --siteswap=XX  show siteswap XX\n");
		printf("  -h, --help         get help\n");
		printf("  -a, --aahelp       get help on AA options\n\n");
	}
	if(aahelp_flag) {
		printf("AALib Options:\n%s\n\n",aa_help);
	}
	if(aahelp_flag || help_flag) {
		return 0;
	}


	context = aa_autoinit(&aa_defparams);
	if (context == NULL) {
		printf("Failed to initialize aalib\n");
		exit(1);
	}
	aa_autoinitkbd(context, 0);
	params = aa_getrenderparams();
	aa_hidecursor(context);
	jmlib->setWindowSize(AAWIDTH(context),AAHEIGHT(context));

	aa_resizehandler(context, resizehandler);

	main_loop();

	aa_close(context);

	delete jmlib;
	return 1;
}
