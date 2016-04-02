#include "global.h"
#include "NotesLoaderCHART.h"
#include "RageUtil_CharConversions.h"
#include "RageFile.h"
#include "TimingData.h"
#include "RageUtil.h"
#include "RageLog.h"
#include "BackgroundUtil.h"
#include "NoteData.h"
#include "Song.h"
#include "Steps.h"
#include "GameManager.h"
#include "Difficulty.h"
#include "NotesWriterSSC.h"
#include "NotesLoaderSSC.h"
#include <string>
#include <sstream>

/*
 Alright, just a fair warning, this whole thing is complete garbage. I tried making a notesloader the legit way, but it never
 worked, so instead it rewrites it as an SSC file then calls the SSC loader
 This isn't a loader at all, this is just a rewriter because I'm fucking clueless as to how this is supposed to work.
 */


// I'm not even going to pretend that the following wasn't copy/pasted from MsdFile.cpp, I needed to parse this somehow
// Well it was originally, now it's just a mess
void ReadBuf( const char *buf, int len, Song &outSong, Steps &outSteps, bool parseSongInfo )
{
   bool bDontReadThis = false;
   int iMode = 0;
   int iPrevNoteMark = -1;
   int iPrevNoteTrack = -1;
   int iPrevNoteLength = -1;
   bool bPrevNoteIsHOPO = false;
   // 192 is default resolution per beat in GH
   int resolution = 192;
   bool readingNotes = false;
   vector<RString> headerInfo(3);
   // NoteData to store stuff that's being parsed
   NoteData notedata;
   // TimingData to store other stuff being parsed
   TimingData timing;
   // Pointer to Steps that may or may not need to be added, depending on if we're parsing for a Song or not
   Steps* pNewNotes = NULL;
   // Dunno if this will actually do anything, but it seems to be defaults put into SSC files for some reason?
   timing.AddSegment(ComboSegment(0.0,1,1));
   timing.AddSegment(ScrollSegment(0.0,1.0));
   // Set notedata tracks
   notedata.SetNumTracks(6);
   // Screw efficiency, just gonna parse this word by word like I was gonna do with Python
   RString bufStr(buf);
   std::istringstream iss(bufStr);
   RString line;
   // Parse line-by-line
   while( std::getline(iss,line) )
   {
      // Remove tabs
      line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
      vector<RString> vsWords;
      std::istringstream lineIss(line);
      // Separate by spaces
      copy( istream_iterator<string>(lineIss), istream_iterator<string>(), back_inserter(vsWords) );
      int i = 0;
      int numWords = vsWords.size();
      // Don't need to parse through every word, since each of these if blocks parse the entire line
      
      /**
       Change the mode once you hit a section tag. Until the actual notes, the tags should always be in the order:
       Song, SyncTrack, Events, then comes the notes which are tagged by difficulty and game mode
       */
      if( vsWords[i].at(0) == '[' ) {
         if( iMode < 4 ) ++iMode;
         // Labels actually have a use! how 'bout that!
         //if( iMode == 3 ) bDontReadThis = true;
         bDontReadThis = false;
         if( iMode == 4 ) {
            // If it's not single mode, we don't care about it
            if( vsWords[i].find("Single") != std::string::npos ) {
               // reset these between songs
               iPrevNoteMark = -1;
               iPrevNoteTrack = -1;
               iPrevNoteLength = -1;
               // If we're parsing a song, add the previous notes parsed, then get space for new steps
               if( parseSongInfo ) {
                  if( readingNotes ) {
                     pNewNotes->m_Timing = timing;
                     pNewNotes->SetNoteData( notedata );
                     pNewNotes->TidyUpData();
                     outSong.AddSteps( pNewNotes );
                     // Reset notedata for new set of steps
                     notedata = NoteData();
                     notedata.SetNumTracks(6);
                  } else {
                     outSong.m_SongTiming = timing;
                  }
                  pNewNotes = outSong.CreateSteps();
                  pNewNotes->SetChartStyle("Guitar");
                  pNewNotes->SetCredit( headerInfo[1] );
                  pNewNotes->SetDescription( headerInfo[1] );
                  pNewNotes->SetMusicFile( headerInfo[2] );
                  // Charts only load for guitar mode... for now.
                  pNewNotes->m_StepsType = StepsType_guitar_solo;
               }
               if( !readingNotes ) readingNotes = true;
               if( vsWords[i].find("Expert") != std::string::npos ) {
                  // Expert == Challenge
                  if( !parseSongInfo && outSteps.GetDifficulty() != Difficulty_Challenge ) {
                     // This is the incorrect difficulty for the desired steps, continue to the next noteinfo tag
                     bDontReadThis = true;
                     continue;
                  }
                  if( parseSongInfo ) {
                     pNewNotes->SetDifficulty( Difficulty_Challenge );
                     pNewNotes->SetChartName( headerInfo[0]+" - Expert" );
                     continue;
                  }
               } else if( vsWords[i].find("Hard") != std::string::npos ) {
                  if( !parseSongInfo && outSteps.GetDifficulty() != Difficulty_Hard ) {
                     bDontReadThis = true;
                     continue;
                  }
                  if( parseSongInfo ) {
                     pNewNotes->SetDifficulty( Difficulty_Hard );
                     pNewNotes->SetChartName( headerInfo[0]+" - Hard" );
                     continue;
                  }
               } else if( vsWords[i].find("Medium") != std::string::npos ) {
                  if( !parseSongInfo && outSteps.GetDifficulty() != Difficulty_Medium ) {
                     bDontReadThis = true;
                     continue;
                  }
                  if( parseSongInfo ) {
                     pNewNotes->SetDifficulty( Difficulty_Medium );
                     pNewNotes->SetChartName( headerInfo[0]+" - Medium" );
                     continue;
                  }
               } else if( vsWords[i].find("Easy") != std::string::npos ) {
                  if( !parseSongInfo && outSteps.GetDifficulty() != Difficulty_Easy ) {
                     bDontReadThis = true;
                     continue;
                  }
                  if( parseSongInfo ) {
                     pNewNotes->SetDifficulty( Difficulty_Easy );
                     pNewNotes->SetChartName( headerInfo[0]+" - Easy" );
                     continue;
                  }
               }
               // Custom Guitar Hero charts have no beginner difficulty, this was added starting in GH World Tour
            } else {
               bDontReadThis = true;
               continue;
            }
         } else {
            continue;
         }
      }
      
      /* Curly braces are just filler */
      if( !bDontReadThis && vsWords[i].at(0) != '{' && vsWords[i].at(0) != '}' )
      {
         if( iMode == 1 ) {
            // Parse song information
            // Separation by words also means that titles, artists and other info is likely broken up, stitch it back together
            int j = 2;
            RString wholeString = "";
            while(j < numWords) {
               if(j>2) wholeString += " ";
               wholeString += vsWords[j];
               j++;
            }
            if( !vsWords[i].compare("Name") )
            {
               RString mainTitle = wholeString.substr(1, wholeString.size()-2);
               headerInfo[0] = mainTitle;
               if( parseSongInfo ) outSong.m_sMainTitle = mainTitle;
            } else if( !vsWords[i].compare("Artist") )
            {
               if( parseSongInfo ) outSong.m_sArtist = wholeString.substr(1, wholeString.size()-2);
            } else if( !vsWords[i].compare("Charter") )
            {
               headerInfo[1] = wholeString.substr(1, wholeString.size()-2);
            } else if( !vsWords[i].compare("Offset") )
            {
               timing.m_fBeat0OffsetInSeconds = -1 * atof(vsWords[i+2].c_str());
            } else if( !vsWords[i].compare("Resolution") )
            {
               /* Here's the thing, Guitar Hero has a default resolution of 192 'ticks' per beat. Comparatively, Stepmania uses
                * 192 ticks per MEASURE as a maximum. This causes a lot of problems. If the resolution is >48, just make it 48
                * But only for setting the tickcount segment, still need the original resolution for note and timing parsing
                */
               resolution = atoi(vsWords[i+2].c_str());
               
               if( resolution <= 48 ) timing.AddSegment(TickcountSegment(0, resolution));
               else timing.AddSegment(TickcountSegment(0, 48));
            } else if( !vsWords[i].compare("Difficulty") )
            {
               /* Uh, skip this for now, causes Stepmania to crash...
               if( parseSongInfo ) pNewNotes->SetMeter(atoi(vsWords[i+2].c_str()) + 1);
               else outSteps->SetMeter(atoi(vsWords[i+2].c_str()) + 1);
                */
            } else if( !vsWords[i].compare("PreviewStart") )
            {
               if( parseSongInfo ) outSong.m_fMusicSampleStartSeconds = atof(vsWords[i+2].c_str());
            } else if( !vsWords[i].compare("PreviewEnd") )
            {
               if( parseSongInfo )
               {
                  // Default to 12 second length, just seems like the default in StepMania
                  float estSampleSec = atof(vsWords[i+2].c_str()) - outSong.m_fMusicSampleStartSeconds;
                  if( estSampleSec <= 12 ) outSong.m_fMusicSampleStartSeconds = 12;
                  else outSong.m_fMusicSampleLengthSeconds = estSampleSec;
               }
            } else if( !vsWords[i].compare("MusicStream") )
            {
               string songFile = wholeString.substr(1, wholeString.size()-2);
               if( parseSongInfo ) outSong.m_sMusicFile = songFile;
               else outSteps.SetMusicFile( songFile );
               headerInfo[2] = songFile;
            }
         } else if( iMode == 2 ) {
            // Parse BPM and time signature changes
            // Steps and songs can have different timing data. Why?
            int startMark = atoi(vsWords[i].c_str());
            float newBPM = atof(vsWords[i+3].c_str()) / 1000;
            if( !vsWords[i+2].compare( "B" ) )
            {
               if( startMark == 0 ) timing.AddSegment( BPMSegment(0, newBPM ));
               else timing.SetBPMAtBeat((float)startMark/resolution, newBPM);
            } else if( !vsWords[i+2].compare( "TS" ) )
            {
               // Fun fact, Guitar Hero songs are always in a time signature of x/4
               timing.AddSegment( TimeSignatureSegment(BeatToNoteRow(atof(vsWords[i].c_str()) / resolution), atoi(vsWords[i+3].c_str()), 4) );
            }
         } else if( iMode == 3 ) {
            // Parse section labels
            int end = vsWords.size()-1;
            RString sectionTitle = vsWords[end].substr(0, vsWords[end].size()-1);
            timing.AddSegment(LabelSegment(BeatToNoteRow(atof(vsWords[i].c_str())/resolution),sectionTitle));
         } else if( iMode == 4 ) {
            // Parse the notedata
            // Some other markers for star power and such are thrown in here, ignore them
            if( !vsWords[i+2].compare( "N" ) )
            {
               int iNoteTrack = atoi(vsWords[i+3].c_str());
               /* Some custom tracks use column 5 to mark HOPOs, which is literally useless since they're always in place to
                * be automatically converted to HOPOs. Skip over these otherwise open strums will be thrown everywhere
                */
               if( iNoteTrack == 5 ) continue;
               /* A note on sustained notes, Guitar Hero likes to have holds end exactly on beats, unfortunately this means that
                * sometimes a hold note can overlap into the next hold if they're in the same track. Need to check for this and
                * correct it by shortening the first hold slightly (1/8 resolution)
                */
               int iNoteMark = atoi(vsWords[i].c_str());
               int iNoteLength = atoi(vsWords[i+4].c_str());
               
               if( iNoteTrack == iPrevNoteTrack && iPrevNoteLength + iPrevNoteMark >= iNoteMark ) {
                  notedata.SetTapNote(iPrevNoteTrack, BeatToNoteRow((float)iPrevNoteMark/resolution), TAP_EMPTY);
                  iPrevNoteLength = iNoteMark - iPrevNoteMark - (resolution / 8);
                  if( !bPrevNoteIsHOPO ) {
                     if( iPrevNoteLength > 0 )
                        notedata.AddHoldNote(iPrevNoteTrack, BeatToNoteRow((float)iPrevNoteMark/resolution),
                                       BeatToNoteRow((float)(iPrevNoteMark+iPrevNoteLength)/resolution), TAP_ORIGINAL_GEM_HOLD);
                     else
                        notedata.SetTapNote(iPrevNoteTrack, BeatToNoteRow((float)iPrevNoteMark/resolution), TAP_ORIGINAL_GEM);
                  } else {
                     if( iPrevNoteLength > 0 )
                        notedata.AddHoldNote(iPrevNoteTrack, BeatToNoteRow((float)iPrevNoteMark/resolution),
                                       BeatToNoteRow((float)(iPrevNoteMark+iPrevNoteLength)/resolution), TAP_ORIGINAL_HOPO_HOLD);
                     else
                        notedata.SetTapNote(iPrevNoteTrack, BeatToNoteRow((float)iPrevNoteMark/resolution), TAP_ORIGINAL_HOPO);
                  }
               }
               
               // If this note and the previous note are on the same beat and the previous note was a HOPO,
               // Change it to a normal gem
               if( bPrevNoteIsHOPO && (iNoteMark == iPrevNoteMark) )
               {
                  if( iPrevNoteLength ) // Previous note was a hold
                  {
                     notedata.AddHoldNote(iPrevNoteTrack, BeatToNoteRow((float)iPrevNoteMark/resolution),
                              BeatToNoteRow((float)(iPrevNoteMark+iPrevNoteLength)/resolution), TAP_ORIGINAL_GEM_HOLD);
                  } else
                  {
                     notedata.SetTapNote(iPrevNoteTrack, BeatToNoteRow((float)iPrevNoteMark/resolution), TAP_ORIGINAL_GEM);
                  }
               }
               
               /* If the difference between this note and the last is <= a 16th note (and > 0), and they're
                * on different tracks, AND this isn't the first note, make this a HOPO
                * ...actually, the chart2mid2chart converter thingamabob has some rounding issues, so add 1 to the 16th note
                * check just to catch all the things that should be HOPOs
                */
               if((iNoteMark - iPrevNoteMark - 1 <= resolution / 4) && (iNoteMark != iPrevNoteMark) && (iNoteTrack != iPrevNoteTrack)
                  && (iPrevNoteMark != -1))
               {
                  if( iNoteLength )
                  {
                     notedata.AddHoldNote(iNoteTrack, BeatToNoteRow((float)iNoteMark/resolution),
                                 BeatToNoteRow((float)(iNoteMark+iNoteLength)/resolution), TAP_ORIGINAL_HOPO_HOLD);
                  } else
                  {
                     notedata.SetTapNote(iNoteTrack, BeatToNoteRow((float)iNoteMark/resolution), TAP_ORIGINAL_HOPO);
                  }
                  bPrevNoteIsHOPO = true;
               }
               
               // Otherwise, plop a Gem in there
               else
               {
                  if( iNoteLength )
                  {
                     notedata.AddHoldNote(iNoteTrack, BeatToNoteRow((float)iNoteMark/resolution),
                                 BeatToNoteRow((float)(iNoteMark+iNoteLength)/resolution), TAP_ORIGINAL_GEM_HOLD);
                  } else
                  {
                     notedata.SetTapNote(iNoteTrack, BeatToNoteRow((float)iNoteMark/resolution),TAP_ORIGINAL_GEM);
                  }
                  bPrevNoteIsHOPO = false;
               }
               
               iPrevNoteMark = iNoteMark;
               iPrevNoteTrack = iNoteTrack;
               iPrevNoteLength = iNoteLength;
            }
         }
      }
   }
   // If we reached the end of the file while still reading notes, add them to the song, if we're parsing that
   if( parseSongInfo && readingNotes ) {
      pNewNotes->m_Timing = timing;
      pNewNotes->SetNoteData( notedata );
      pNewNotes->TidyUpData();
      outSong.AddSteps(pNewNotes);
   }
   if( !parseSongInfo )
   {
      outSteps.m_Timing = timing;
      outSteps.SetNoteData( notedata );
      outSteps.TidyUpData();
   }
}

// Returns true if successful, false otherwise
bool ReadFile( RString sNewPath, Song &outSong, Steps &outSteps, bool parseSongInfo )
{
   RageFile f;
   /* Open a file. */
   if( !f.Open( sNewPath ) )return false;
   
   // allocate a string to hold the file
   RString FileString;
   FileString.reserve( f.GetFileSize() );
   
   int iBytesRead = f.Read( FileString );
   if( iBytesRead == -1 )return false;
   
   Song tempSong;
   Steps tempSteps = NULL;
   if( parseSongInfo ) tempSong = Song();
   else tempSteps = Steps(outSteps);
   
   RString sscFile = sNewPath.substr(0,sNewPath.length()-5) + "ssc";
   RString dir = sNewPath.substr(0,sNewPath.find_last_of("/\\")+1);
   
   ReadBuf( FileString.c_str(), iBytesRead, tempSong, tempSteps, parseSongInfo );
   
   /* This totally works, but it doesn't work if I just call ReadBuf with the output files */
   NotesWriterSSC::Write(sscFile, tempSong, tempSong.GetAllSteps(), false);
   
   SSCLoader loaderSSC;
   
   if( parseSongInfo ) {
      return loaderSSC.LoadFromDir(dir,outSong);
   } else {
      return loaderSSC.LoadNoteDataFromSimfile(sNewPath, outSteps);
   }
}

void CHARTLoader::GetApplicableFiles( const RString &sPath, vector<RString> &out )
{
   GetDirListing( sPath + RString("*.chart"), out );
}

bool CHARTLoader::LoadFromDir( const RString &sDir, Song &out ) {
   LOG->Trace( "CHARTLoader::LoadFromDir(%s)", sDir.c_str() );
   
   vector<RString> arrayCHARTFileNames;
   GetDirListing( sDir + RString("*.chart"), arrayCHARTFileNames );
   
   // We shouldn't have been called to begin with if there were no CHARTs.
   ASSERT( arrayCHARTFileNames.size() != 0 );
   
   RString dir = out.GetSongDir();
   
   Steps *tempSteps = NULL;
   
   // Only need to use the first file, since there should only be 1
   return ReadFile( dir+arrayCHARTFileNames[0], out, *tempSteps, true );
}

bool CHARTLoader::LoadNoteDataFromSimfile( const RString & cachePath, Steps &out ) {
   Song *tempSong = NULL;
   // This is simple since the path is already given to us
   return ReadFile(cachePath, *tempSong, out, false);
}
