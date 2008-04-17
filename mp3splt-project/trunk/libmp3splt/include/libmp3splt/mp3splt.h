/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2006 Munteanu Alexandru - io_alex_2002@yahoo.fr
 *
 * http://mp3splt.sourceforge.net
 *
 *********************************************************/

/**********************************************************
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307,
 * USA.
 *
 *********************************************************/

#ifndef MP3SPLT_MP3SPLT_H

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <ltdl.h>

/**
 * @file  mp3splt.h
 * 
 * @brief Main types, error codes and confirmations.
 * 
 * This is the main file containing the most important types, error
 * codes and confirmations
 *
 * The errors are negative values, the warnings and the confirmations
 * are positive values
 */

/**
 * @brief True value
 */
#define SPLT_TRUE 1
/**
 * @brief False value
 */
#define SPLT_FALSE 0

/******************************/
/* Structures for the freedb  */

/**
 * @brief Defines one search result from the freedb search
 *
 * @see splt_freedb_results
 * @see mp3splt_get_freedb_search
 */
typedef struct {
  /**
   * @brief Name of the album for this result
   */
  char *name;
  /**
   * @brief Unique identifier for this result
   */
  int id;
  /**
   * @brief How many revisions this result has
   *
   * If having revisions, the unique identifier for a revision is
   * #id + revision + 1, with 0 <= revision < #revision_number\n
   */
  int revision_number;
  /**
   * @brief Contains the revisions of the result
   * 
   * The #revisions table contains #revision_number elements,
   * the consecutive numbers starting at 2 (you might not need it)
   */
  int *revisions;
} splt_freedb_one_result;

/**
 * @brief All the freedb search results
 *
 * @see splt_freedb_one_result
 * @see mp3splt_get_freedb_search
 */
typedef struct {
  /**
   * @brief All the freedb results
   */
  splt_freedb_one_result *results;
  /**
   * @brief How many results we have
   */
  int number;
} splt_freedb_results;

/**
 * @brief Maximum results for the freedb search
 */
#define SPLT_MAXCD 512

//maximum length of the disc id
#define SPLT_DISCIDLEN 8

//structure for the freedb search
struct splt_cd {
  char discid[SPLT_DISCIDLEN+1];
  char category[20];
};

typedef struct {
  struct splt_cd discs[SPLT_MAXCD];
  int foundcd;
} splt_cd_state;

//structure containing everything used for the
//freedb search
typedef struct {
  //we stock here the results of the freedb search
  splt_freedb_results *search_results;
  //we stock the state of the CD
  //(for the freedb search)
  splt_cd_state *cdstate;
} splt_freedb;

/******************************/
/* Structures for the wrap    */

/**
 * @brief The wrapped filenames found inside a file
 *
 * @see mp3splt_get_wrap_files
 */
typedef struct {
  /**
   * @brief How many filenames we have
   */
  int wrap_files_num;
  /**
   * @brief The filenames
   */
  char **wrap_files;
} splt_wrap;

/************************************/
/* Structures for the syncerrors    */

/**
 * @brief The number of syncerrors
 *
 * @see mp3splt_get_syncerrors
 */
typedef struct {
  off_t *serrors_points;
  /**
   * @brief How many syncerrors have been found
   */
  long int serrors_points_num;
} splt_syncerrors;

/***************************************/
/* Structures for the output format    */

#define SPLT_MAXOLEN 255
#define SPLT_OUTNUM  10

//structure defining the output format
typedef struct {
  //format as @n_@t.. as a string
  char *format_string;
  //when we have @n option on output format
  char output_format_digits;
  //format for the cddb cue output
  char format[SPLT_OUTNUM+1][SPLT_MAXOLEN];
} splt_oformat;

/***************************/
/* Structures for the tags */

/**
 * @brief The tags of a splitpoint
 *
 * The structure contains the tags that we can set to a filename
 * generated from a splitpoint. Tags may also define the output filenames.
 *
 * @see mp3splt_append_tags
 * @see mp3splt_get_tags
 */
typedef struct {
  /**
   * @brief The title
   */
  char *title;
  /**
   * @brief The artist
   */
  char *artist;
  /**
   * @brief The album
   */
  char *album;
  /**
   * @brief The performer
   * 
   * The performer is not part of the tags, but may replace the
   * #artist in some cases and it is useful for the output filenames\n
   * You can also look at #SPLT_OPT_OUTPUT_DEFAULT
   */
  char *performer;
  /**
   * @brief The year
   */
  char *year;
  /**
   * @brief A comment
   */
  char *comment;
  /**
   * @brief The track number
   */
  int track;
  /**
   * @brief The genre
   */
  unsigned char genre;
} splt_tags;

/**
 * @brief Definition of a splitpoint
 *
 * @see mp3splt_append_splitpoint
 * @see mp3splt_get_splitpoints
 */
typedef struct {
  /**
   * @brief Value of the splitpoint in hundreths of seconds
   */
  long value;
  /**
   * @brief Name of the new filename issued from the splitpoint
   */
  char *name;
} splt_point;

/*****************************/
/* Structure for the silence */

struct splt_ssplit {
  double begin_position;
  double end_position;
  long len;
  struct splt_ssplit *next;
};

/**********************************/
/* Structure for the split        */

/**
 * @brief must be documented
 */
typedef struct splt_progres {
  //maximum number of characters for the filename(without the
  //extension) when displaying the progress
  //warning, don't set this more than 512 !
  //default is 40
  int progress_text_max_char;
  //filename that we are currently splitting
  char filename_shorted[512];
  //the current percent of the progress
  float percent_progress;
  //the number we are currently splitting
  int current_split;
  //the maximum number of splits
  int max_splits;
  //the progress type
  //can be :
  //SPLT_PROGRESS_PREPARE
  //SPLT_PROGRESS_CREATE
  //SPLT_PROGRESS_SEARCH_SYNC
  //SPLT_PROGRESS_SCAN_SILENCE
  int progress_type;
  //use this variable as you wish
  //this variable will not be modified by the library
  //but is 0 at the start
  int user_data;
  //float = fraction of how much %
  //char * = name on the progress bar
  void (*progress)(struct splt_progres*);
} splt_progress;

typedef struct {
  //total time of the song
  long total_time;
  //the part of file that we are currently splitting
  //1 if the first, 2 if the second
  //the number of splitpoints
  int current_split;
  //how many splits, this will be modified
  //by check_splitpts_inf_song_length()
  //to really see how many splitpoints we have
  //look at real_splitnumber
  int splitnumber;
  //how many splitpoints we have
  int real_splitnumber;
  //put this function if you want that the library
  //tells you when a file has been splitted
  //the char* is the filename
  void (*file_splitted)(char *,int);
  //for the progress bar
  splt_progress *p_bar;
  //sends a message to the main program to tell him what
  //he is doing
  //possible values : 
  //SPLT_MESS_FRAME_MODE_ENABLED
  //SPLT_MESS_START_WRAP_SPLIT
  void (*put_message)(int);
  //structure in which we have all the splitpoints
  splt_point *points;
  //how many tags we have
  int real_tagsnumber;
  //structure in which we have all the tags
  splt_tags *tags;
} splt_struct;

/**********************************/
/* Options structure              */

/**
 * @brief Values for the #SPLT_OPT_SPLIT_MODE option
 *
 * Values for the #SPLT_OPT_SPLIT_MODE option
 */
typedef enum {
  /**
   * Normal split
   */
  SPLT_OPTION_NORMAL_MODE,
  /**
   * Split the file created with mp3wrap or albumwrap
   */
  SPLT_OPTION_WRAP_MODE,
  /**
   * Split with silence detection
   */
  SPLT_OPTION_SILENCE_MODE,
  /**
   * Split with error mode 
   * It is useful to split large file derivated from a concatenation of
   * smaller files
   */
  SPLT_OPTION_ERROR_MODE,
  /**
   * Will create an indefinite number of smaller files with
   * a fixed time length specified by #SPLT_OPT_SPLIT_TIME
   */
  SPLT_OPTION_TIME_MODE
} splt_split_mode_options;

/**
 * @brief Values for the #SPLT_OPT_OUTPUT_FILENAMES option
 *
 * Values for the #SPLT_OPT_OUTPUT_FILENAMES option
 */
typedef enum {
  //output specified by the set_oformat
  SPLT_OUTPUT_FORMAT,
  //output filename like song_1m_2s_3h__2m_33s_65h.ogg
  SPLT_OUTPUT_MINS_SECS,
  //the default output
  //it depends of the type of the split
  SPLT_OUTPUT_DEFAULT,
  //we don't change anything, must put the filenames with
  //the functions set_..
  SPLT_OUTPUT_CUSTOM
} splt_output_filenames_options;

/**
 * @brief Default value for the #SPLT_OPT_PARAM_THRESHOLD option
 */
#define SPLT_DEFAULT_PARAM_THRESHOLD -48.0
/**
 * @brief Default value for the #SPLT_OPT_PARAM_OFFSET option
 */
#define SPLT_DEFAULT_PARAM_OFFSET 0.8
/**
 * @brief Default value for the #SPLT_OPT_PARAM_MIN_LENGTH option
 */
#define SPLT_DEFAULT_PARAM_MINIMUM_LENGTH 0.0
/**
 * @brief Default value for the #SPLT_OPT_PARAM_GAP option
 */
#define SPLT_DEFAULT_PARAM_GAP 30
/**
 * @brief Default value for the #SPLT_OPT_PARAM_NUMBER_TRACKS option
 */
#define SPLT_DEFAULT_PARAM_TRACKS 0

/**
 * @brief Values for the #SPLT_OPT_TAGS option
 *
 * Values for the #SPLT_OPT_TAGS option
 */
typedef enum {
  /**
   * Keep the tags of the original file
   */
  SPLT_TAGS_ORIGINAL_FILE,
  /**
   * Keep the tags issued from cddb, cue or
   * set by the user with #mp3splt_append_tags
   */
  SPLT_CURRENT_TAGS,
  /**
   * Does not put any tags
   */
  SPLT_NO_TAGS
} splt_tags_options;

/**
 * @brief Default output for the cddb and cue.
 * See #mp3splt_set_oformat
 */
#define SPLT_DEFAULT_OUTPUT "@a - @n - @t"
/**
 * @brief Default output for the syncerror.
 * See #mp3splt_set_oformat
 */
#define SPLT_DEFAULT_SYNCERROR_OUTPUT "@f_error_@n"
/**
 * @brief Default output for the silence split.
 * See #mp3splt_set_oformat
 */
#define SPLT_DEFAULT_SILENCE_OUTPUT "@f_silence_@n"

//structure with all the options supplied to split the file
typedef struct {
  //this can take the following values :
  //SPLT_OPTION_NORMAL_MODE
  //SPLT_OPTION_WRAP_MODE
  //SPLT_OPTION_SILENCE_MODE
  //SPLT_OPTION_ERROR_MODE
  //SPLT_OPTION_TIME_MODE
  splt_split_mode_options split_mode;

  //might be :
  //SPLT_TAGS_ORIGINAL_FILE - write tags from original file
  //SPLT_NO_TAGS - does not write any tags
  //SPLT_CURRENT_TAGS - tags issued from the cddb or cue for example
  //or that we set manually with the functions
  splt_tags_options tags;

  //defines the output filenames
  splt_output_filenames_options output_filenames;

  //frame mode (mp3 only). Process all frames, seeking split positions
  //by counting frames and not with bitrate guessing.
  short option_frame_mode;
  //the time of split when split_mode = OPTION_TIME_SPLIT
  float split_time;
  //this option uses silence detection to auto-adjust splitpoints.
  short option_auto_adjust;
  //input not seekable. enabling this allows you to split mp3 and ogg streams
  //which can be read only one time and canât be seeked.
  //WARNING!
  //if you don't know what this means, set it to FALSE
  short option_input_not_seekable;

  //PARAMETERS---------------------------------------
  //PARAMETERS for option_auto_adjust and option_silence_mode :
  //the sound level to be considered silence
  //(it is a float number between -96 and 0. Default is -48 dB)
  float parameter_threshold;
  //the offset of cutpoint in silence
  //Float number between -2 and 2 and allows you to adjust the offset
  //of cutpoint in silence time.0 is the begin of silence, and 1 the
  //end;default is 0.8. 
  float parameter_offset;

  //PARAMETERS for option_silence_mode :
  //the desired number of tracks
  //(positive integer number of tracks to be splitted;by default all
  //tracks are splitted)
  int parameter_number_tracks;
  //the minimum silence length in seconds
  //(positive float of the minimum number of seconds to be considered
  //a valid splitpoint)
  float parameter_minimum_length;
  //allows you to remove the silence between splitted tracks
  short parameter_remove_silence;

  //PARAMETERS for option_auto_adjust :
  //the gap value around splitpoint to search for silence
  //(positive integer for the time to decode before and after
  //splitpoint;default gap is 30 seconds)
  int parameter_gap;

  //if we set all the tags like the x one,
  //tags_like_x_one must be different of -1
  int tags_after_x_like_x_one;
} splt_options;

/**********************************/
/* Main structure                 */

//internal structures
typedef struct
{
  //if we have send the message frame mode enabled
  int frame_mode_enabled;
  //if current_refresh_rate = refresh_rate, we call
  //the progress callback
  int current_refresh_rate;
  //if set to SPLT_TRUE,
  //then we don't send messages to clients
  int messages_locked;
  //if we currently use the library, we lock it
  int library_locked;
  //the new filename path (internal)
  char *new_filename_path;
  //used for the normal split
  double split_begin;
  double split_end;
} splt_internal;

/*
 * Structure containing information about one plugin.
 * Must be filled up at plugin initialisation.
 */
typedef struct
{
  float version;
  char *name;
  char *extension;
} splt_plugin_info;

//contains pointers to the plugin functions
typedef struct {
  int (*check_plugin_is_for_file)(void *state, int *error);
  void (*set_plugin_info)(splt_plugin_info *info, int *error);
  void (*search_syncerrors)(void *state, int *error);
  void (*dewrap)(void *state, int listonly, char *dir, int *error);
  void (*set_total_time)(void *state, int *error);
  void (*simple_split)(void *state, char *final_fname, double begin_point,
      double end_point, int *error);
  int (*scan_silence)(void *state, int *error);
  void (*set_original_tags)(void *state, int *error);
} splt_plugin_func;

//structure containing all the data about one plugin
typedef struct
{
  splt_plugin_info info;
  //complete filename of the plugin shared object
  char *plugin_filename;
  //plugin handle get with lt_dlopen
  //-would be closed with lt_dlclose
  void *plugin_handle;
  //plugin functions
  splt_plugin_func *func;
} splt_plugin_data;

//internal plugins structure
typedef struct
{
  //directories where we scan for plugins
  char **plugins_scan_dirs;
  int number_of_dirs_to_scan;
  //the number of plugins found
  int number_of_plugins_found;
  //data structure about all the plugins
  splt_plugin_data *data;
} splt_plugins;

//structure for the splt state
typedef struct {

  //if we cancel split or not
  //set to SPLT_TRUE cancels the split
  short cancel_split;
  //filename to split
  char *fname_to_split;
  //where the splitted file will be splitted
  char *path_of_split;

  //if this is non null, we write a m3u from the splitted files
  char *m3u_filename;

  //tags of the original file to split
  splt_tags original_tags;

  //options for the split
  splt_options options;
  //split related
  splt_struct split;
  //output format  
  splt_oformat oformat;
  //wrap related
  splt_wrap *wrap;
  //syncerror related
  splt_syncerrors *serrors;
  //number of sync errors found
  //(syncerror mode)
  unsigned long syncerrors;
  //freedb related
  splt_freedb fdb;

  //internal options
  splt_internal iopts;

  //see the ssplit structure
  struct splt_ssplit *silence_list;

  //file format states, mp3,ogg..
  void *codec;

  //plugins structure
  splt_plugins *plug;
  int current_plugin;
} splt_state;

/*****************************************/
/* Confirmations, errors and messages    */

//error and confirmation messages :
//sync
/**
 * @brief Warning, split : mp3 file might be VBR
 */
#define SPLT_MIGHT_BE_VBR 301
/**
 * @brief Confirmation, syncerror : syncerror processed ok
 */
#define SPLT_SYNC_OK 300
/**
 * @brief Error, syncerror : error for the syncerror
 */
#define SPLT_ERR_SYNC -300
/**
 * @brief Error, syncerror : no sync errors found
 */
#define SPLT_ERR_NO_SYNC_FOUND -301
/**
 * @brief Error, syncerror : too many syncerrors found
 */
#define SPLT_ERR_TOO_MANY_SYNC_ERR -302

//freedb
/**
 * @brief Warning, freedb : maximum number of CD reached
 */
#define SPLT_FREEDB_MAX_CD_REACHED 104
/**
 * @brief Confirmation, cue : file processed ok
 */
#define SPLT_FREEDB_CUE_OK 103
/**
 * @brief Confirmation, cddb : file processed ok
 */
#define SPLT_FREEDB_CDDB_OK 102
/**
 * @brief Confirmation, freedb : file processed ok
 */
#define SPLT_FREEDB_FILE_OK 101
/**
 * @brief Confirmation, freedb : search ok
 */
#define SPLT_FREEDB_OK 100

/**
 * @brief Error, freedb : cannot initialise socket
 */
#define SPLT_FREEDB_ERROR_INITIALISE_SOCKET -101
/**
 * @brief Error, freedb : cannot get host by name
 */
#define SPLT_FREEDB_ERROR_CANNOT_GET_HOST -102
/**
 * @brief Error, freedb : cannot open socket
 */
#define SPLT_FREEDB_ERROR_CANNOT_OPEN_SOCKET -103
/**
 * @brief Error, freedb : cannot connect to host
 */
#define SPLT_FREEDB_ERROR_CANNOT_CONNECT -104
/**
 * @brief Error, freedb : cannot send message
 */
#define SPLT_FREEDB_ERROR_CANNOT_SEND_MESSAGE -105
/**
 * @brief Error, freedb : invalid server answer
 */
#define SPLT_FREEDB_ERROR_INVALID_SERVER_ANSWER -106
/**
 * @brief Error, freedb : site returned 201 error
 */
#define SPLT_FREEDB_ERROR_SITE_201 -107
/**
 * @brief Error, freedb : site returned 200 error
 */
#define SPLT_FREEDB_ERROR_SITE_200 -108
/**
 * @brief Error, freedb : bad communication between server and client
 */
#define SPLT_FREEDB_ERROR_BAD_COMMUNICATION -109
/**
 * @brief Error, freedb : error getting server informations
 */
#define SPLT_FREEDB_ERROR_GETTING_INFOS -110
/**
 * @brief Error, freedb : no CD found for the search
 */
#define SPLT_FREEDB_NO_CD_FOUND -111
/**
 * @brief Error, freedb : cannot receive message from server
 */
#define SPLT_FREEDB_ERROR_CANNOT_RECV_MESSAGE -112
/**
 * @brief Error, cue : cannot open file for reading
 */
#define SPLT_CUE_ERROR_CANNOT_OPEN_FILE_READING -113
/**
 * @brief Error, cddb : cannot open file for reading
 */
#define SPLT_CDDB_ERROR_CANNOT_OPEN_FILE_READING -114
/**
 * @brief Error, cue : invalid cue file, the parse failed
 */
#define SPLT_INVALID_CUE_FILE -115
/**
 * @brief Error, cddb : invalid cddb file, the parse failed
 */
#define SPLT_INVALID_CDDB_FILE -116
/**
 * @brief Error, cddb : cannot write cddb file
 */
#define SPLT_CANNOT_WRITE_CDDB_FILE -117
/**
 * @brief Error, freedb : No such CD entry in database
 */
#define SPLT_FREEDB_NO_SUCH_CD_IN_DATABASE -118
/**
 * @brief Error, freedb : site returned an unknown error
 */
#define SPLT_FREEDB_ERROR_SITE -119

//wrap
/**
 * @brief Confirmation, dewrap : dewrap processed ok
 */
#define SPLT_DEWRAP_OK 200

/**
 * @brief Error, dewrap : file length error
 */
#define SPLT_DEWRAP_ERR_FILE_LENGTH -200
/**
 * @brief Error, dewrap : wrapped with a too old version of mp3wrap
 */
#define SPLT_DEWRAP_ERR_VERSION_OLD -201
/**
 * @brief Error, dewrap : file damaged
 */
#define SPLT_DEWRAP_ERR_NO_FILE_OR_BAD_INDEX -202
/**
 * @brief Error, dewrap : file damaged or incomplete
 */
#define SPLT_DEWRAP_ERR_FILE_DAMAGED_INCOMPLETE -203
/**
 * @brief Error, dewrap : file not wrapped or damaged
 */
#define SPLT_DEWRAP_ERR_FILE_NOT_WRAPED_DAMAGED -204

/**
 * @brief Warning, split : splitted, end of file
 */
#define SPLT_OK_SPLITTED_EOF 8
/**
 * @brief Warning, silence detection : no silence splitpoint found
 */
#define SPLT_NO_SILENCE_SPLITPOINTS_FOUND 7
/**
 * @brief Confirmation, time split : time split processed ok
 */
#define SPLT_TIME_SPLIT_OK 6
/**
 * @brief Confirmation, silence split : silence split processed ok
 */
#define SPLT_SILENCE_OK 5
/**
 * @brief Warning, split : splitpoints bigger than file length
 */
#define SPLT_SPLITPOINT_BIGGER_THAN_LENGTH 4
/**
 * @brief Confirmation, split : splitted
 */
#define SPLT_OK_SPLITTED 1
/**
 * @brief Confirmation : no error
 */
#define SPLT_OK 0

/**
 * @brief Error, split : not enough splitpoints
 */
#define SPLT_ERROR_SPLITPOINTS -1
/**
 * @brief Error, split : cannot open file
 */
#define SPLT_ERROR_CANNOT_OPEN_FILE -2
/**
 * @brief Error, split : invalid mp3 file
 */
#define SPLT_ERROR_INVALID -3
/**
 * @brief Error, split : splitpoints are equal
 */
#define SPLT_ERROR_EQUAL_SPLITPOINTS -5
/**
 * @brief Error, split : splitpoints are not in order
 */
#define SPLT_ERROR_SPLITPOINTS_NOT_IN_ORDER -6
/**
 * @brief Error, split : negative splitpoint found
 */
#define SPLT_ERROR_NEGATIVE_SPLITPOINT -7
/**
 * @brief Error, split : incorrect split path
 */
#define SPLT_ERROR_INCORRECT_PATH -8
/**
 * @brief Error, split : ogg syncerror not implemented
 */
#define SPLT_ERROR_CANNOT_SYNC -9
/**
 * @brief Error, split : incompatible split options
 */
#define SPLT_ERROR_INCOMPATIBLE_OPTIONS -10
/**
 * @brief Error, silence split : error while doing the silence split
 */
#define SPLT_ERROR_SILENCE -11
/**
 * @brief Error, split : input and output are the same file
 */
#define SPLT_ERROR_INPUT_OUTPUT_SAME_FILE -12
/**
 * @brief Error : cannot allocate memory
 */
#define SPLT_ERROR_CANNOT_ALLOCATE_MEMORY -15
/**
 * @brief Error, split : cannot open destination file
 */
#define SPLT_ERROR_CANNOT_OPEN_DEST_FILE -16
/**
 * @brief Error, split : cannot write to destination file
 */
#define SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE -17
/**
 * @brief Error, split : error while reading file
 */
#define SPLT_ERROR_WHILE_READING_FILE -18
/**
 * @brief Error, split : error while seeking file
 */
#define SPLT_ERROR_SEEKING_FILE -19
/**
 * @brief Error, split : begin is out of file
 */
#define SPLT_ERROR_BEGIN_OUT_OF_FILE -20
/**
 * @brief Error, split : inexistent input file
 */
#define SPLT_ERROR_INEXISTENT_FILE -21
/**
 * @brief Error, split : split canceled
 */
#define SPLT_SPLIT_CANCELLED -22
/**
 * @brief Error, wrap split : wrap not implemented for this format
 */
#define SPLT_ERROR_WRAP_NOT_IMPLEMENTED -23
/**
 * @brief Error, the library is being used
 */
#define SPLT_ERROR_LIBRARY_LOCKED -24
/**
 * @brief Error, the state has not been initialised with #mp3splt_new_state
 */
#define SPLT_ERROR_STATE_NULL -25
/**
 * @brief Error, the time split has a negative value
 */
#define SPLT_ERROR_NEGATIVE_TIME_SPLIT -26
/**
 * @brief Error, cannot create output directory
 */
#define SPLT_ERROR_CANNOT_CREATE_DIRECTORY -27
/**
 * @brief Error, invalid format
 */
#define SPLT_ERROR_INVALID_FORMAT -28
/**
 * @brief Error, cannot find plugins
 */
#define SPLT_ERROR_NO_PLUGIN_FOUND -29
/**
 * @brief Error, cannot init libltdl
 */
#define SPLT_ERROR_CANNOT_INIT_LIBLTDL -30

//output format
/**
 * @brief Confirmation, output format : the output format is ok
 */
#define SPLT_OUTPUT_FORMAT_OK 400
/**
 * @brief Warning, output format : output format ambigous
 */
#define SPLT_OUTPUT_FORMAT_AMBIGUOUS 401

/**
 * @brief Error, output format : error occured while parsing the
 * output format
 */
#define SPLT_OUTPUT_FORMAT_ERROR -400

//miscellaneous error messages
#define SPLT_ERROR_INEXISTENT_SPLITPOINT -500


//plugin error messages
#define SPLT_ERROR_UNSUPPORTED_FEATURE -600

//miscellaneous messages that the library sends to the clients 
/**
 * @brief Infos, split : frame mode has been enabled
 */
#define SPLT_MESS_FRAME_MODE_ENABLED 0
#define SPLT_INTERNAL_PROGRESS_RATE 1
/**
 * @brief todo
 */
#define SPLT_MESS_START_WRAP_SPLIT 2
/**
 * @brief todo
 */
#define SPLT_MESS_START_SILENCE_SPLIT 3
/**
 * @brief todo
 */
#define SPLT_MESS_START_TIME_SPLIT 4
/**
 * @brief todo
 */
#define SPLT_MESS_START_ERROR_SPLIT 5
/**
 * @brief todo
 */
#define SPLT_MESS_START_NORMAL_SPLIT 6
/**
 * @brief todo
 */
#define SPLT_MESS_DETECTED 7
/**
 * @brief todo
 */
#define SPLT_MESS_DETECTED_INVALID 9
/**
 * @brief todo
 */
#define SPLT_MESS_STOP_SPLIT 10

//progress messages
/**
 * @brief Progress messages sent from the library to the client
 *
 * Progress messages sent from the library to the client
 */
typedef enum {
  /**
   * Preparing to split a song
   */
  SPLT_PROGRESS_PREPARE,
  /**
   * Creating the splited file
   */
  SPLT_PROGRESS_CREATE,
  /**
   * Searching for syncerrors
   */
  SPLT_PROGRESS_SEARCH_SYNC,
  /**
   * Scanning for silence
   */
  SPLT_PROGRESS_SCAN_SILENCE
} splt_progress_messages;

//options types : integer
/**
 * @brief Integer options
 *
 * Integer options
 *
 * Use #mp3splt_set_int_option to set those options\n
 * Use #mp3splt_get_int_option to get those options
 */
typedef enum {
  /**
   * If we print out debug messages
   *
   * The option can take the values #SPLT_TRUE or #SPLT_FALSE
   *
   * Default is #SPLT_FALSE
   */
  SPLT_OPT_DEBUG_MODE,
  /**
   * The type of the split
   *
   * The option can take the values from #splt_split_mode_options
   *
   * Default is #SPLT_OPTION_NORMAL_MODE
   */
  SPLT_OPT_SPLIT_MODE,
  /**
   * The type of tags to put in the new splitted files
   *
   * The option can take the values from #splt_tags_options
   *
   * Default is #SPLT_CURRENT_TAGS
   */
  SPLT_OPT_TAGS,
  /**
   * The option can take the values from #SPLT_OUTPUT_FILENAMES_OPTIONS
   *
   * Default is #SPLT_FALSE
   */
  SPLT_OPT_OUTPUT_FILENAMES,
  /**
   * If we enable the frame mode or not \n
   * The frame mode processes the file frame by frame and
   * it is useful when splitting a VBR (Variable Bit Rate) file
   * 
   * The option can take the values #SPLT_TRUE or #SPLT_FALSE
   *
   * Default is #SPLT_TRUE
   */
  SPLT_OPT_FRAME_MODE,
  /**
   * If we use silence detection to auto-adjust splitpoints\n
   * The following options may change the behaviour of the
   * auto-adjust : #SPLT_OPT_PARAM_THRESHOLD, #SPLT_OPT_PARAM_OFFSET,
   * #SPLT_OPT_PARAM_GAP, #
   *
   * The option can take the values #SPLT_TRUE or #SPLT_FALSE
   *
   * Default is #SPLT_FALSE
   */
  SPLT_OPT_AUTO_ADJUST,
  /**
   * If the input is not seekable\n
   * This allows you to split mp3 and ogg streams which can be read
   * only one time and can't be seeked
   *
   * The option can take the values #SPLT_TRUE or #SPLT_FALSE
   *
   * Default is #SPLT_FALSE
   */
  SPLT_OPT_INPUT_NOT_SEEKABLE,
  /**
   * The desired number of tracks when having a
   * #SPLT_OPTION_SILENCE_MODE split
   *
   * The option can take positive integer values. 0 means that we
   * split as many files we found
   *
   * Default is #SPLT_DEFAULT_PARAM_TRACKS
   */
  SPLT_OPT_PARAM_NUMBER_TRACKS,
  /**
   * Allows you to remove the silence between the splitted tracks when
   * having a #SPLT_OPTION_SILENCE_MODE split
   *
   * The option can take the values #SPLT_TRUE or #SPLT_FALSE
   *
   * Default is #SPLT_FALSE
   */
  SPLT_OPT_PARAM_REMOVE_SILENCE,
  /**
   * The time to auto-adjust before and after splitpoint
   * when having the #SPLT_OPT_AUTO_ADJUST option
   *
   * The option can take positive integer values
   *
   * Default is #SPLT_DEFAULT_PARAM_GAP
   */
  SPLT_OPT_PARAM_GAP,
  /**
   * if to set all tags like X one
   */
  SPLT_OPT_ALL_TAGS_LIKE_X_AFTER_X
} splt_int_options;

//option types : float
/**
 * @brief Float options
 *
 * Float options
 *
 * Use #mp3splt_set_float_option to set those options\n
 * Use #mp3splt_get_float_option to get those options
 */
typedef enum {
  /**
   * The interval for the #SPLT_OPTION_TIME_MODE split (in
   * hundreths of seconds)
   *
   * The option can take positive float values
   *
   * Default is 6000 hundreths of seconds (one minute)
   */
  SPLT_OPT_SPLIT_TIME,
  /**
   * The threshold  level (dB) to be considered silence\n
   * It is a float number between -96 and 0. It is used when
   * having a #SPLT_OPTION_SILENCE_MODE split or when having the
   * #SPLT_OPT_AUTO_ADJUST option
   *
   * The option can take float values between -96 and 0
   *
   * Default is #SPLT_DEFAULT_PARAM_THRESHOLD
   */
  SPLT_OPT_PARAM_THRESHOLD,
  /**
   * Allows you to adjust the offset of cutpoint in silence time when
   * having a #SPLT_OPTION_SILENCE_MODE split or when having the
   * #SPLT_OPT_AUTO_ADJUST option
   *
   * The option can take float values between -2 and 2\n
   * 0  is  the begin of silence, and 1 the end
   *
   * Default is #SPLT_DEFAULT_PARAM_OFFSET
   */
  SPLT_OPT_PARAM_OFFSET,
  /**
   * Minimum number of seconds to be considered a valid splitpoint\n
   * All silences shorter than this value are discarded.
   *
   * The option can take positive float values
   *
   * Default is #SPLT_DEFAULT_PARAM_MINIMUM_LENGTH
   */
  SPLT_OPT_PARAM_MIN_LENGTH
} splt_float_options;

/**
 * Freedb constants
 */

/*
 * freedb2 search type
 */
#define SPLT_FREEDB_SEARCH_TYPE_CDDB_CGI 1
/*
 * freedb search type
 */
#define SPLT_FREEDB_SEARCH_TYPE_CDDB 2
/*
 * freedb get file type
 * we retrieve the file by using the cddb.cgi script
 * (usually on port 80)
 */
#define SPLT_FREEDB_GET_FILE_TYPE_CDDB_CGI 3
/*
 * we retrieve the file by using the freedb cddb protocol 
 * (usually on port 8880)
 */
#define SPLT_FREEDB_GET_FILE_TYPE_CDDB 4
/**
 * default port
 */
#define SPLT_FREEDB_CDDB_CGI_PORT 80
/**
 * default port
 */
#define SPLT_FREEDB_CDDB_PORT 8880
/**
 * urls of freedb2.org and freedb.org
 */
#define SPLT_FREEDB_CGI_SITE "freedb.org/~cddb/cddb.cgi"
#define SPLT_FREEDB2_CGI_SITE "freedb2.org/~cddb/cddb.cgi"

//package information constants
#ifndef SPLT_PACKAGE_NAME
/**
 * @brief Package name
 */
#define SPLT_PACKAGE_NAME "libmp3splt"
#endif
#ifndef SPLT_PACKAGE_VERSION
/**
 * @brief Package version
 */
#define SPLT_PACKAGE_VERSION "0.4_rc1"
#endif
/**
 * @brief Package authors
 */
#define SPLT_AUTHOR "Matteo Trotta | Munteanu Alexandru"
#define SPLT_EMAIL "<mtrotta@users.sourceforge.net> | <io_alex_2002@yahoo.fr>"
/**
 * @brief Package website
 */
#define SPLT_WEBSITE "http://mp3splt.sourceforge.net"


/**
 * @file mp3splt.h
 * 
 * @brief main functions
 * 
 * Contains the library API functions.
 */

/**
 * Initialisation and free
 */

/**
 * @brief Creates a new state structure
 *
 * Creates a new state structure, needed by libmp3splt
 */
splt_state *mp3splt_new_state(int *error);

//find plugins
int mp3splt_find_plugins(splt_state *state);

//this function frees the left variables in the library
//don't forget to call this function ONLY at the end of the program
//returns possible error
void mp3splt_free_state(splt_state *state, int *error);

/************************************/
/* Set path                         */

//puts the path for the new splitted files
//returns possible error
int mp3splt_set_path_of_split(splt_state *state, char *path);

/************************************/
/* Set filename                     */

//put the filename to split
//returns possible error
int mp3splt_set_filename_to_split(splt_state *state, char *filename);
int mp3splt_set_m3u_filename(splt_state *state, char *filename);

/************************************/
/* Set callback functions           */

int mp3splt_set_message_function(splt_state *state,
    void (*put_message)(int));
int mp3splt_set_splitted_filename_function(splt_state *state,
    void (*file_cb)(char *,int));
int mp3splt_set_progress_function(splt_state *state,
    void (*progress_cb)(splt_progress *p_bar));

/************************************/
/* Splitpoints                      */

//puts a splitpoint
//returns possible error
int mp3splt_append_splitpoint(splt_state *state,
    long split_value, char *name);

//returns a pointer to all the current splitpoints
splt_point *mp3splt_get_splitpoints(splt_state *state,
    int *splitpoints_number,
    int *error);

//erase all the splitpoints
void mp3splt_erase_all_splitpoints(splt_state *state,
    int *error);

/************************************/
/* Tags                             */

//puts a tag
int mp3splt_append_tags(splt_state *state, 
    char *title, char *artist,
    char *album, char *performer,
    char *year, char *comment,
    int track, unsigned char genre);

//returns a pointer to all the current tags
splt_tags *mp3splt_get_tags(splt_state *state,
    int *tags_number,
    int *error);

//puts tags from a string
int mp3splt_put_tags_from_string(splt_state *state, 
    char *tags);

void mp3splt_erase_all_tags(splt_state *state,
    int *error);

/************************************/
/* Options                          */

int mp3splt_set_int_option(splt_state *state, int option_name,
    int value);

int mp3splt_set_float_option(splt_state *state, int option_name,
    float value);

int mp3splt_get_int_option(splt_state *state, int option_name,
    int *error);

float mp3splt_get_float_option(splt_state *state, int option_name,
    int *error);

/************************************/
/* Split functions                  */

//split a ogg or mp3 file
//returns possible error
int mp3splt_split(splt_state *state);

//cancel split function
//returns possible error
void mp3splt_stop_split(splt_state *state,
    int *error);

/************************************/
/*    Cddb and Cue functions        */

//get the cue splitpoints from a file and puts them in the state
void mp3splt_put_cue_splitpoints_from_file(splt_state *state,
    char *cue_file,
    int *error);

//read cddb splitpoints from file and puts them in the state
void mp3splt_put_cddb_splitpoints_from_file(splt_state *state,
    char *cddb_file,
    int *error);

/************************************/
/*    Freedb functions              */

//returns the freedb results and possible eerror
/**
 * @brief test
 */
splt_freedb_results *mp3splt_get_freedb_search(splt_state *state,
    char *searched_string,
    int *error,
    int search_type,
    char search_server[256],
    int port);

void mp3splt_write_freedb_file_result(splt_state *state,
    int disc_id,
    char *cddb_file,
    int *error,
    int cddb_get_type,
    char cddb_get_server[256],
    int port);

//string s is freed, call with strdup for example
void mp3splt_set_oformat(splt_state *state,
    char *format_string,
    int *error);

/************************************/
/* Other utilities                  */

//returns the version of libmp3splt
void mp3splt_get_version(char *version);

//returns the number of syncerrors
//puts possible error in error variable
splt_syncerrors *mp3splt_get_syncerrors(splt_state *state,
    int *error);

//returns the wrapped files found
splt_wrap *mp3splt_get_wrap_files(splt_state *state,
    int *error);

//count how many silence splitpoints we have with silence detection
int mp3splt_count_silence_points(splt_state *state, int *error);

#define MP3SPLT_MP3SPLT_H

#endif
