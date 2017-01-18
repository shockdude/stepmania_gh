/* Parses .mid files and makes them easy to handle (no plans for writing yet) */

#ifndef MidiFile_h
#define MidiFile_h

/* Skeleton, will probably be scrapped.
 */

class MidiFile
{
public:
   /** Midi event types */
   enum Midi_EventType
   {
      Midi_TitleEvent,     // Title for the track, usually the first event
      Midi_NoteOnEvent,    // Beginning of a note, includes pitch info
      Midi_NoteOffEvent,      // End of a note, also with pitch info
      Midi_BPMEvent,       // Change in tempo
      Midi_TSEvent,        // Change in time signature
      Midi_LyricEvent,     // Lyrics for the song, often used in vocal/karaoke track
      Midi_OtherEvent,     // Catch all for unimportant events
   };
   
   /**
    * @brief The list of params found in the files.
    *
    * Note that &#35;param:param:param:param; is one whole value. */
   struct value_t
   {
      /** @brief The list of parameters. */
      std::vector<std::string> params;
      /** @brief Set up the parameters with default values. */
      value_t(): params() {}
      
      /**
       * @brief Access the proper parameter.
       * @param i the index.
       * @return the proper parameter.
       */
      std::string operator[]( unsigned i ) const { if( i >= params.size() ) return std::string(); return params[i]; }
   };
   
   /** @brief A single event and all its related information */
   struct midi_event
   {
      /* How the payloads work based on midi event type:
       * TitleEvent    - chars contains the parsed title
       * NoteOnEvent   - integers contain key number, velocity
       * NoteOffEvent  - integers contain key number, velocity
       * BPMEvent      - integers contain numerator, denominator (divide to get BPM)
       * TSEvent       - integers contain numerator, denominator
       * LyricEvent    - chars contains the parsed lyric(s)
       * OtherEvent    - only payload has data
       */
      
      // total time, in ticks, from the start of the song
      int total_time;
      
      // number of ticks difference from previous event to this event
      int delta_time;
      
      // what type of event this is
      Midi_EventType type;
      
      // the MIDI code for the event type, incase it's not yet in EventTypes
      char eventCode;
      
      // raw payload for the event, just in case, may remove later
      std::vector<uint8_t> payload;
      
      // vector of integers parsed from payload, applies to most events
      std::vector<int> integers;
      
      // string of characters parsed from payload, for title and lyric events
      std::string chars;
   };
   
   /** @brief All events and information for one track in a midi file */
   struct midi_track
   {
      // name of the track, auto assigned or read from events
      std::string name;
      
      // all events in this track. By the nature of midi files, this is already
      // sorted from first to last
      std::vector<midi_event> events;
   };
   
   MidiFile(): values(), tracks(), error("") {}
   
   /** @brief Remove the MidiFile. */
   virtual ~MidiFile() { }
   
   /**
    * @brief Attempt to read a mid file.
    * @param sFilePath the path to the file.
    * @param bUnescape a flag to see if we need to unescape values.
    * @return its success or failure.
    */
   bool ReadFile( std::string sFilePath, bool bUnescape );
   /**
    * @brief Attempt to read a mid file.
    * @param sString the path to the file.
    * @param bUnescape a flag to see if we need to unescape values.
    * @return its success or failure.
    */
   void ReadFromString( const std::string &sString, bool bUnescape );
   
   /**
    * @brief Should an error take place, have an easy place to get it.
    * @return the current error. */
   std::string GetError() const { return error; }
   
   //************************ Midi file stuff ****************************
   /**
    * @brief Retrieve the total number of tracks.
    * @return the nmber of tracks. */
   unsigned GetNumTracks() const { return tracks.size(); }
   /**
    * @brief Get the number of events for the current track.
    * @param track the current track index.
    * @return the number of events.
    */
   unsigned GetNumEvents( unsigned track ) const { if( track >= GetNumTracks() ) return 0; return tracks[track].events.size(); }
   /**
    * @brief Get the specified track.
    * @param track the current track index.
    * @return The specified track.
    */
   const midi_track &GetTrack( unsigned track ) const { ASSERT(track < GetNumTracks()); return tracks[track]; }
   /**
    * @brief Retrieve the specified event.
    * @param track the current track index.
    * @param evt the current event index.
    * @return the event in question.
    */
   midi_event GetEvent( unsigned track, unsigned evt ) const;
   
   /** @brief The number of ticks per beat, 0 if midi uses frames/sec. */
   unsigned TicksPerBeat;
   /** @brief The delta time units between frames, 0 if midi uses ticks/beat */
   unsigned TicksPerFrame;
   /** @brief The number of frames per second, 0 if midi uses ticks/beat */
   unsigned FramesPerSecond;
   //*********************************************************************
   
   /**
    * @brief Retrieve the number of values for each tag.
    * @return the nmber of values. */
   unsigned GetNumValues() const { return values.size(); }
   /**
    * @brief Get the number of parameters for the current index.
    * @param val the current value index.
    * @return the number of params.
    */
   unsigned GetNumParams( unsigned val ) const { if( val >= GetNumValues() ) return 0; return values[val].params.size(); }
   /**
    * @brief Get the specified value.
    * @param val the current value index.
    * @return The specified value.
    */
   const value_t &GetValue( unsigned val ) const { ASSERT(val < GetNumValues()); return values[val]; }
   /**
    * @brief Retrieve the specified parameter.
    * @param val the current value index.
    * @param par the current parameter index.
    * @return the parameter in question.
    */
   std::string GetParam( unsigned val, unsigned par ) const;
   
   
private:
   /**
    * @brief Attempt to read an MSD file from the buffer.
    * @param buf the buffer containing the MSD file.
    * @param len the length of the buffer.
    * @param bUnescape a flag to see if we need to unescape values.
    */
   void ReadBuf( const char *buf, int len, bool bUnescape );
   /**
    * @brief Add a new parameter.
    * @param buf the new parameter.
    * @param len the length of the new parameter.
    */
   void AddParam( const char *buf, int len );
   /**
    * @brief Add a new value.
    */
   void AddValue();
   //************************** MIDI STUFF *******************************
   /**
    * @brief Create a new track
    * @param track the new track to append
    */
   void AddTrack( midi_track track );
   /**
    * @brief Rename a track
    * @param track the track to rename
    * @param name the new name for the track
    */
   void RenameTrack( midi_track track, const char *name );
   /**
    * @brief Add an event to the track, it's expected to be in order
    * @param track the track to add the event to
    * @param event the event to add
    */
   void AddEvent( midi_track track, midi_event event );
   /**
    * @brief Create an event
    * @param type the type of event
    * @param delta the delta ticks from previous event
    * @param total the total ticks from song start
    * @param payload the information that the event holds
    * @return the newly created event
    */
   midi_event CreateEvent( Midi_EventType type, int delta, int total, std::vector<uint8_t> payload );
   /**
    * @brief Attempt to read a MIDI file from the buffer
    * @param buf the buffer containing the midi file
    * @param len the length of the buffer
    * @return whether the read was successful or not
    */
   bool ReadBuf( const char *buf, int len );
   
   /** @brief The list of values. */
   std::vector<value_t> values;
   /** @brief The list of tracks. */
   std::vector<midi_track> tracks;
   /** @brief The error string. */
   std::string error;
};

#endif /* MidiFile_h */
