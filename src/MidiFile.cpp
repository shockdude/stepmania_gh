/*
 * Skeleton for MidiFile.cpp, functions are subject to change and they likely will, this was started a long time ago
 * Will probably scrap this and start from scratch
 */

#include "global.h"
#include "MidiFile.h"
#include "RageFile.h"
#include "RageLog.h"
#include "RageUtil.h"

void MidiFile::AddParam( const char *buf, int len )
{
   values.back().params.push_back( std::string(buf, len) );
}

void MidiFile::AddValue() /* (no extra charge) */
{
   values.push_back( value_t() );
   values.back().params.reserve( 32 );
}

/**
 TODO: 
 -AddTrack,
 -RenameTrack,
 -AddEvent,
 -CreateEvent,
 ReadBuf
 -GetEvent
 
 Delete old functions
 Modify readfile
 */

void MidiFile::AddTrack( midi_track track )
{
   tracks.push_back(track);
}

void MidiFile::RenameTrack( midi_track track, const char *name )
{
   track.name = name;
}

void MidiFile::AddEvent( midi_track track, midi_event event )
{
   track.events.push_back(event);
}

MidiFile::midi_event MidiFile::CreateEvent( Midi_EventType type, int delta, int total, std::vector<uint8_t> payload )
{
   midi_event newEvent = midi_event();
   newEvent.delta_time = delta;
   newEvent.total_time = total;
   newEvent.type = type;
   newEvent.payload = payload;
   return newEvent;
}

bool MidiFileReadBuf( const char *buf, int len )
{
   /**
    * Need to do:
    * Open file and read byte by byte
    * parse header and such 
    * yadda yadda through the events
    */
   
   /**
    FF 51 = BPM change
       xth byte = length
       take next 3(?) convert to decimal, divide 6000000 by that to find BPM
    FF 58 = TimeSignature Change
       xth bytes are len
       next byte is numerator
       third is denominator (always 4 for RB / GH, but read it for other uses
    */
   return false;
}

// leftover cruft
void MidiFile::ReadBuf( const char *buf, int len, bool bUnescape )
{
   values.reserve( 64 );
   
   bool ReadingValue=false;
   int i = 0;
   char *cProcessed = new char[len];
   int iProcessedLen = -1;
   while( i < len )
   {
      if( i+1 < len && buf[i] == '/' && buf[i+1] == '/' )
      {
         /* Skip a comment entirely; don't copy the comment to the value/parameter */
         do
         {
            i++;
         } while( i < len && buf[i] != '\n' );
         
         continue;
      }
      
      if( ReadingValue && buf[i] == '#' )
      {
         /* Unfortunately, many of these files are missing ;'s.
          * If we get a # when we thought we were inside a value, assume we
          * missed the ;.  Back up and end the value. */
         // Make sure this # is the first non-whitespace character on the line.
         bool FirstChar = true;
         int j = iProcessedLen;
         while( j > 0 && cProcessed[j - 1] != '\r' && cProcessed[j - 1] != '\n' )
         {
            if( cProcessed[j - 1] == ' ' || cProcessed[j - 1] == '\t' )
            {
               --j;
               continue;
            }
            
            FirstChar = false;
            break;
         }
         
         if( !FirstChar )
         {
            /* We're not the first char on a line.  Treat it as if it were a normal character. */
            cProcessed[iProcessedLen++] = buf[i++];
            continue;
         }
         
         /* Skip newlines and whitespace before adding the value. */
         iProcessedLen = j;
         while( iProcessedLen > 0 &&
               ( cProcessed[iProcessedLen - 1] == '\r' || cProcessed[iProcessedLen - 1] == '\n' ||
                cProcessed[iProcessedLen - 1] == ' ' || cProcessed[iProcessedLen - 1] == '\t' ) )
            --iProcessedLen;
         
         AddParam( cProcessed, iProcessedLen );
         iProcessedLen = 0;
         ReadingValue=false;
      }
      
      /* # starts a new value. */
      if( !ReadingValue && buf[i] == '#' )
      {
         AddValue();
         ReadingValue=true;
      }
      
      if( !ReadingValue )
      {
         if( bUnescape && buf[i] == '\\' )
            i += 2;
         else
            ++i;
         continue; /* nothing else is meaningful outside of a value */
      }
      
      /* : and ; end the current param, if any. */
      if( iProcessedLen != -1 && (buf[i] == ':' || buf[i] == ';') )
         AddParam( cProcessed, iProcessedLen );
      
      /* # and : begin new params. */
      if( buf[i] == '#' || buf[i] == ':' )
      {
         ++i;
         iProcessedLen = 0;
         continue;
      }
      
      /* ; ends the current value. */
      if( buf[i] == ';' )
      {
         ReadingValue=false;
         ++i;
         continue;
      }
      
      /* We've gone through all the control characters.  All that is left is either an escaped character,
       * ie \#, \\, \:, etc., or a regular character. */
      if( bUnescape && i < len && buf[i] == '\\' )
         ++i;
      if( i < len )
      {
         cProcessed[iProcessedLen++] = buf[i++];
      }
   }
   
   /* Add any unterminated value at the very end. */
   if( ReadingValue )
      AddParam( cProcessed, iProcessedLen );
   
   delete [] cProcessed;
}

// returns true if successful, false otherwise
bool MidiFile::ReadFile( std::string sNewPath, bool bUnescape )
{
   error = "";
   
   RageFile f;
   /* Open a file. */
   if( !f.Open( sNewPath ) )
   {
      error = f.GetError();
      return false;
   }
   
   // allocate a string to hold the file
   std::string FileString;
   FileString.reserve( f.GetFileSize() );
   
   int iBytesRead = f.Read( FileString );
   if( iBytesRead == -1 )
   {
      error = f.GetError();
      return false;
   }
   
   ReadBuf( FileString.c_str(), iBytesRead, bUnescape );
   
   return true;
}

void MidiFile::ReadFromString( const std::string &sString, bool bUnescape )
{
   ReadBuf( sString.c_str(), sString.size(), bUnescape );
}

std::string MidiFile::GetParam(unsigned val, unsigned par) const
{
   if( val >= GetNumValues() || par >= GetNumParams(val) )
      return std::string();
   
   return values[val].params[par];
}

MidiFile::midi_event MidiFile::GetEvent( unsigned track, unsigned evt ) const
{
   return tracks[track].events[evt];
}
