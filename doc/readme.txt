Collected behaviour from prodding mciSendString with a stick:
- MCI auto-opens device when told to PLAY. It will also autoclose after told to STOP.
- If MCI is manually opened, it must be manually closed.
- PLAY and STOP commands will work regardless of when it was opened.
- Requests to open already open MCI are denied. Likewise for already closed.
- Different applications can open and close MCI independent of eachother.
- Default time format is MSF. Closing and reopening device resets it.
- Playback cannot start earlier than 2s.
- If told to PLAY from time longer than total time of tracks, it will jump to begining of last track.
- PLAY pairs with STOP, RESUME with PAUSE. Cannot be mixed.
- The parser is case insensitive.

What ECMA-130(RedBook) standard says:
- The minimum addressable unit is the frame(called sector in the doc).
- There are 75 frames in one second, and 588 samples in each frame.
- Track 00 is special case, unusable.
- There's the pre-gap and the postgap.
- Pregap has interval-1 of at least 1s and interval-2 of at least 2s.
- Postgap is at least 2s.
- There are 3 areas: Lead-in, User Area, Lead-out.
- TODO...

Other things:
- Apparently, win95-era games did stupid things(like states shared between threads) with then-nonfunctional various parts of the API. Watch out for RESUME.
- Oops! Winmm is treated as "known dll" in win9x and is not loaded from app folder. Executable must be patched.

Questions
- Do data tracks in the middle of CD affect total track count? What about playback times of tracks after?





symbol list:
* supported, we handle
! supported, special case
> forward to real MCI

List of MCI commands we support:
*CAPABILITY
*CLOSE
*INFO
*OPEN
*PAUSE
*PLAY
!RESUME         
*SEEK
*SET
*STATUS
*STOP
*SYSINFO

List of commands that get forwarded to real MCI
>BREAK
>CAPTURE
>CONFIGURE
>COPY
>CUE
>CUT
>DELETE
>ESCAPE
>FREEZE
>INDEX
>LIST
>LOAD
>MARK
>MONITOR
>PASTE
>PUT
>QUALITY
>REALIZE
>RECORD
>RESERVE
>RESTORE
>SAVE
>SETAUDIO
>SETTIMECODE
>SETTUNER
>SETVIDEO
>SIGNAL
>SPIN
>STEP
>UNDO
>UNFREEZE
>UPDATE
>WHERE
>WINDOW