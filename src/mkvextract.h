/*
  mkvextract -- extract tracks from Matroska files into other files

  mkvextract.h

  Written by Moritz Bunkus <moritz@bunkus.org>

  Distributed under the GPL
  see the file COPYING for details
  or visit http://www.gnu.org/copyleft/gpl.html
*/

/*!
    \file
    \version $Id$
    \brief extracts tracks and other items from Matroska files into other files
    \author Moritz Bunkus <moritz@bunkus.org>
*/

#ifndef __MKVEXTRACT_H
#define __MKVEXTRACT_H

extern "C" {
#include <avilib.h>
}

#include <ogg/ogg.h>

#include <vector>

#include <ebml/EbmlElement.h>

#include "mm_io.h"

using namespace std;
using namespace libebml;
using namespace libmatroska;

class ssa_line_c {
public:
  char *line;
  int num;

  bool operator < (const ssa_line_c &cmp) const;
};

typedef struct {
  char *out_name;

  mm_io_c *out;
  avi_t *avi;
  ogg_stream_state osstate;

  int64_t tid;
  bool in_use, done;

  char track_type;
  int type;

  char *codec_id;
  void *private_data;
  int private_size;

  float a_sfreq;
  int a_channels, a_bps;

  float v_fps;
  int v_width, v_height;

  int64_t default_duration;

  int srt_num;
  int conv_handle;
  vector<ssa_line_c> ssa_lines;

  wave_header wh;
  int64_t bytes_written;

  unsigned char *buffered_data;
  int buffered_size;
  int64_t packetno, last_end;
  int header_sizes[3];
  unsigned char *headers[3];

  int aac_id, aac_profile, aac_srate_idx;
} kax_track_t;

extern vector<kax_track_t> tracks;
extern char typenames[14][20];

#define fits_parent(l, p) (l->GetElementPosition() < \
                           (p->GetElementPosition() + p->ElementSize()))
#define in_parent(p) (in->getFilePointer() < \
                      (p->GetElementPosition() + p->ElementSize()))

// Helper functions in mkvextract.cpp
void show_element(EbmlElement *l, int level, const char *fmt, ...);
void show_error(const char *fmt, ...);
kax_track_t *find_track(int tid);

bool extract_tracks(const char *file_name);
void extract_tags(const char *file_name);
void extract_chapters(const char *file_name, bool chapter_format_simple);
void extract_attachments(const char *file_name);

#endif // __MKVEXTRACT_H
