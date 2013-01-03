/****************************************************************************
 * Copyright (C) 2012 FIX94
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#include <cstdio>
#include <algorithm>
#include "MusicPlayer.hpp"
#include "SoundHandler.hpp"
#include "gui/text.hpp"
#include "gecko/gecko.hpp"

#define	MUSIC_DEPTH 10
Musicplayer MusicPlayer;

static vector<string> FileNames;
static vector<string>::const_iterator CurrentFileName;

void Musicplayer::Cleanup()
{
	Stop();
	DisplayTime = 0;
	CurrentPosition = 0;
	MusicChanged = false;
	MusicStopped = true;
	FileNames.clear();
}

void Musicplayer::Init() 
{
	Cleanup();
	SetVolume(0);
	MusicFile.SetVoice(0);
}

void Musicplayer::SetMaxVolume(u8 volume)
{
	Volume = volume;
	SetVolume(volume);
}

void Musicplayer::SetVolume(u8 volume)
{
	CurrentVolume = volume;
	MusicFile.SetVolume(CurrentVolume);
}

void Musicplayer::Previous()
{
	if(FileNames.empty() || PosFromPrevFile())
		return;

	if(CurrentFileName == FileNames.begin())
		CurrentFileName = FileNames.end();
	--CurrentFileName;
	LoadCurrentFile();
}

void Musicplayer::Next()
{
	if(FileNames.empty() || PosFromPrevFile())
		return;

	++CurrentFileName;
	if(CurrentFileName == FileNames.end())
		CurrentFileName = FileNames.begin();
	LoadCurrentFile();
}

bool Musicplayer::PosFromPrevFile()
{
	if(!CurrentPosition)
		return false;

	MusicFile.Load((*CurrentFileName).c_str());
	SoundHandle.Decoder(MusicFile.GetVoice())->Seek(CurrentPosition);
	SetVolume(CurrentVolume);
	MusicFile.Play();
	CurrentPosition = 0;
	MusicStopped = false;
	MusicChanged = false;
	return true;
}

void Musicplayer::Stop()
{
	if(!MusicFile.IsPlaying())
		return;
	MusicFile.Pause();
	CurrentPosition = SoundHandle.Decoder(MusicFile.GetVoice())->Tell();
	MusicFile.FreeMemory();
	MusicStopped = true;
}

void Musicplayer::Tick(bool attenuate)
{
	if(FileNames.empty())
		return;
	if(!attenuate && CurrentVolume < Volume)
		SetVolume(CurrentVolume + FadeRate > Volume ? Volume : CurrentVolume + FadeRate);
	else if(attenuate && CurrentVolume > 0)
		SetVolume(CurrentVolume - FadeRate < 0 ? 0 : CurrentVolume - FadeRate);
	if(!attenuate && !MusicFile.IsPlaying())
		Next();
}

void Musicplayer::LoadCurrentFile()
{
	MusicFile.Load((*CurrentFileName).c_str());
	SetVolume(CurrentVolume);
	MusicFile.Play();
	CurrentPosition = 0;
	MusicStopped = false;
	MusicChanged = true;
}

/* For our GUI */
wstringEx Musicplayer::GetFileName()
{
	wstringEx CurrentFile;
	string CurrentFileStr((*CurrentFileName).begin()+(*CurrentFileName).find_last_of('/')+1, 
			(*CurrentFileName).begin()+(*CurrentFileName).find_last_of('.'));
	CurrentFile.fromUTF8(CurrentFileStr.c_str());
	return CurrentFile;
}

bool Musicplayer::SongChanged()
{
	if(!MusicChanged)
		return false;
	MusicChanged = false;
	return true;
}
