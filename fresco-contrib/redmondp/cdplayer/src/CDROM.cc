#include <exception>
#include <iostream>
#include "CDROM.hh"

using namespace std;

CDROM::CDROM()
{
    if (SDL_Init(SDL_INIT_CDROM) < 0)
	throw std::exception();
    
    cdrom = SDL_CDOpen(0);
    if (cdrom == NULL)
	throw std::exception();

    SDL_CDStatus(cdrom);
}

CDROM::~CDROM()
{
    stop();
    SDL_CDClose(cdrom);
    SDL_Quit();    
}

void CDROM::play()
{
    CDstatus status = SDL_CDStatus(cdrom);

    if (status == CD_PLAYING)
	return;    
    if (status == CD_PAUSED)
	SDL_CDResume(cdrom);
//    if (status == CD_STOPPED)
	SDL_CDPlayTracks(cdrom, cdrom->cur_track, 0, 
	    cdrom->numtracks - cdrom->cur_track - 1, 
	    cdrom->track[cdrom->numtracks - 1].length);
}

void CDROM::stop()
{
    SDL_CDStop(cdrom);
}

void CDROM::pause()
{
    SDL_CDPause(cdrom);
}

void CDROM::next()
{
    int track;
    CDstatus status = SDL_CDStatus(cdrom);
    
    track = (cdrom->cur_track + 1) % cdrom->numtracks;
    cout << "skipping to track " << track << endl;
    stop();
    SDL_CDPlayTracks(cdrom, track, 0, 
	cdrom->numtracks - track - 1, 
	cdrom->track[cdrom->numtracks - 1].length);
}

void CDROM::prev()
{
    int track;
    CDstatus status = SDL_CDStatus(cdrom);
    
    track = (cdrom->cur_track - 1);
    cout << "skipping to track " << track << endl;
    if (track < 0)
	track = cdrom->numtracks - 1;
    SDL_CDPlayTracks(cdrom, track, 0, 
	cdrom->numtracks - track - 1, 
	cdrom->track[cdrom->numtracks - 1].length);
}

void CDROM::eject()
{
    stop();
    SDL_CDEject(cdrom);
}

int CDROM::trackNumber()
{
    SDL_CDStatus(cdrom);
    return cdrom->cur_track + 1;
}

int CDROM::trackCount()
{
    SDL_CDStatus(cdrom);
    return cdrom->numtracks;
}

void CDROM::trackSeek(double position)
{
    if (position < 0.0) position = 0.0;
    if (position > 1.0) position = 1.0;
    
    SDL_CDPlayTracks(cdrom, cdrom->cur_track, 
	int(position * cdrom->track[cdrom->cur_track].length), 
	cdrom->numtracks - cdrom->cur_track - 1, 
	cdrom->track[cdrom->numtracks - 1].length);
    
}

// return length of track in seconds
double CDROM::trackLength()
{
    CDstatus status = SDL_CDStatus(cdrom);
    
    return (double)cdrom->track[cdrom->cur_track].length / CD_FPS;
}

// return position in track in seconds
double CDROM::trackPosition()
{
    CDstatus status = SDL_CDStatus(cdrom);

    double pos = (double)cdrom->cur_frame / CD_FPS;
    return (pos>0.0)?pos:0.0;
}

// return the percentage played of the current track
double CDROM::trackProgress()
{
    CDstatus status = SDL_CDStatus(cdrom);
    
    if ((status == CD_PLAYING)||(status == CD_PAUSED))
	if (trackPosition() != 0.0)
	    return trackPosition() / trackLength();    

    return 0.0;
}
/*
int main(int argc, char *argv[])
{
    CDROM *cdrom = new CDROM;
    int opt = 0;
    do {
	cout << "1: play" << endl;
	cout << "2: pause" << endl;
	cout << "3: stop" << endl;
	cout << "4: next" << endl;
	cout << "5: prev" << endl;
	cout << "6: eject" << endl;
	cout << cdrom->trackNumber() << "/" << cdrom->trackCount() << 
	    ": " << cdrom->trackPosition() << "/" <<
	    cdrom->trackLength() << 
	    "[" << cdrom->trackProgress() <<"%]"<< endl;

	cin >> opt;
	switch (opt) {
	
	case 1: cdrom->play();break;
	case 2: cdrom->pause();break;
	case 3: cdrom->stop();break;
	case 4: cdrom->next();break;
	case 5: cdrom->prev();break;
	case 6: cdrom->eject();break;
	}
	
    } while (opt);
    delete cdrom;
}*/
