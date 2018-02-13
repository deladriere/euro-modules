// Defines parameters for S1V30120

// Commands
// Boot mode
#define ISC_VERSION_REQ			0x0005
#define ISC_BOOT_LOAD_REQ		0x1000
#define ISC_BOOT_RUN_REQ 		0x1002
#define ISC_TEST_REQ 			0x0003

// Normal (run) mode

#define ISC_AUDIO_CONFIG_REQ 	0x0008
#define ISC_AUDIO_VOLUME_REQ	0x000A
#define ISC_AUDIO_MUTE_REQ		0x000C

#define ISC_TTS_CONFIG_REQ		0x0012
//11 kHz
#define ISC_TTS_SAMPLE_RATE		0x01
#define ISC_TTS_VOICE			0x00
#define ISC_TTS_EPSON_PARSE  	0x00
#define ISC_TTS_LANGUAGE  		0x00
// 200 words/min
#define ISC_TTS_SPEAK_RATE_LSB	0xC8
#define ISC_TTS_SPEAK_RATE_MSB	0x00
#define ISC_TTS_DATASOURCE		0x00


#define ISC_TTS_SPEAK_REQ 		0x0014




// Response messages
// Boot mode
#define ISC_VERSION_RESP		0x0006
#define ISC_BOOT_LOAD_RESP		0x1001
#define ISC_BOOT_RUN_RESP 		0x1003
#define ISC_TEST_RESP 			0x0004

// Normal (run) mode

#define ISC_AUDIO_CONFIG_RESP   0x0009
#define ISC_AUDIO_VOLUME_RESP	0x000B
#define ISC_AUDIO_MUTE_RESP		0x000D

#define ISC_TTS_CONFIG_RESP		0x0013


#define ISC_TTS_SPEAK_RESP 		0x0015

// Fatal error indication
#define ISC_ERROR_IND			0x0000

// Request blocked
#define ISC_MSG_BLOCKED_RESP	0x0007

#define ISC_TTS_FINISHED_IND	0x0021



// Parameters

// Audio config
// See page 42 in S1V30120 Message Protocol Specification

// MONO = 0x00, all other values = reserved
#define TTS_AUDIO_CONF_AS 	0x00

// Audio gain = +18 db
#define TTS_AUDIO_CONF_AG 	0x43

// Audio amp not selected
#define TTS_AUDIO_CONF_AMP	0x00

// Sample rate 11kHz
#define TTS_AUDIO_CONF_ASR 	0x01

// Audio routing: application to DAC
#define TTS_AUDIO_CONF_AR 	0x00

// Audio tone control: depreciated, set to 0
#define TTS_AUDIO_CONF_ATC 	0x00

// Audio click source: internal, set to 0
#define TTS_AUDIO_CONF_ACS 	0x00

// DAC is on only while speech decoder
// or TTS synthesis is outputting audio
#define TTS_AUDIO_CONF_DC 	0x00

// TTS Config
