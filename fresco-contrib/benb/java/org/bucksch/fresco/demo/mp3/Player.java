package org.bucksch.berlin.mp3;

// GUI
import Warsaw.*;
import org.berlinconsortium.Warsaw.*;
// Player
import javazoom.jlGui.*;

/*
 * This is a simple MP3-Player for Berlin based on jlGui.
 *
 * @author	Ben Bucksch
 * @date	2001-09-03
 * @version	0.1
 * @license GPL (due to jlGui). My code (most of this file) also under BSD.
 */

/*
 * GPL license blabla:
 *-----------------------------------------------------------------------
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *----------------------------------------------------------------------
 */

public class Player implements BasicPlayerListenner
{

    /*-- JavaSound Members --*/
    private static final int    INIT = 0;
    private static final int    OPEN = 1;
    private static final int    PLAY = 2;
    private static final int    PAUSE = 3;
    private static final int    STOP = 4;

	// Data model
  // Access this via the set/get methods
	protected int               mState = INIT;
	//protected long              mCursor = 0;
	protected BasicPlayer       mBackend = null;

	// GUI
	protected ClientContextImpl cc;
	protected BoundedValue mVolumeBvalue;  // coord is in the unit of the backend
	protected BoundedValue mPositionBvalue; // dito
	protected Graphic mTime; // for UpdateTimeDisplay

	public Player(boolean verbose, String[] otherCommandlineArgs)
	{
    System.out.println("Started");

		// Backend
    mBackend = new BasicPlayer(this);
    mBackend.LOG = verbose;
    System.out.println("Created backend");

		// GUI, data
		cc = new ClientContextImpl(otherCommandlineArgs);
    System.out.println("Created clientContext");
		initData();
		createGUI();
    System.out.println("Created GUI");
	}


	///////////////////////////////////////////////////////////
  // Data model

  // fromBackend:
  //  if the backend notified us about this state change, then true
  //  if we want to change the backend state (e.g. the user pressed a button),
  //   then false
  //  in most cases, you want false
	// newCursor/newVolume in the unit the backend uses

	public void setState(int newState, boolean fromBackend)
	{
		if (newState == STOP)
		{
      setCursor(0, fromBackend);
		}

		mState = newState;
	}

	public void setCursor(int newCursor, boolean fromBackend)
	{
		mPositionBvalue.value(newCursor);
	}

	public void setVolume(int newVol, boolean fromBackend)
	{
		mVolumeBvalue.value(newVol);
	}

  // initialises data model attributes
  // needs cc for boundedvalue
  // has to be called before createGUI()
	public void initData()
	{
    int volMin = mBackend.minGain();
    int volMax = mBackend.maxGain();
    int volRange = volMax - volMin;
		int volStep = volRange / 100;
		int volPage = volRange / 10;
		int volCur = mBackend.getGain();
		mVolumeBvalue = cc.command.bvalue(volMin, volMax,
																			volStep, volPage, volCur);

    int posMin = mBackend.minPan();
    int posMax = mBackend.maxPan();
    int posRange = posMax - posMin;
		int posStep = posRange / 100;
		int posPage = posRange / 10;
		int posCur = mBackend.getPan();
		mPositionBvalue = cc.command.bvalue(posMin, posMax,
																				posStep, posPage, posCur);
	}

    /*------------------------------------------*/
    /*--    BasicPlayerListenner Interface    --*/
    /*------------------------------------------*/

	/**
	 * BasicPlayerListenner implementation.
	 * It manages "elapsed time" event.
	 */
    public void updateCursor(int secondsAmount)
    {
    }

	/**
	 * BasicPlayerListenner implementation.
	 * It manages End Of Media event.
	 */
	public void updateMediaState(String state)
	{
		if (state.equals("EOM"))   // End Of Media reached
		{
			setState(STOP, true);
		}
	}

	/**
	 * BasicPlayerListenner implementation.
	 * Dummy
	 */
	public void updateMediaData(byte[] data)
	{
		// Spectrum analyzer could take place here
	}


	///////////////////////////////////////////////////////////
  // GUI

	/*
		Creates the main window with the following layout:

         Time                Volume
          position slider (cursor)
            Command buttons (Play, open etc.)
            File info (not impl.)

	*/
	public void createGUI()
	{
		// Time
		mTime = cc.layout.hbox();   // will be filled by UpdateTimeDisplay
		Graphic time = cc.layout.halign(mTime, 0.75);

		// Volume
		Graphic volumeSlider = cc.widget.slider(mVolumeBvalue, yaxis);

		// Position
		Graphic positionSlider = cc.widget.slider(mPositionBvalue, xaxis);

		// Commands
		Graphic commands = cc.layout.hbox();
		// XXX Create callback |Command|s
		Graphic label;
		label = cc.text.chunk(new Unistring("Play").toBerlin());
		commands.append_graphic(cc.widget.button(null, label));
		label = cc.text.chunk(new Unistring("Stop").toBerlin());
		commands.append_graphic(cc.widget.button(null, label));
		label = cc.text.chunk(new Unistring("Pause").toBerlin());
		commands.append_graphic(cc.widget.button(null, label));

		// Layout
		Graphic timeVol = cc.layout.hbox(); // row: Time, Volume
		timeVol.append_graphic(time);
		timeVol.append_graphic(volumeSlider);
		Graphic mainwindow = cc.layout.vbox();
		mainwindow.append_graphic(timeVol);
		mainwindow.append_graphic(positionSlider);
		mainwindow.append_graphic(commands);

		Graphic fixed = cc.layout.fixed(mainwindow, 4000.);
		cc.desktop.shell(cc.tool.group(background(cc, fixed)));
	}

  protected void UpdateTimeDisplay()
  {
		// Display elapsed time
		// form: m:ss, e.g. 5:54 or 13:05

		int secondsAmount = mPositionBvalue.value();

		int      secondD,second,minuteD,minute;
		int      minutes = (int) (secondsAmount/60);
		int      seconds = (int) (secondsAmount - (minutes*60));
		if (seconds < 10)
		{
			secondD = 0;
			second = seconds;
		}
		else
		{
			secondD = ((int) seconds/10);
			second = ((int) (seconds - (((int) seconds/10))*10));
		}
		if (minutes < 10)
		{
			minuteD = 0;
			minute = minutes;
		}
		else
		{
			minuteD = ((int) minutes/10);
			minute = ((int) (minutes - (((int) minutes/10))*10));
		}
		String output = (minuteD == 0 ? " " : " " + minuteD) +
			              minute + ":" + secondD + second;

		mTime = cc.text.chunk(new Unistring(output).toBerlin());
  }

	// Misc GUI helper functions

	static public Graphic background(ClientContextImpl cc, Graphic foreground)
	{
		FrameSpec fs = new FrameSpec();
		Color col = new Color();
		col.red = 255; col.green = 255; col.blue = 255; col.alpha = 0;
		fs.foreground(col);
		//		Graphic content = cc.tool.rgb(details, 255., 255., 255.);
		return cc.tool.frame(foreground, 1., fs, true);
	}


	///////////////////////////////////////////////////////////
	// Entry point

	static public void main(String[] args)
	{
		Player theApp = new Player(false, args);
		// check for file/URL and open it
	}
}
