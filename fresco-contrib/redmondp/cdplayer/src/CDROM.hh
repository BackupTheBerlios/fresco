#ifndef _CDROM_h
#define _CDROM_h

#include <SDL/SDL.h>

class CDROM
{
public:
    CDROM();
    ~CDROM();
    
    void play();
    void pause();
    void stop();
    void next();
    void prev();
    void eject();
    
    void trackSeek(double position);
    
    int trackCount();
    int trackNumber();

    double trackLength();
    double trackPosition();
    double trackProgress();
private:
    SDL_CD *cdrom;
};

#endif
