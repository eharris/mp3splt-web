#!/bin/bash

. ./utils.sh || exit 1

#normal mode functional tests

function test_normal_vbr
{
  local tags_version=$1

  remove_output_dir

  M_FILE="La_Verue__Today"

  if [[ $tags_version -eq -1 ]];then
    tags_option="-n"
    test_name="vbr no tags"
  else
    test_name="vbr id3v$tags_version"
    tags_option="-T $tags_version"
  fi

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  mp3splt_args="$tags_option -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing

  if [[ $tags_version -eq -1 ]];then
    check_current_mp3_no_tags
    check_current_file_size "1366334"
  else
    check_all_mp3_tags_with_version $tags_version "La Verue" "Riez Noir" "Today"\
    "2007" "Rock" "17" "1" "http://www.jamendo.com/"

    if [[ $tags_version -eq 2 ]];then
      check_current_file_size "1412518"
    else
      check_current_file_size "1366462"
    fi
  fi

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3" 
  check_current_mp3_length "01.04"
  check_current_file_has_xing

  if [[ $tags_version -eq -1 ]];then
    check_current_mp3_no_tags
    check_current_file_size "1521748"
  else
    check_all_mp3_tags_with_version $tags_version "La Verue" "Riez Noir"\
    "Today" "2007" "Rock" "17" "2" "http://www.jamendo.com/"

    if [[ $tags_version -eq 2 ]];then
      check_current_file_size "1567932"
    else
      check_current_file_size "1521876"
    fi
  fi

  current_file="$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing

  if [[ $tags_version -eq -1 ]];then
    check_current_mp3_no_tags
    check_current_file_size "1399171"
  else
    check_all_mp3_tags_with_version $tags_version "La Verue" "Riez Noir" "Today"\
      "2007" "Rock" "17" "3" "http://www.jamendo.com/"

    if [[ $tags_version -eq 2 ]];then
      check_current_file_size "1445355"
    else
      check_current_file_size "1399299"
    fi
  fi

  print_ok
  echo
}

function test_normal_vbr_no_tags { test_normal_vbr -1; }
function test_normal_vbr_id3v1 { test_normal_vbr 1; }
function test_normal_vbr_id3v2 { test_normal_vbr 2; }

function test_normal_vbr_pretend
{
  remove_output_dir

  test_name="vbr pretend"
  M_FILE="La_Verue__Today"

  expected=" Pretending to split file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  mp3splt_args="-P -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_is_empty

  print_ok
  echo
}

function test_normal_vbr_cue_export
{
  remove_output_dir

  test_name="vbr & cue export"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)
 CUE file 'output/output_out.cue' created."
  mp3splt_args="-T 2 -E output/out.cue -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_number_of_files 4

  check_file_content "output/output_out.cue" 'TITLE "Riez Noir"
PERFORMER "La Verue"
FILE "songs/La_Verue__Today.mp3" MP3
  TRACK 01 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    INDEX 01 01:00:00
  TRACK 02 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    INDEX 01 02:00:20
  TRACK 03 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    INDEX 01 03:05:00'

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "1412518"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3" 
  check_current_mp3_length "01.04"
  check_current_file_has_xing
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir"\
  "Today" "2007" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "1567932"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "3" "http://www.jamendo.com/"
  check_current_file_size "1445355"

  print_ok
  echo
}

function test_normal_vbr_pretend_and_cue_export
{
  remove_output_dir

  test_name="vbr pretend & cue export"
  M_FILE="La_Verue__Today"

  expected=" Pretending to split file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)
 CUE file 'output/output_out.cue' created."
  mp3splt_args="-E output/out.cue -P -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"
 
  check_output_directory_number_of_files 1

  check_file_content "output/output_out.cue" 'TITLE "Riez Noir"
PERFORMER "La Verue"
FILE "songs/La_Verue__Today.mp3" MP3
  TRACK 01 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    INDEX 01 01:00:00
  TRACK 02 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    INDEX 01 02:00:20
  TRACK 03 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    INDEX 01 03:05:00'

  print_ok
  echo
}

function test_normal_vbr_overlap_split
{
  remove_output_dir

  test_name="vbr overlap splitpoints"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
 info: overlapping split files with 0.30.0
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_30s_20h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__04m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_30s__04m_05s_58h.mp3\" created
 file split (EOF)"
  mp3splt_args="-T 2 -O 0.30 -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.30 EOF"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_30s_20h.mp3"
  check_current_mp3_length "01.30"
  check_current_file_size "2054691"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__04m_00s.mp3"
  check_current_mp3_length "01.59"
  check_current_file_size "2869690"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_30s__04m_05s_58h.mp3"
  check_current_mp3_length "00.35"
  check_current_file_size "855248"

  print_ok
  echo
}

function test_normal_vbr_overlap_and_cue_export
{
  remove_output_dir

  test_name="vbr overlap & cue export"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
 info: overlapping split files with 0.30.0
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_30s_20h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__04m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_30s__04m_05s_58h.mp3\" created
 file split (EOF)
 CUE file 'output/output_out.cue' created."
  mp3splt_args="-T 2 -E output/out.cue -O 0.30 -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.30 EOF"
  run_check_output "$mp3splt_args" "$expected"

  #TODO: what to output as CUE ?
  check_file_content 'output/output_out.cue' 'TITLE "Riez Noir"
PERFORMER "La Verue"
FILE "songs/La_Verue__Today.mp3" MP3
  TRACK 01 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    INDEX 01 01:00:00
  TRACK 02 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    INDEX 01 02:00:20
  TRACK 03 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    INDEX 01 03:30:00'

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_30s_20h.mp3"
  check_current_mp3_length "01.30"
  check_current_file_size "2054691"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__04m_00s.mp3"
  check_current_mp3_length "01.59"
  check_current_file_size "2869690"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_30s__04m_05s_58h.mp3"
  check_current_mp3_length "00.35"
  check_current_file_size "855248"

  print_ok
  echo
}

function test_normal_vbr_m3u
{
  remove_output_dir

  test_name="vbr m3u"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 M3U file '$OUTPUT_DIR/m3u/playlist.m3u' will be created.
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/m3u/${M_FILE}_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/m3u/${M_FILE}_02m_00s_20h__03m_05s.mp3\" created
   File \"$OUTPUT_DIR/m3u/${M_FILE}_03m_05s__04m_05s_58h.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  mp3splt_args="-m playlist.m3u -d $OUTPUT_DIR/m3u $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  expected="La_Verue__Today_01m_00s__02m_00s_20h.mp3
La_Verue__Today_02m_00s_20h__03m_05s.mp3
La_Verue__Today_03m_05s__04m_05s_58h.mp3"
  check_file_content "$OUTPUT_DIR/m3u/playlist.m3u" "$expected"

  print_ok
  echo
}

function test_normal_vbr_pretend_and_m3u
{
  remove_output_dir

  test_name="vbr pretend & m3u"
  M_FILE="La_Verue__Today"

  expected=" Pretending to split file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 M3U file 'output/output_out.m3u' will be created.
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  mp3splt_args="-m output/out.m3u -P -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"
 
  check_output_directory_number_of_files 0

  print_ok
  echo
}

function test_normal_vbr_default_tags
{
  remove_output_dir

  test_name="vbr normal"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "1 2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "1412646"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3" 
  check_current_mp3_length "01.04"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "1 2" "La Verue" "Riez Noir"\
  "Today" "2007" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "1568060"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "1 2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "3" "http://www.jamendo.com/"
  check_current_file_size "1445483"

  print_ok
  echo
}

function test_normal_vbr_original_tags
{
  remove_output_dir

  test_name="vbr original tags %[@o]"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  mp3splt_args="-g %[@o] -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="songs/${M_FILE}.mp3"
  #8/14: we don't support yet the total tracknumber
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "8/14" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3" 
  check_all_mp3_tags_with_version "1 2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "8" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3" 
  check_all_mp3_tags_with_version "1 2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "8" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3" 
  check_all_mp3_tags_with_version "1 2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "8" "http://www.jamendo.com/"

  print_ok
  echo
}

function test_normal_vbr_id3v1_and_id3v2
{
  remove_output_dir

  M_FILE="La_Verue__Today"

  test_name="vbr id3v1 and id3v2"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  mp3splt_args="-T 12 -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing
  check_current_file_size "1412646"
  check_all_mp3_tags_with_version "1 2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3" 
  check_current_mp3_length "01.04"
  check_current_file_has_xing
  check_current_file_size "1568060"
  check_all_mp3_tags_with_version "1 2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "2" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing
  check_current_file_size "1445483"
  check_all_mp3_tags_with_version "1 2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "3" "http://www.jamendo.com/"

  print_ok
  echo
}

function test_normal_vbr_no_input_tags
{
  remove_output_dir

  test_name="vbr normal & no input tags"
  M_FILE="La_Verue__Today__no_tags"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR $NO_TAGS_MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing
  check_current_mp3_no_tags
  check_current_file_size "1366334"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3" 
  check_current_mp3_length "01.04"
  check_current_file_has_xing
  check_current_mp3_no_tags
  check_current_file_size "1521748"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing
  check_current_mp3_no_tags
  check_current_file_size "1399171"

  print_ok
  echo
}

function test_normal_vbr_no_xing
{
  remove_output_dir

  test_name="vbr no xing"
  M_FILE="La_Verue__Today"

  mp3splt_args="-x -T 2 -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" ""

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3" 
  check_current_mp3_length "01.08"
  check_current_file_has_no_xing
  check_current_file_size "1412101"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3" 
  check_current_mp3_length "01.16"
  check_current_file_has_no_xing
  check_current_file_size "1567515"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3" 
  check_current_mp3_length "00.58"
  check_current_file_has_no_xing
  check_current_file_size "1444938"

  print_ok
  echo
}

function test_normal_vbr_custom_tags
{
  remove_output_dir

  test_name="vbr custom tags"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_00m_05s__01m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__01m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_01m_05s__02m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_00s__03m_05s.mp3\" created
 Processed 7084 frames - Sync errors: 0
 file split"
  tags_option="[@a=a1,@b=b1,@t=t1,@y=2000,@c=my_comment,@n=10,@g=Slow Rock][]%[@o,@b=album,@N=7,@g=Humour][@a=custom_artist][@o,@n=20]"
  mp3splt_args="-d $OUTPUT_DIR -g \"$tags_option\" $MP3_FILE 0.5 1.0 1.5 2.0 3.0 3.5"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_00m_05s__01m_00s.mp3"
  check_all_mp3_tags_with_version "2" "a1" "b1" "t1" "2000"\
  "Slow Rock" "95" "10" "my_comment"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__01m_05s.mp3"
  check_current_mp3_no_tags

  current_file="$OUTPUT_DIR/${M_FILE}_01m_05s__02m_00s.mp3"
  check_all_mp3_tags_with_version "2" "La Verue" "album" "Today"\
  "2007" "Humour" "100" "7" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3"
  check_all_mp3_tags_with_version "2" "custom_artist" "album" "Today"\
  "2007" "Humour" "100" "8" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_00s__03m_05s.mp3"
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "20" "http://www.jamendo.com/"

  print_ok
  echo
}

function test_normal_vbr_custom_tags_id3v1
{
  remove_output_dir

  test_name="vbr custom tags & id3v1"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_00m_05s__01m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__01m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_01m_05s__02m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_00s__03m_05s.mp3\" created
 Processed 7084 frames - Sync errors: 0
 file split"
  tags_option="[@a=a1,@b=b1,@t=t1,@y=2000,@c=my_comment,@n=10,@g=Country][]%[@o,@b=album,@N=7,@g=Humour][@a=custom_artist,@g=doesnotexists][@o,@n=20]"
  mp3splt_args="-d $OUTPUT_DIR -T 1 -g \"$tags_option\" $MP3_FILE 0.5 1.0 1.5 2.0 3.0 3.5"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_00m_05s__01m_00s.mp3"
  check_all_mp3_tags_with_version "1" "a1" "b1" "t1" "2000"\
  "Country" "2" "10" "my_comment"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__01m_05s.mp3"
  check_current_mp3_no_tags

  current_file="$OUTPUT_DIR/${M_FILE}_01m_05s__02m_00s.mp3"
  check_all_mp3_tags_with_version "1" "La Verue" "album" "Today"\
  "2007" "Humour" "100" "7" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3"
  check_all_mp3_tags_with_version "1" "custom_artist" "album" "Today"\
  "2007" "Other" "12" "8" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_00s__03m_05s.mp3"
  check_all_mp3_tags_with_version "1" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "20" "http://www.jamendo.com/"

  print_ok
  echo
}


function test_normal_vbr_custom_tags_and_cue_export
{
  remove_output_dir

  test_name="vbr custom tags & cue export"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_00m_05s__01m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__01m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_01m_05s__02m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_00s__03m_05s.mp3\" created
 Processed 7084 frames - Sync errors: 0
 file split
 CUE file 'output/output_out.cue' created."
  tags_option="[@a=a1,@b=b1,@t=t1,@y=2000,@c=my_comment,@n=10][]%[@o,@b=album,@N=7][@a=custom_artist][@o,@n=20]"
  mp3splt_args="-E output/out.cue -d $OUTPUT_DIR -g \"$tags_option\" $MP3_FILE 0.5 1.0 1.5 2.0 3.0 3.5"
  run_check_output "$mp3splt_args" "$expected"

  check_file_content "output/output_out.cue" 'TITLE "b1"
PERFORMER "a1"
FILE "songs/La_Verue__Today.mp3" MP3
  TRACK 01 AUDIO
    TITLE "t1"
    PERFORMER "a1"
    INDEX 01 00:05:00
  TRACK 02 AUDIO
    INDEX 01 01:00:00
  TRACK 03 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    INDEX 01 01:05:00
  TRACK 04 AUDIO
    TITLE "Today"
    PERFORMER "custom_artist"
    INDEX 01 02:00:00
  TRACK 05 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    INDEX 01 03:00:00
  TRACK 06 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    INDEX 01 03:05:00'

  current_file="$OUTPUT_DIR/${M_FILE}_00m_05s__01m_00s.mp3"
  check_all_mp3_tags_with_version "2" "a1" "b1" "t1"\
  "2000" "" "" "10" "my_comment"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__01m_05s.mp3"
  check_current_mp3_no_tags

  current_file="$OUTPUT_DIR/${M_FILE}_01m_05s__02m_00s.mp3"
  check_all_mp3_tags_with_version "2" "La Verue" "album" "Today"\
  "2007" "Rock" "17" "7" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3"
  check_all_mp3_tags_with_version "2" "custom_artist" "album" "Today"\
  "2007" "Rock" "17" "8" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_00s__03m_05s.mp3"
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "20" "http://www.jamendo.com/"

  print_ok
  echo
}

function test_normal_vbr_custom_tags_and_input_no_tags
{
  remove_output_dir

  test_name="vbr custom tags & no input tags"
  M_FILE="La_Verue__Today__no_tags"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_00m_05s__01m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__01m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_01m_05s__02m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_00s__03m_05s.mp3\" created
 Processed 7084 frames - Sync errors: 0
 file split"
  tags_option="[@a=a1,@b=b1,@t=t1,@y=2000,@c=my_comment,@n=10][]%[@o,@b=album,@N=7][@a=custom_artist][@o,@n=20]"
  mp3splt_args="-d $OUTPUT_DIR -g \"$tags_option\" $NO_TAGS_MP3_FILE 0.5 1.0 1.5 2.0 3.0 3.5"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_00m_05s__01m_00s.mp3"
  check_all_mp3_tags_with_version "1 2" "a1" "b1" "t1"\
  "2000" "" "" "10" "my_comment"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__01m_05s.mp3"
  check_current_mp3_no_tags

  current_file="$OUTPUT_DIR/${M_FILE}_01m_05s__02m_00s.mp3"
  check_all_mp3_tags_with_version "1 2" "" "album" "" "" "" "" "7" ""

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3"
  check_all_mp3_tags_with_version "1 2" "custom_artist" "album" "" ""\
  "" "" "8" ""

  current_file="$OUTPUT_DIR/${M_FILE}_03m_00s__03m_05s.mp3"
  check_all_mp3_tags_with_version "1 2" "" "" "" "" "" "" "20" ""

  print_ok
  echo
}

function test_normal_vbr_custom_tags_multiple_percent
{
  remove_output_dir

  test_name="vbr custom tags multiple percent"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_00m_05s__01m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__01m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_01m_05s__02m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_00s__03m_05s.mp3\" created
 Processed 7084 frames - Sync errors: 0
 file split"
  tags_option="%[@a=a1,@b=b1,@n=10][]%[@o,@b=album,@N=7][@a=custom_artist][@o,@n=20]"
  mp3splt_args="-d $OUTPUT_DIR -g \"$tags_option\" $MP3_FILE 0.5 1.0 1.5 2.0 3.0 3.5"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_00m_05s__01m_00s.mp3"
  check_all_mp3_tags_with_version "2" "a1" "b1" "" "" "" "" "10" ""

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__01m_05s.mp3"
  check_all_mp3_tags_with_version "2" "a1" "b1" "" "" "" "" "10" ""

  current_file="$OUTPUT_DIR/${M_FILE}_01m_05s__02m_00s.mp3"
  check_all_mp3_tags_with_version "2" "La Verue" "album" "Today"\
  "2007" "Rock" "17" "7" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3"
  check_all_mp3_tags_with_version "2" "custom_artist" "album" "Today"\
  "2007" "Rock" "17" "8" "http://www.jamendo.com/"

  print_ok
  echo
}

function test_normal_vbr_stdin
{
  no_tags_file=$1

  remove_output_dir

  if [[ -z $no_tags_file ]];then
    test_name="vbr stdin"
    M_FILE="La_Verue__Today"
    sync_errors=1
    frames=9400
    last_file_size=806141
  elif [[ $no_tags_file -eq "no_input_tags" ]];then
    test_name="vbr stdin & no input tags"
    M_FILE="La_Verue__Today__no_tags"
    frames=9399
    sync_errors=0
    last_file_size=806037
  fi

  expected=" Processing file '-' ...
 info: file matches the plugin 'mp3 (libmad)'
 warning: stdin '-' is supposed to be mp3 stream.
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE NS - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/-_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/-_02m_00s_20h__03m_30s.mp3\" created
   File \"$OUTPUT_DIR/-_03m_30s__04m_05s_58h.mp3\" created
 Processed $frames frames - Sync errors: $sync_errors
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR - 1.0 2.0.2 3.30 EOF"
  run_custom_check_output "cat songs/${M_FILE}.mp3 | $MP3SPLT" "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/-_01m_00s__02m_00s_20h.mp3"
  check_current_mp3_length "01.00"
  check_current_mp3_no_tags
  check_current_file_size "1365917"

  current_file="$OUTPUT_DIR/-_02m_00s_20h__03m_30s.mp3"
  check_current_mp3_length "01.29"
  check_current_mp3_no_tags
  check_current_file_size "2113734"

  current_file="$OUTPUT_DIR/-_03m_30s__04m_05s_58h.mp3"
  check_current_mp3_length "00.35"
  check_current_mp3_no_tags
  check_current_file_size "$last_file_size"

  print_ok
  echo
}

function test_normal_vbr_stdin_no_input_tags { test_normal_vbr_stdin "no_input_tags"; }

function _test_normal_vbr_stdin_and_tags
{
  local tags_version=$1

  remove_output_dir

  test_name="vbr stdin and tags v$tags_version"
  M_FILE="La_Verue__Today"

  expected=" Processing file '-' ...
 info: file matches the plugin 'mp3 (libmad)'
 warning: stdin '-' is supposed to be mp3 stream.
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE NS - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/-_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/-_02m_00s_20h__03m_30s.mp3\" created
   File \"$OUTPUT_DIR/-_03m_30s__04m_05s_58h.mp3\" created
 Processed 9400 frames - Sync errors: 1
 file split (EOF)"
  mp3splt_args="-g %[@a=a1,@b=b1,@y=1070,@N=1] -T $tags_version -d $OUTPUT_DIR - 1.0 2.0.2 3.30 EOF"
  run_custom_check_output "cat songs/${M_FILE}.mp3 | $MP3SPLT" "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/-_01m_00s__02m_00s_20h.mp3"
  check_all_mp3_tags_with_version $tags_version "a1" "b1" "" "1070"\
  "" "" "1" ""
  check_current_mp3_length "01.00"
  check_current_file_has_xing
  if [[ $tags_version -eq 2 ]];then
    check_current_file_size "1365997"
  else
    check_current_file_size "1366045"
  fi

  current_file="$OUTPUT_DIR/-_02m_00s_20h__03m_30s.mp3"
  check_all_mp3_tags_with_version $tags_version "a1" "b1" "" "1070"\
  "" "" "2" ""
  check_current_mp3_length "01.29"
  check_current_file_has_xing
  if [[ $tags_version -eq 2 ]];then
    check_current_file_size "2113814"
  else
    check_current_file_size "2113862"
  fi

  current_file="$OUTPUT_DIR/-_03m_30s__04m_05s_58h.mp3"
  check_all_mp3_tags_with_version $tags_version "a1" "b1" "" "1070"\
  "" "" "3" ""
  check_current_mp3_length "00.35"
  check_current_file_has_xing
  if [[ $tags_version -eq 2 ]];then
    check_current_file_size "806221"
  else
    check_current_file_size "806269"
  fi

  print_ok
  echo
}

function test_normal_vbr_stdin_and_tags_v1 { _test_normal_vbr_stdin_and_tags 1; }
function test_normal_vbr_stdin_and_tags_v2 { _test_normal_vbr_stdin_and_tags 2; }

function test_normal_vbr_output_fname
{
  remove_output_dir

  test_name="vbr output fname"
  M_FILE="La_Verue__Today"

  expected=" warning: output format ambiguous (@t or @n missing)
 Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/test.mp3\" created
 Processed 4595 frames - Sync errors: 0
 file split"
  mp3splt_args="-o 'test' -d $OUTPUT_DIR $MP3_FILE 1.0 2.0" 
  run_check_output "$mp3splt_args" "$expected"

  check_if_file_exist "$OUTPUT_DIR/test.mp3"

  current_file="$OUTPUT_DIR/test.mp3" 
  check_current_mp3_length "01.00"

  print_ok
  echo
}

function test_normal_vbr_output_fnames_and_custom_tags
{
  remove_output_dir

  test_name="vbr output fnames & custom tags"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/a1_b1_t1_1_10_${M_FILE} 00:05:00 01:00:30.mp3\" created
   File \"$OUTPUT_DIR/___2_2_${M_FILE} 01:00:30 01:05:00.mp3\" created
   File \"$OUTPUT_DIR/La Verue_album_Today_3_7_${M_FILE} 01:05:00 02:00:00.mp3\" created
 Processed 4595 frames - Sync errors: 0
 file split"
  tags_option="[@a=a1,@b=b1,@t=t1,@y=2000,@c=my_comment,@n=10][]%[@o,@b=album,@N=7]"
  output_option="\"@a_@b_@t_@n_@N_@f+@m:@s:@h @M:@S:@H\""
  mp3splt_args="-d $OUTPUT_DIR -g \"$tags_option\" -o $output_option $MP3_FILE 0.5 1.0.30 1.5 2.0"
  run_check_output "$mp3splt_args" "$expected"

  check_if_file_exist "$OUTPUT_DIR/a1_b1_t1_1_10_${M_FILE} 00:05:00 01:00:30.mp3"
  check_if_file_exist "$OUTPUT_DIR/___2_2_${M_FILE} 01:00:30 01:05:00.mp3"
  check_if_file_exist "$OUTPUT_DIR/La Verue_album_Today_3_7_${M_FILE} 01:05:00 02:00:00.mp3"

  print_ok
  echo
}

function test_normal_vbr_output_fnames_and_dirs
{
  remove_output_dir

  test_name="vbr output fnames & directories"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/La Verue/Riez Noir/La Verue-Today-Rock 1.mp3\" created
   File \"$OUTPUT_DIR/La Verue/Riez Noir/La Verue-Today-Rock 2.mp3\" created
   File \"$OUTPUT_DIR/La Verue/Riez Noir/La Verue-Today-Rock 3.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  output_option="@a/@b/@a-@t-@g @n"
  mp3splt_args="-o '$output_option' -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  check_if_directory_exist "$OUTPUT_DIR/La Verue"
  check_if_directory_exist "$OUTPUT_DIR/La Verue/Riez Noir"
  check_if_file_exist "$OUTPUT_DIR/La Verue/Riez Noir/La Verue-Today-Rock 1.mp3"
  check_if_file_exist "$OUTPUT_DIR/La Verue/Riez Noir/La Verue-Today-Rock 2.mp3"
  check_if_file_exist "$OUTPUT_DIR/La Verue/Riez Noir/La Verue-Today-Rock 3.mp3"

  print_ok
  echo
}

function test_normal_vbr_output_fnames_and_custom_tags_and_dirs
{
  remove_output_dir

  test_name="vbr output fnames & custom tags & directories"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/La Verue/album1/La Verue-Today-Rock 1.mp3\" created
   File \"$OUTPUT_DIR/La Verue/album2/La Verue-Today-Rock 2.mp3\" created
   File \"$OUTPUT_DIR/La Verue/album3/La Verue-Today-Soundtrack 3.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  output_option="@a/@b/@a-@t-@g @n"
  tags_option="%[@o,@b=album1][@b=album2][@b=album3,@g=Soundtrack]"
  mp3splt_args="-o '$output_option' -g \"$tags_option\" -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  check_if_directory_exist "$OUTPUT_DIR/La Verue"
  check_if_directory_exist "$OUTPUT_DIR/La Verue/album1"
  check_if_directory_exist "$OUTPUT_DIR/La Verue/album2"
  check_if_directory_exist "$OUTPUT_DIR/La Verue/album3"
  check_if_file_exist "$OUTPUT_DIR/La Verue/album1/La Verue-Today-Rock 1.mp3"
  check_if_file_exist "$OUTPUT_DIR/La Verue/album2/La Verue-Today-Rock 2.mp3"
  check_if_file_exist "$OUTPUT_DIR/La Verue/album3/La Verue-Today-Soundtrack 3.mp3"

  print_ok
  echo
}

function test_normal_vbr_stdout
{
  remove_output_dir

  test_name="vbr stdout"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"-\" created
 Processed 5751 frames - Sync errors: 0
 file split"
  mp3splt_args="-T 2 -o - $MP3_FILE 1.0 2.30.2"
  run_custom_check_output "$MP3SPLT $mp3splt_args > $OUTPUT_DIR/stdout.mp3" "" "$expected"

  current_file="$OUTPUT_DIR/stdout.mp3"
  check_current_mp3_length "01.30"
  check_current_file_size "2054691"

  print_ok
  echo
}

function test_normal_vbr_stdout_multiple_splitpoints
{
  remove_output_dir

  test_name="vbr stdout & splitpoints > 2"
  M_FILE="La_Verue__Today"

  expected=" Warning: multiple splitpoints with stdout !
 Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"-\" created
   File \"-\" created
 Processed 6509 frames - Sync errors: 0
 file split"
  mp3splt_args="-T 2 -o - $MP3_FILE 1.0 2.30.2 2.50"
  run_custom_check_output "$MP3SPLT $mp3splt_args > $OUTPUT_DIR/stdout.mp3" "" "$expected"

  current_file="$OUTPUT_DIR/stdout.mp3"
#TODO: 2 outputs are concatenated in the same file ? should we do something ?
  check_current_mp3_length "01.30"
  check_current_file_size "2582245"

  print_ok
  echo
}

function test_normal_vbr_custom_tags_with_replace_tags_in_tags
{
  remove_output_dir

  test_name="vbr custom tags & replace tags in tags"
  M_FILE="La_Verue__Today"

  F1="-a_10-b_a_@n-10.mp3"
  F2="Today-La Verue-album_cc_@t-7.mp3"
  F3="Today-La Verue-album_cc_@t-8.mp3"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/$F1\" created
   File \"$OUTPUT_DIR/$F2\" created
   File \"$OUTPUT_DIR/$F3\" created
 Processed 4595 frames - Sync errors: 0
 file split"
  tags_option="r[@a=a_@n,@b=b_@a,@c=cc_@b,@n=10]%[@o,@c=cc_@t,@b=album_@c,@N=7]"
  output_option="@t-@a-@b-@N"
  mp3splt_args="-d $OUTPUT_DIR -o $output_option -g \"$tags_option\" $MP3_FILE 0.5 1.0 1.5 2.0"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/$F1"
  check_all_mp3_tags_with_version "2" "a_10" "b_a_@n" "" "" "" "" "10" "cc_b_@a"

  current_file="$OUTPUT_DIR/$F2"
  check_all_mp3_tags_with_version "2" "La Verue" "album_cc_@t" "Today" "2007"\
  "Rock" "17" "7" "cc_Today"

  current_file="$OUTPUT_DIR/$F3"
  check_all_mp3_tags_with_version "2" "La Verue" "album_cc_@t" "Today" "2007"\
  "Rock" "17" "8" "cc_Today"

  print_ok
  echo
}

function test_normal_vbr_custom_tags_without_replace_tags_in_tags
{
  remove_output_dir

  test_name="vbr custom tags without replace tags in tags"
  M_FILE="La_Verue__Today"

  F1="-a_@n-b_@a-10.mp3"
  F2="Today-La Verue-album_@c-7.mp3"
  F3="Today-La Verue-album_@c-8.mp3"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/$F1\" created
   File \"$OUTPUT_DIR/$F2\" created
   File \"$OUTPUT_DIR/$F3\" created
 Processed 4595 frames - Sync errors: 0
 file split"
  tags_option="[@a=a_@n,@b=b_@a,@c=cc_@b,@n=10]%[@o,@c=cc_@t,@b=album_@c,@N=7]"
  output_option="@t-@a-@b-@N"
  mp3splt_args="-d $OUTPUT_DIR -o $output_option -g \"$tags_option\" $MP3_FILE 0.5 1.0 1.5 2.0"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/$F1"
  check_all_mp3_tags_with_version "2" "a_@n" "b_@a" "" ""\
  "" "" "10" "cc_@b"

  current_file="$OUTPUT_DIR/$F2"
  check_all_mp3_tags_with_version "2" "La Verue" "album_@c" "Today" "2007"\
  "Rock" "17" "7" "cc_@t"

  current_file="$OUTPUT_DIR/$F3"
  check_all_mp3_tags_with_version "2" "La Verue" "album_@c" "Today" "2007"\
  "Rock" "17" "8" "cc_@t"

  print_ok
  echo
}

function test_normal_vbr_custom_empty_tags
{
  remove_output_dir

  test_name="vbr custom tags empty tags"
  M_FILE="La_Verue__Today"

  F1="1.mp3"
  F2="2.mp3"
  F3="3.mp3"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/$F1\" created
   File \"$OUTPUT_DIR/$F2\" created
   File \"$OUTPUT_DIR/$F3\" created
 Processed 4595 frames - Sync errors: 0
 file split"
  output_option="@n"
  mp3splt_args="-d $OUTPUT_DIR -o $output_option -g \"%[]\" $MP3_FILE 0.5 1.0 1.5 2.0"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/$F1"
  check_current_mp3_no_tags

  current_file="$OUTPUT_DIR/$F2"
  check_current_mp3_no_tags

  current_file="$OUTPUT_DIR/$F3"
  check_current_mp3_no_tags

  print_ok
  echo
}

function test_normal_vbr_split_in_equal_parts
{
  remove_output_dir

  test_name="vbr split in equal parts"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting 'split in equal tracks' mode
   File \"$OUTPUT_DIR/1.mp3\" created
   File \"$OUTPUT_DIR/2.mp3\" created
   File \"$OUTPUT_DIR/3.mp3\" created
   File \"$OUTPUT_DIR/4.mp3\" created
 Processed 9402 frames - Sync errors: 0
 split in equal tracks ok"
  mp3splt_args="-d $OUTPUT_DIR -o @n -S 4 $MP3_FILE"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/1.mp3" 
  check_current_mp3_length "01.01"
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir"\
  "Today" "2007" "Rock" "17" "1" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/2.mp3" 
  check_current_mp3_length "01.01"
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir"\
  "Today" "2007" "Rock" "17" "2" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/3.mp3" 
  check_current_mp3_length "01.01"
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir"\
  "Today" "2007" "Rock" "17" "3" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/4.mp3" 
  check_current_mp3_length "01.01"
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir"\
  "Today" "2007" "Rock" "17" "4" "http://www.jamendo.com/"

  print_ok
  echo
}

function test_normal_vbr_tags_from_filename_regex
{
  remove_output_dir

  test_name="vbr tags from filename regex"

  NEW_M_FILE="artist1__album2__title3__comment4__2__2004__Samba"
  NEW_MP3_FILE=$SONGS_DIR/${NEW_M_FILE}.mp3

  cp $MP3_FILE $NEW_MP3_FILE

  F1="title3-artist1-album2-2-1.mp3"
  F2="title3-artist1-album2-2-2.mp3"
  F3="title3-artist1-album2-2-3.mp3"

  expected=" Processing file 'songs/${NEW_M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/$F1\" created
   File \"$OUTPUT_DIR/$F2\" created
   File \"$OUTPUT_DIR/$F3\" created
 Processed 4595 frames - Sync errors: 0
 file split"
  regex_option="(?<artist>.*?)__(?<album>.*?)__(?<title>.*?)__(?<comment>.*?)__(?<tracknum>.*?)__(?<year>.*?)__(?<genre>.*)"
  output_option="@t-@a-@b-@N-@n"
  mp3splt_args="-d $OUTPUT_DIR -o $output_option -G \"regex=$regex_option\" ${NEW_MP3_FILE} 0.5 1.0 1.5 2.0"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/$F1"

#ID3v2, eyeD3 apparently reads other YEAR tag from the original ID3v2 tags
  check_all_mp3_tags_with_version "2" "artist1" "album2" "title3" "2007"\
  "Samba" "114" "2" "comment4"
  check_all_mp3_tags_with_version "1" "artist1" "album2" "title3" "2004"\
  "Samba" "114" "2" "comment4"

  current_file="$OUTPUT_DIR/$F2"
  check_all_mp3_tags_with_version "2" "artist1" "album2" "title3" "2007"\
  "Samba" "114" "2" "comment4"

  current_file="$OUTPUT_DIR/$F3"
  check_all_mp3_tags_with_version "2" "artist1" "album2" "title3" "2007"\
  "Samba" "114" "2" "comment4"

  rm -f $NEW_MP3_FILE

  print_ok
  echo
}

function test_normal_vbr_with_auto_adjust
{
  remove_output_dir

  M_FILE="La_Verue__Today_silence"

  test_name="normal with auto adjust"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 Working with SILENCE AUTO-ADJUST (Threshold: -48.0 dB Gap: 30 sec Offset: 0.80)
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_00m_00s__01m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_00s__04m_05s_69h.mp3\" created
 Processed 9406 frames - Sync errors: 0
 file split (EOF)"
  mp3splt_args="-a -d $OUTPUT_DIR $SILENCE_MP3_FILE 0.0 1.0 2.0 3.0 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_00m_00s__01m_00s.mp3" 
  check_current_mp3_length "01.04"
  check_current_file_has_xing
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "1792357"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "1586487"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3" 
  check_current_mp3_length "00.55"
  check_current_file_has_xing
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir"\
  "Today" "2007" "Rock" "17" "3" "http://www.jamendo.com/"
  check_current_file_size "1561672"
  
  current_file="$OUTPUT_DIR/${M_FILE}_03m_00s__04m_05s_69h.mp3" 
  check_current_mp3_length "01.05"
  check_current_file_has_xing
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "4" "http://www.jamendo.com/"
  check_current_file_size "1976114"

  print_ok
  echo
}

function test_normal_vbr_with_negative_splitpoints
{
  remove_output_dir

  M_FILE="La_Verue__Today"

  test_name="normal with negative splitpoints"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_05s_58h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_05s_58h__03m_05s_58h.mp3\" created
 Processed 7106 frames - Sync errors: 0
 file split"
  mp3splt_args=" -d $OUTPUT_DIR $MP3_FILE 1.0 EOF-2.0 EOF-1.0" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_05s_58h.mp3" 
  check_current_mp3_length "01.05"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_05s_58h__03m_05s_58h.mp3" 
  check_current_mp3_length "01.00"

  print_ok
  echo
}

function test_normal_vbr_custom_tags_with_replace_tags_in_tags_and_original_tags
{
  remove_output_dir

  test_name="vbr custom tags & replace tags in tags & original tags"
  M_FILE="La_Verue__Today"

  F1="-a_10-b_a_@n-10.mp3"
  F2="Today_7_8-La Verue-album_cc_@t-7.mp3"
  F3="Today_8_8-La Verue-album_cc_@t-8.mp3"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/$F1\" created
   File \"$OUTPUT_DIR/$F2\" created
   File \"$OUTPUT_DIR/$F3\" created
 Processed 4595 frames - Sync errors: 0
 file split"
  tags_option="r[@a=a_@n,@b=b_@a,@c=cc_@b,@n=10]%[@o,@c=cc_@t,@b=album_@c,@t=#t_@n_#n,@N=7]"
  output_option="@t-@a-@b-@N"
  mp3splt_args="-d $OUTPUT_DIR -o $output_option -g \"$tags_option\" $MP3_FILE 0.5 1.0 1.5 2.0"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/$F1"
  check_all_mp3_tags_with_version "2" "a_10" "b_a_@n" "" "" "" "" "10" "cc_b_@a"

  current_file="$OUTPUT_DIR/$F2"
  check_all_mp3_tags_with_version "2" "La Verue" "album_cc_@t" "Today_7_8" "2007"\
  "Rock" "17" "7" "cc_#t_@n_#n"

  current_file="$OUTPUT_DIR/$F3"
  check_all_mp3_tags_with_version "2" "La Verue" "album_cc_@t" "Today_8_8" "2007"\
  "Rock" "17" "8" "cc_#t_@n_#n"

  print_ok
  echo
}

function test_normal_vbr_custom_tags_with_replace_tags_in_tags_and_time_variables
{
  remove_output_dir

  test_name="vbr custom tags & replace tags in tags & time variables"
  M_FILE="La_Verue__Today"

  F1="-a_10-b_a_@n-10.mp3"
  F2="7__01_00__01_05-La Verue-album_cc_@t-7.mp3"
  F3="8__01_05__02_00-La Verue-album_cc_@t-8.mp3"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/$F1\" created
   File \"$OUTPUT_DIR/$F2\" created
   File \"$OUTPUT_DIR/$F3\" created
 Processed 4595 frames - Sync errors: 0
 file split"
  tags_option="r[@a=a_@n,@b=b_@a,@c=cc_@b,@n=10]%[@o,@c=cc_@t,@b=album_@c,@t=@n__@m_@s__@M_@S,@N=7]"
  output_option="@t-@a-@b-@N"
  mp3splt_args="-d $OUTPUT_DIR -o $output_option -g \"$tags_option\" $MP3_FILE 0.5 1.0 1.5 2.0"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/$F1"
  check_all_mp3_tags_with_version "2" "a_10" "b_a_@n" "" "" "" "" "10" "cc_b_@a"

  current_file="$OUTPUT_DIR/$F2"
  check_all_mp3_tags_with_version "2" "La Verue" "album_cc_@t" "7__01_00__01_05" "2007"\
  "Rock" "17" "7" "cc_@n__@m_@s__@M_@S"

  current_file="$OUTPUT_DIR/$F3"
  check_all_mp3_tags_with_version "2" "La Verue" "album_cc_@t" "8__01_05__02_00" "2007"\
  "Rock" "17" "8" "cc_@n__@m_@s__@M_@S"

  print_ok
  echo
}

function test_normal_with_sync_errors
{
  local tags_version=$1

  remove_output_dir

  M_FILE="syncerror"

  test_name="vbr with sync errors"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_07m_00s__04m_05s_58h.mp3\" created
 Processed 27372 frames - Sync errors: 2
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR $SYNCERR_FILE 1.0 2.0.2 3.5 7.0 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_size "1412518"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3" 
  check_current_mp3_length "01.04"
  check_current_file_size "1567932"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3" 
  check_current_mp3_length "03.55"
  check_current_file_size "5375652"

  current_file="$OUTPUT_DIR/${M_FILE}_07m_00s__04m_05s_58h.mp3" 
  check_current_mp3_length "04.55"
  check_current_file_size "5308269"

  print_ok
  echo
}


function run_normal_vbr_tests
{
  p_blue " NORMAL VBR mp3 tests ..."
  echo

  normal_test_functions=$(declare -F | grep " test_normal_vbr_" | awk '{ print $3 }')

  for test_func in $normal_test_functions;do
    eval $test_func
  done

  p_blue " NORMAL VBR tests DONE."
  echo
}

#main
export LC_ALL="C"
start_date=$(date +%s)

run_normal_vbr_tests

p_failed_tests

end_date=$(date +%s)

p_time_diff_cyan $start_date $end_date "\t"
echo -e '\n'

exit 0

