/*
 * JMLib - Portable JuggleMaster Library
 * Version 2.1
 * (C) Per Johan Groland 2000-2008, Gary Briggs 2003
 *
 * Based on JuggleMaster Version 1.60
 * Copyright (c) 1995-1996 Ken Matsuoka
 *
 * JuggleSaver support based on Juggler3D
 * Copyright (c) 2005-2008 Brian Apps <brian@jugglesaver.co.uk>
 *
 * You may redistribute and/or modify JMLib under the terms of the
 * Modified BSD License as published in various places online or in the
 * COPYING.jmlib file in the package you downloaded.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the
 * Modified BSD License for more details.
 */ 

#ifndef JMLIB_JUGGLEMASTER_H
#define JMLIB_JUGGLEMASTER_H

// The JuggleMaster class
class JuggleMaster : public JMLib {
  friend class JMLib;
private:
  JML_INT32 balln;
  JML_INT32 bm1;
  JML_INT32 arm_x;
  JML_INT32 arm_y;
  JML_INT32 hand_x;
  JML_INT32 hand_y;
  JML_INT32 horCenter, verCenter;
  JML_CHAR styledata[STYLEMAX*4];
  JML_INT32 style_len;
  JML_FLOAT ga;
  JML_FLOAT dwell_ratio;
  JML_FLOAT height_ratio;
  JML_INT32 base;
  JML_INT32 mirror;
  JML_UINT32 time_count;
  JML_INT32 time_period;
  JML_FLOAT cSpeed;
  JML_INT32 beep;
  JML_INT32 syn;
  JML_INT32 hand_on;
  JML_INT32 fpu;
  JML_INT32 tw;
  JML_INT32 aw;
  JML_INT32 pmax;
  JML_INT32 patt[LMAX][MMAX];
  JML_INT32 patts[LMAX];
  JML_INT32 pattw;
  JML_INT32 r[LMAX*2];
  JML_FLOAT smode;
  JML_INT32 high0[BMAX+1];
  JML_FLOAT high[BMAX+1];
  JML_INT32 kw0;       // XR/KW [m]
  JML_CHAR siteswap[JML_MAX_SITELEN]; // The current siteswap
  JML_CHAR pattname[LMAX]; // The name of the current pattern
  JML_INT8 steps[LMAX]; // used to print the site on screen
  JML_CHAR stylename[JML_MAX_NAMELEN]; // The name of the current style
  static JML_CHAR *possible_styles[]; // Contains list of all possible styles

  // Scaling
  JML_INT32 scalingMethod;
  JML_INT32 scaleImageWidth, scaleImageHeight;
  void doCoordTransform();

  // internal methods
  JML_INT32 ctod(JML_CHAR c) EXTRA_SECTION_TWO;
  JML_FLOAT fadd(JML_FLOAT x, JML_INT32 k, JML_FLOAT t)  EXTRA_SECTION_TWO;
  void hand_pos(JML_INT32 c, JML_INT32 h, JML_INT32* x, JML_INT32* z)  EXTRA_SECTION_TWO;
  JML_INT32 juggle(struct ball *)  EXTRA_SECTION_TWO;
  void set_dpm(void)  EXTRA_SECTION_TWO;
  JML_INT32 set_ini(JML_INT32 rr)  EXTRA_SECTION_TWO;
  JML_INT32 set_patt(JML_CHAR* w)  EXTRA_SECTION_TWO;
  void xbitset(void);
  void arm_line(void);
  void applyCorrections(void);
  void doStepcalc(void);
  struct hand handpoly_ex;

  void resetSpin() { for (int i = 0; i < BMAX; i++) { b[i].resetSpin(); } }

#ifdef JUGGLEMASTER_NEW_TIMING
  float CurrentFrameRate;
  float JuggleSpeed;
  unsigned FramesSinceSync;
  unsigned LastSyncTime;
#endif

private:
  JuggleMaster();
public:
  ~JuggleMaster();

  virtual void initialize();
  virtual void shutdown();

  virtual engine_t getType() { return JUGGLING_ENGINE_JUGGLEMASTER; }
  
  virtual JML_BOOL setPattern(JML_CHAR* name, JML_CHAR* site, JML_FLOAT hr = HR_DEF, JML_FLOAT dr = DR_DEF);
  virtual JML_BOOL setPattern(JML_CHAR* site) { return setPattern(site, site); }
  virtual JML_BOOL setStyle(JML_CHAR* name, JML_UINT8 length, JML_INT8* data, JML_INT32 offset = 0);
  virtual JML_BOOL setStyle(JML_CHAR* name);
  virtual JML_CHAR **getStyles(void);
  virtual JML_INT32 numStyles();
  virtual void setPatternDefault(void);
  virtual void setStyleDefault(void);

  virtual void setHR(JML_FLOAT HR);
  virtual JML_FLOAT getHR();
  virtual void setDR(JML_FLOAT DR);
  virtual JML_FLOAT getDR();

  virtual JML_INT32 numBalls(void);
  
  virtual void setScalingMethod(JML_INT32 scalingMethod);

  virtual void startJuggle(void);
  virtual void stopJuggle(void);
  virtual void togglePause(void);
  virtual void setPause(JML_BOOL pauseOn = true);
  virtual JML_INT32  getStatus(void);

  virtual JML_INT32 doJuggleEx(void);
  virtual JML_INT32 doJuggle(void);

  virtual JML_BOOL setWindowSize(JML_INT32 width, JML_INT32 height);
  virtual void     setWindowSizeDefault() { setWindowSize(480, 400); }
  virtual void setMirror(JML_BOOL mir = true);

  virtual JML_CHAR* getSite(void) { return siteswap; }
  virtual JML_CHAR* getPattName(void) { return pattname; }
  virtual JML_CHAR* getStyle(void) { return stylename; }

  virtual JML_INT32 getImageWidth();
  virtual JML_INT32 getImageHeight();

  virtual void speedUp(void);
  virtual void speedDown(void);
  virtual void speedReset(void);
  virtual void setSpeed(float s);
  virtual float speed(void);

  virtual JML_CHAR getSiteposStart(void) {
    if (syn && time_period % 2 == 1) return steps[time_period-1];
    return steps[time_period];
  }

  virtual JML_CHAR getSiteposStop(void) {
    if (syn && time_period % 2 == 0) return steps[time_period+2];
    return steps[time_period+1];
  }

  virtual JML_INT32 getSiteposLen(void) {
    return getSiteposStop() - getSiteposStart();
  }

  virtual JML_INT32 getiterations(void) {
    return (dpm); /* FIXME */
  }
  
  virtual JML_INT32 getBallRadius(void);
	virtual JML_BOOL isValidPattern(const char* patt);
  
#ifdef JUGGLEMASTER_OPENGL_SUPPORT
  virtual void setRenderingMode(rendering_t mode);
  virtual rendering_t getRenderingMode();
  virtual void toggleRenderingMode();
#endif
};

#endif
