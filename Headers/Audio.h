#ifndef AUDIO_H_
#define AUDIO_H_

#include <stdint.h>

typedef struct AudioChannel AudioChannel;

// A callback function that is called in response to certain events happening
// to an audio entry in the channel queue.
//
// * The "ending" callback is called shortly before an audio entry finishes
//   playing. The intent of this callback is to allow a new audio entry to
//   be added to the channel queue to be played immediately after the
//   almost-finished entry.
//
//   If this callback is set to NULL in an audio entry, then nothing happens
//   in response to the audio entry being close to finishing.
//
// * The "destroy" callback is called when the audio entry's data buffer is no
//   longer being used by this audio library. Either the entry has completely
//   finished playing, or the audio queue was cleared by the user or by the
//   closing of the audio channel. The intent of this callback is to notify
//   the owner of the data buffer that the data buffer doesn't need to exist
//   any more. The data buffer must be valid until this callback is called.
//
//   If this callback is set to NULL in an audio entry, then nothing happens
//   in response to the audio entry being released. To ensure that the data
//   buffer is no longer being used, call the AudioChannel_Close function or
//   the AudioChannel_ClearAudio function before freeing the data buffer.
//
typedef void (*AudioCallback)(AudioChannel *channel, void *userdata);

typedef struct AudioEntry
{
	// The data buffer holding the audio bytes.
	const unsigned char *buffer;
	// The number of bytes in the data buffer.
	uint32_t length;
	// The ending callback function.
	AudioCallback endingCallback;
	// The destroy callback function.
	AudioCallback destroyCallback;
	// User data to be passed to the callback functions.
	void *userdata;
} AudioEntry;

// Initialize the internal audio-management structures.
//
// This function must be called only once (at the beginning of the program),
// and before calling any other Audio_Xxx functions. The behavior is undefined
// if this rule is not followed. Call Audio_KillDevice when you are finished
// with the audio interface. The function returns nonzero for success and
// zero for failure.
int Audio_InitDevice(void);

// Release the internal audio-management structures.
//
// This function must be called only once (at the end of the program), and only
// after a successful call to Audio_InitDevice. No other Audio_Xxx functions
// should be running while Audio_KillDevice releases structures, or else the
// behavior is undefined.
void Audio_KillDevice(void);

// Get the master volume for channels managed by this audio interface.
// The value ranges between 0.0 (for complete silence) and 1.0 (for full volume).
float Audio_GetMasterVolume(void);

// Set the master volume for channels managed by this audio interface.
// The value ranges between 0.0 (for complete silence) and 1.0 (for full volume).
void Audio_SetMasterVolume(float newVolume);

// Open an audio output channel that plays a single audio format. The function
// returns NULL on failure. Release the audio channel pointer by calling the
// AudioChannel_Close function. The audio channel will not play any audio until
// AudioChannel_QueueAudio is called.
AudioChannel *AudioChannel_Open(uint16_t channels, uint16_t bitsPerSample, uint32_t samplesPerSec);

// Close an audio output channel. All queued sounds will be removed from the
// channel's queue, and all playing sound will be stopped. Do not use the
// AudioChannel pointer after this function.
void AudioChannel_Close(AudioChannel *channel);

// Add an entry to the audio channel's queue. The sound will play as soon as
// possible if the queue is empty. Otherwise, the sound will be played after
// the other sound entries that are already in the queue.
void AudioChannel_QueueAudio(AudioChannel *channel, const AudioEntry *entry);

// Clear all entries waiting in the channel's queue, and stop all current sound
// playing.
void AudioChannel_ClearAudio(AudioChannel *channel);

// Return whether the audio channel is currently playing any audio.
// Returns nonzero to indicate that the channel is playing, and zero otherwise.
int AudioChannel_IsPlaying(AudioChannel *channel);

#endif
