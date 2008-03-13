/**
 * \file glc/common/glc.h
 * \brief glc types and structures
 * \author Pyry Haulos <pyry.haulos@gmail.com>
 * \date 2007-2008
 */

/* glc.h -- ALSA & OpenGL video capture tool
  version 0.5.1, March 10th, 2008

  Copyright (C) 2007-2008 Pyry Haulos <pyry.haulos@gmail.com>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/**
 * \defgroup capture capture
 * \defgroup core core
 * \defgroup export export format filters
 * \defgroup play playback
 * \defgroup common common utilities and data structures
 * \defgroup support optional support libraries
 */

/**
 * \addtogroup common
 *  \{
 */

#ifndef _GLC_H
#define _GLC_H

#include <sys/types.h>
#include <semaphore.h>

/** always export this object  */
#define __PUBLIC __attribute__ ((visibility ("default")))
/** always hide this object */
#define __PRIVATE __attribute__ ((visibility ("hidden")))

/** stream version */
#define GLC_STREAM_VERSION                  0x2
/** file signature = "GLC" */
#define GLC_SIGNATURE                0x00434c47
/** glc version string */
#define GLC_VERSION                     "0.5.1"

/** unsigned time in microseconds */
typedef u_int64_t glc_utime_t;
/** signed time in microseconds */
typedef int64_t glc_stime_t;

/** picture context number */
typedef int32_t glc_ctx_i;
/** audio stream number */
typedef int32_t glc_audio_i;
/** size, used in stream to ensure compability */
typedef u_int64_t glc_size_t;
/** sizeof(glc_size_t) */
#define GLC_SIZE_SIZE                         8

/** flags */
typedef u_int32_t glc_flags_t;

/**
 * \brief stream info structure
 *
 * Each glc stream file should start with stream info
 * structure. [name_size + date_size] sized data area should
 * follow stream info:
 *
 * First [name_size] bytes contain null-terminated application
 * path string. [date_size] bytes starting at [name_size]
 * contain null-terminated date string in UTC format.
 */
typedef struct {
	/** file signature */
	u_int32_t signature;
	/** stream version */
	u_int32_t version;
	/** fps */
	double fps;
	/** flags */
	glc_flags_t flags;
	/** captured program pid */
	u_int32_t pid;
	/** size of captured program's name */
	u_int32_t name_size;
	/** size of date */
	u_int32_t date_size;
} glc_stream_info_t;
/** sizeof(glc_stream_info_t) */
#define GLC_STREAM_INFO_SIZE             32

typedef struct glc_core_s* glc_core_t;
typedef struct glc_util_s* glc_util_t;
typedef struct glc_log_s* glc_log_t;
typedef struct glc_state_s* glc_state_t;

/**
 * \brief glc structure
 */
typedef struct {
	/** core internal state */
	glc_core_t core;
	/** util internal state */
	glc_util_t util;
	/** log internal state */
	glc_log_t log;
	/** state internal structure */
	glc_state_t state;
	/** state flags */
	glc_flags_t state_flags;
} glc_t;

/** error */
#define GLC_ERROR                         0
/** warning */
#define GLC_WARNING                       1
/** performance information */
#define GLC_PERFORMANCE                   2
/** information */
#define GLC_INFORMATION                   3
/** debug */
#define GLC_DEBUG                         4

/** stream message type */
typedef char glc_message_type_t;
/** end of stream */
#define GLC_MESSAGE_CLOSE              0x01
/** picture */
#define GLC_MESSAGE_PICTURE            0x02
/** picture context message */
#define GLC_MESSAGE_CTX                0x03
/** lzo-compressed packet */
#define GLC_MESSAGE_LZO                0x04
/** audio format message */
#define GLC_MESSAGE_AUDIO_FORMAT       0x05
/** audio data message */
#define GLC_MESSAGE_AUDIO              0x06
/** quicklz-compressed packet */
#define GLC_MESSAGE_QUICKLZ            0x07
/** color correction information */
#define GLC_MESSAGE_COLOR              0x08
/** plain container */
#define GLC_MESSAGE_CONTAINER          0x09

/**
 * \brief stream message header
 */
typedef struct {
	/** stream message type */
	glc_message_type_t type;
} glc_message_header_t;
/** sizeof(glc_message_header_t) */
#define GLC_MESSAGE_HEADER_SIZE           1

/**
 * \brief lzo-compressed message header
 */
typedef struct {
	/** uncompressed data size */
	glc_size_t size;
	/** original message header */
	glc_message_header_t header;
} glc_lzo_header_t;
/** sizeof(glc_lzo_header_t) */
#define GLC_LZO_HEADER_SIZE               9

/**
 * \brief quicklz-compressed message header
 */
typedef struct {
	/** uncompressed data size */
	glc_size_t size;
	/** original message header */
	glc_message_header_t header;
} glc_quicklz_header_t;
/** sizeof(glc_quicklz_header_t) */
#define GLC_QUICKLZ_HEADER_SIZE           9

/**
 * \brief picture header
 */
typedef struct {
	/** time */
	glc_utime_t timestamp;
	/** picture context number */
	glc_ctx_i ctx;
} glc_picture_header_t;
/** sizeof(glc_picture_header_size) */
#define GLC_PICTURE_HEADER_SIZE          12

/**
 * \brief picture context message
 */
typedef struct {
	/** context flags */
	glc_flags_t flags;
	/** context number */
	glc_ctx_i ctx;
	/** width */
	u_int32_t w;
	/** height */
	u_int32_t h;
} glc_ctx_message_t;
/** sizeof(glc_ctx_message_t) */
#define GLC_CTX_MESSAGE_SIZE             16

/** create context */
#define GLC_CTX_CREATE                    1
/** update existing context */
#define GLC_CTX_UPDATE                    2
/** 24bit BGR, last row first */
#define GLC_CTX_BGR                       4
/** 32bit BGRA, last row first */
#define GLC_CTX_BGRA                      8
/** planar YV12 420jpeg */
#define GLC_CTX_YCBCR_420JPEG            16
/** double-word aligned rows (GL_PACK_ALIGNMENT = 8) */
#define GLC_CTX_DWORD_ALIGNED            32

/**
 * \brief audio format message
 */
typedef struct {
	/** stream flags */
	glc_flags_t flags;
	/** audio stream number */
	glc_audio_i audio;
	/** rate */
	u_int32_t rate;
	/** number of channels */
	u_int32_t channels;
} glc_audio_format_message_t;
/** sizeof(glc_audio_format_message_t) */
#define GLC_AUDIO_FORMAT_MESSAGE_SIZE    16

/** interleaved */
#define GLC_AUDIO_INTERLEAVED             1
/** unknown/unsupported format */
#define GLC_AUDIO_FORMAT_UNKNOWN          2
/** signed 16bit little-endian */
#define GLC_AUDIO_S16_LE                  4
/** signed 24bit little-endian */
#define GLC_AUDIO_S24_LE                  8
/** signed 32bit little-endian */
#define GLC_AUDIO_S32_LE                 16

/**
 * \brief audio data message header
 */
typedef struct {
	/** time */
	glc_utime_t timestamp;
	/** data size */
	glc_size_t size;
	/** audio stream number */
	glc_audio_i audio;
} glc_audio_header_t;
/** sizeof(glc_audio_header_t) */
#define GLC_AUDIO_HEADER_SIZE            20

/**
 * \brief color correction information message
 */
typedef struct {
	/** context */
	glc_ctx_i ctx;
	/** brightness */
	float brightness;
	/** contrast */
	float contrast;
	/** red gamma */
	float red;
	/** green gamma */
	float green;
	/** blue gamma */
	float blue;
} glc_color_message_t;
/** sizeof(glc_color_message_t) */
#define GLC_COLOR_MESSAGE_SIZE           24

/**
 * \brief container message
 */
typedef struct {
	/** size */
	glc_size_t size;
	/** header */
	glc_message_header_t header;
} glc_container_message_t;
/** sizeof(glc_container_message_t) */
#define GLC_CONTAINER_MESSAGE_SIZE        9

#endif

/**  \} */