/*
  mkvmerge -- utility for splicing together matroska files
      from component media subtypes

  tagparser_start.cpp

  Written by Moritz Bunkus <moritz@bunkus.org>

  Distributed under the GPL
  see the file COPYING for details
  or visit http://www.gnu.org/copyleft/gpl.html
*/

/*!
    \file
    \version $Id$
    \brief XML tag parser. Functions for the start tags + helper functions
    \author Moritz Bunkus <moritz@bunkus.org>
*/

#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <vector>

#include <expat.h>

#include "common.h"
#include "mm_io.h"
#include "tagparser.h"

using namespace std;
using namespace libebml;
using namespace libmatroska;

#define check_instances(p, c) \
  if (FindChild<c>(*p) != NULL) \
    perror_oneinstance();

void start_level1(parser_data_t *pdata, const char *name) {
  string parent_name;

  parent_name = (*pdata->parent_names)[pdata->parent_names->size() - 2];

  if (!strcmp(name, "Tag")) {
    if (pdata->tag == NULL)
      pdata->tag = &GetChild<KaxTag>(*pdata->tags);
    else
      pdata->tag = &GetNextChild<KaxTag>(*pdata->tags, *pdata->tag);

  } else
    perror_nochild();

  pdata->parents->push_back(E_Tag);
}

void start_level2(parser_data_t *pdata, const char *name) {
  string parent_name;

  parent_name = (*pdata->parent_names)[pdata->parent_names->size() - 2];

  if (!strcmp(name, "Targets")) {
    if (pdata->targets == NULL)
      pdata->targets = &GetChild<KaxTagTargets>(*pdata->tag);
    else
      perror_oneinstance();
    pdata->parents->push_back(E_Targets);

  } else if (!strcmp(name, "General")) {
    if (pdata->general == NULL)
      pdata->general = &GetChild<KaxTagGeneral>(*pdata->tag);
    else
      perror_oneinstance();
    pdata->parents->push_back(E_General);

  } else if (!strcmp(name, "Genres")) {
    if (pdata->genres == NULL)
      pdata->genres = &GetChild<KaxTagGenres>(*pdata->tag);
    else
      perror_oneinstance();
    pdata->parents->push_back(E_Genres);

  } else if (!strcmp(name, "AudioSpecific")) {
    if (pdata->audio_specific == NULL)
      pdata->audio_specific = &GetChild<KaxTagAudioSpecific>(*pdata->tag);
    else
      perror_oneinstance();
    pdata->parents->push_back(E_AudioSpecific);

  } else if (!strcmp(name, "ImageSpecific")) {
    if (pdata->image_specific == NULL)
      pdata->image_specific = &GetChild<KaxTagImageSpecific>(*pdata->tag);
    else
      perror_oneinstance();
    pdata->parents->push_back(E_ImageSpecific);

  } else if (!strcmp(name, "MultiCommercial")) {
    if (pdata->m_commercial == NULL)
      pdata->m_commercial = &GetChild<KaxTagMultiCommercial>(*pdata->tag);
    else
      perror_oneinstance();
    pdata->parents->push_back(E_MultiCommercial);

  } else if (!strcmp(name, "MultiDate")) {
    if (pdata->m_date == NULL)
      pdata->m_date = &GetChild<KaxTagMultiDate>(*pdata->tag);
    else
      perror_oneinstance();
    pdata->parents->push_back(E_MultiDate);

  } else if (!strcmp(name, "MultiEntity")) {
    if (pdata->m_entity == NULL)
      pdata->m_entity = &GetChild<KaxTagMultiEntity>(*pdata->tag);
    else
      perror_oneinstance();
    pdata->parents->push_back(E_MultiEntity);

  } else if (!strcmp(name, "MultiIdentifier")) {
    if (pdata->m_identifier == NULL)
      pdata->m_identifier = &GetChild<KaxTagMultiIdentifier>(*pdata->tag);
    else
      perror_oneinstance();
    pdata->parents->push_back(E_MultiIdentifier);

  } else if (!strcmp(name, "MultiLegal")) {
    if (pdata->m_legal == NULL)
      pdata->m_legal = &GetChild<KaxTagMultiLegal>(*pdata->tag);
    else
      perror_oneinstance();
    pdata->parents->push_back(E_MultiLegal);

  } else if (!strcmp(name, "MultiTitle")) {
    if (pdata->m_title == NULL)
      pdata->m_title = &GetChild<KaxTagMultiTitle>(*pdata->tag);
    else
      perror_oneinstance();
    pdata->parents->push_back(E_MultiTitle);

  } else if (!strcmp(name, "MultiComment")) {
    if (pdata->m_comment == NULL)
      pdata->m_comment = &GetChild<KaxTagMultiComment>(*pdata->tag);
    else
      pdata->m_comment =
        &GetNextChild<KaxTagMultiComment>(*pdata->tag, *pdata->m_comment);
    pdata->parents->push_back(E_MultiComment);

  } else
    perror_nochild();
}

void start_level3(parser_data_t *pdata, const char *name) {
  string parent_name;
  int parent;

  parent_name = (*pdata->parent_names)[pdata->parent_names->size() - 2];
  parent = (*pdata->parents)[pdata->parents->size() - 1];

  if (parent == E_Targets) {
    pdata->data_allowed = true;

    if (!strcmp(name, "TrackUID")) {
      if (pdata->track_uid == NULL)
        pdata->track_uid = &GetChild<KaxTagTrackUID>(*pdata->targets);
      else
        pdata->track_uid =
          &GetNextChild<KaxTagTrackUID>(*pdata->targets, *pdata->track_uid);
      pdata->parents->push_back(E_TrackUID);
    } else if (!strcmp(name, "ChapterUID")) {
      if (pdata->chapter_uid == NULL)
        pdata->chapter_uid = &GetChild<KaxTagChapterUID>(*pdata->targets);
      else
        pdata->chapter_uid =
          &GetNextChild<KaxTagChapterUID>(*pdata->targets,
                                          *pdata->chapter_uid);
      pdata->parents->push_back(E_ChapterUID);

    } else
      perror_nochild();

  } else if (parent == E_General) {
    pdata->data_allowed = true;

    if (!strcmp(name, "Subject")) {
      check_instances(pdata->general, KaxTagSubject);
      pdata->parents->push_back(E_Subject);
    } else if (!strcmp(name, "Bibliography")) {
      check_instances(pdata->general, KaxTagBibliography);
      pdata->parents->push_back(E_Biography);
    } else if (!strcmp(name, "Language")) {
      check_instances(pdata->general, KaxTagLanguage);
      pdata->parents->push_back(E_Language);
    } else if (!strcmp(name, "Rating")) {
      check_instances(pdata->general, KaxTagRating);
      pdata->parents->push_back(E_Rating);
    } else if (!strcmp(name, "Encoder")) {
      check_instances(pdata->general, KaxTagEncoder);
      pdata->parents->push_back(E_Encoder);
    } else if (!strcmp(name, "EncodeSettings")) {
      check_instances(pdata->general, KaxTagEncodeSettings);
      pdata->parents->push_back(E_EncodeSettings);
    } else if (!strcmp(name, "File")) {
      check_instances(pdata->general, KaxTagFile);
      pdata->parents->push_back(E_File);
    } else if (!strcmp(name, "ArchivalLocation")) {
      check_instances(pdata->general, KaxTagArchivalLocation);
      pdata->parents->push_back(E_ArchivalLocation);
    } else if (!strcmp(name, "Keywords")) {
      if (pdata->keywords == NULL)
        pdata->keywords = &GetChild<KaxTagKeywords>(*pdata->general);
      else
        pdata->keywords =
          &GetNextChild<KaxTagKeywords>(*pdata->general, *pdata->keywords);
      pdata->parents->push_back(E_Keywords);
    } else if (!strcmp(name, "Mood")) {
      check_instances(pdata->general, KaxTagMood);
      pdata->parents->push_back(E_Mood);
    } else if (!strcmp(name, "RecordLocation")) {
      if (pdata->rec_location == NULL)
        pdata->rec_location = &GetChild<KaxTagRecordLocation>(*pdata->general);
      else
        pdata->rec_location =
          &GetNextChild<KaxTagRecordLocation>(*pdata->general,
                                              *pdata->rec_location);
      pdata->parents->push_back(E_RecordLocation);
    } else if (!strcmp(name, "Source")) {
      check_instances(pdata->general, KaxTagSource);
      pdata->parents->push_back(E_Source);
    } else if (!strcmp(name, "SourceForm")) {
      check_instances(pdata->general, KaxTagSourceForm);
      pdata->parents->push_back(E_SourceForm);
    } else if (!strcmp(name, "Product")) {
      check_instances(pdata->general, KaxTagProduct);
      pdata->parents->push_back(E_Product);
    } else if (!strcmp(name, "OriginalMediaType")) {
      check_instances(pdata->general, KaxTagOriginalMediaType);
      pdata->parents->push_back(E_OriginalMediaType);
    } else if (!strcmp(name, "PlayCounter")) {
      check_instances(pdata->general, KaxTagPlayCounter);
      pdata->parents->push_back(E_PlayCounter);
    } else if (!strcmp(name, "Popularimeter")) {
      check_instances(pdata->general, KaxTagPopularimeter);
      pdata->parents->push_back(E_Popularimeter);
    } else
      perror_nochild();

  } else if (parent == E_Genres) {
    pdata->data_allowed = true;

    if (!strcmp(name, "AudioGenre")) {
      if (pdata->audio_genre == NULL)
        pdata->audio_genre = &GetChild<KaxTagAudioGenre>(*pdata->genres);
      else
        pdata->audio_genre =
          &GetNextChild<KaxTagAudioGenre>(*pdata->genres, *pdata->audio_genre);
      pdata->parents->push_back(E_AudioGenre);
    } else if (!strcmp(name, "VideoGenre")) {
      if (pdata->video_genre == NULL)
        pdata->video_genre = &GetChild<KaxTagVideoGenre>(*pdata->genres);
      else
        pdata->video_genre =
          &GetNextChild<KaxTagVideoGenre>(*pdata->genres, *pdata->video_genre);
      pdata->parents->push_back(E_VideoGenre);
    } else if (!strcmp(name, "SubGenre")) {
      check_instances(pdata->genres, KaxTagSubGenre);
      pdata->parents->push_back(E_SubGenre);
    } else
      perror_nochild();

  } else if (parent == E_AudioSpecific) {
    pdata->data_allowed = true;

    if (!strcmp(name, "AudioEncryption")) {
      check_instances(pdata->audio_specific, KaxTagAudioEncryption);
      pdata->parents->push_back(E_AudioEncryption);
    } else if (!strcmp(name, "AudioGain")) {
      check_instances(pdata->audio_specific, KaxTagAudioGain);
      pdata->parents->push_back(E_AudioGain);
    } else if (!strcmp(name, "AudioPeak")) {
      check_instances(pdata->audio_specific, KaxTagAudioPeak);
      pdata->parents->push_back(E_AudioPeak);
    } else if (!strcmp(name, "BPM")) {
      check_instances(pdata->audio_specific, KaxTagBPM);
      pdata->parents->push_back(E_BPM);
    } else if (!strcmp(name, "Equalisation")) {
      check_instances(pdata->audio_specific, KaxTagEqualisation);
      pdata->parents->push_back(E_Equalisation);
    } else if (!strcmp(name, "DiscTrack")) {
      check_instances(pdata->audio_specific, KaxTagDiscTrack);
      pdata->parents->push_back(E_DiscTrack);
    } else if (!strcmp(name, "SetPart")) {
      check_instances(pdata->audio_specific, KaxTagSetPart);
      pdata->parents->push_back(E_SetPart);
    } else if (!strcmp(name, "InitialKey")) {
      check_instances(pdata->audio_specific, KaxTagInitialKey);
      pdata->parents->push_back(E_InitialKey);
    } else if (!strcmp(name, "OfficialFileURL")) {
      check_instances(pdata->audio_specific, KaxTagOfficialAudioFileURL);
      pdata->parents->push_back(E_OfficialFileURL);
    } else if (!strcmp(name, "OfficialSourceURL")) {
      check_instances(pdata->audio_specific, KaxTagOfficialAudioSourceURL);
      pdata->parents->push_back(E_OfficialSourceURL);
    } else
      perror_nochild();

  } else if (parent == E_ImageSpecific) {
    pdata->data_allowed = true;

    if (!strcmp(name, "CaptureDPI")) {
      check_instances(pdata->image_specific, KaxTagCaptureDPI);
      pdata->parents->push_back(E_CaptureDPI);
    } else if (!strcmp(name, "CaptureLightness")) {
      check_instances(pdata->image_specific, KaxTagCaptureLightness);
      pdata->parents->push_back(E_CaptureLightness);
    } else if (!strcmp(name, "CapturePaletteSetting")) {
      check_instances(pdata->image_specific, KaxTagCapturePaletteSetting);
      pdata->parents->push_back(E_CapturePaletteSetting);
    } else if (!strcmp(name, "CaptureSharpness")) {
      check_instances(pdata->image_specific, KaxTagCaptureSharpness);
      pdata->parents->push_back(E_CaptureSharpness);
    } else if (!strcmp(name, "Cropped")) {
      check_instances(pdata->image_specific, KaxTagCropped);
      pdata->parents->push_back(E_Cropped);
    } else if (!strcmp(name, "OriginalDimensions")) {
      check_instances(pdata->image_specific, KaxTagOriginalDimensions);
      pdata->parents->push_back(E_OriginalDimensions);
    } else
      perror_nochild();

  } else if (parent == E_MultiCommercial) {
    if (!strcmp(name, "Commercial")) {
      if (pdata->commercial == NULL)
        pdata->commercial = &GetChild<KaxTagCommercial>(*pdata->m_commercial);
      else
        pdata->commercial =
          &GetNextChild<KaxTagCommercial>(*pdata->m_commercial,
                                          *pdata->commercial);
     pdata->parents->push_back(E_Commercial);
    } else
      perror_nochild();

  } else if (parent == E_MultiDate) {
    if (!strcmp(name, "Date")) {
      if (pdata->date == NULL)
        pdata->date = &GetChild<KaxTagDate>(*pdata->m_date);
      else
        pdata->date =
          &GetNextChild<KaxTagDate>(*pdata->m_date, *pdata->date);
      pdata->parents->push_back(E_Date);
    } else
      perror_nochild();

  } else if (parent == E_MultiEntity) {
    if (!strcmp(name, "Entity")) {
      if (pdata->entity == NULL)
        pdata->entity = &GetChild<KaxTagEntity>(*pdata->m_entity);
      else
        pdata->entity =
          &GetNextChild<KaxTagEntity>(*pdata->m_entity, *pdata->entity);
      pdata->parents->push_back(E_Entity);
    } else
      perror_nochild();

  } else if (parent == E_MultiIdentifier) {
    if (!strcmp(name, "Identifier")) {
      if (pdata->identifier == NULL)
        pdata->identifier = &GetChild<KaxTagIdentifier>(*pdata->m_identifier);
      else
        pdata->identifier =
          &GetNextChild<KaxTagIdentifier>(*pdata->m_identifier,
                                          *pdata->identifier);
      pdata->parents->push_back(E_Identifier);
    } else
      perror_nochild();

  } else if (parent == E_MultiLegal) {
    if (!strcmp(name, "Legal")) {
      if (pdata->legal == NULL)
        pdata->legal = &GetChild<KaxTagLegal>(*pdata->m_legal);
      else
        pdata->legal =
          &GetNextChild<KaxTagLegal>(*pdata->m_legal, *pdata->legal);
      pdata->parents->push_back(E_Legal);
    } else
      perror_nochild();

  } else if (parent == E_MultiTitle) {
    if (!strcmp(name, "Title")) {
      if (pdata->title == NULL)
        pdata->title = &GetChild<KaxTagTitle>(*pdata->m_title);
      else
        pdata->title =
          &GetNextChild<KaxTagTitle>(*pdata->m_title, *pdata->title);
      pdata->parents->push_back(E_Title);
    } else
      perror_nochild();

  } else
    die("Unknown parent: level 3, %d", parent);
}

void start_level4(parser_data_t *pdata, const char *name) {
  string parent_name;
  int parent;

  parent_name = (*pdata->parent_names)[pdata->parent_names->size() - 2];
  parent = (*pdata->parents)[pdata->parents->size() - 1];

  if (parent == E_Commercial) {
    pdata->data_allowed = true;

    if (!strcmp(name, "CommercialType")) {
      check_instances(pdata->commercial, KaxTagMultiCommercialType);
      pdata->parents->push_back(E_CommercialType);
    } else if (!strcmp(name, "Address")) {
      check_instances(pdata->commercial, KaxTagMultiCommercialAddress);
      pdata->parents->push_back(E_Address);
    } else if (!strcmp(name, "URL")) {
      if (pdata->c_url == NULL)
        pdata->c_url = &GetChild<KaxTagMultiCommercialURL>(*pdata->commercial);
      else
        pdata->c_url =
          &GetNextChild<KaxTagMultiCommercialURL>(*pdata->commercial,
                                                  *pdata->c_url);
      pdata->parents->push_back(E_URL);
    } else if (!strcmp(name, "Email")) {
      if (pdata->c_email == NULL)
        pdata->c_email =
          &GetChild<KaxTagMultiCommercialEmail>(*pdata->commercial);
      else
        pdata->c_email =
          &GetNextChild<KaxTagMultiCommercialEmail>(*pdata->commercial,
                                                    *pdata->c_email);
      pdata->parents->push_back(E_Email);
    } else if (!strcmp(name, "MultiPrice")) {
      pdata->data_allowed = false;
      if (pdata->m_price == NULL)
        pdata->m_price = &GetChild<KaxTagMultiPrice>(*pdata->commercial);
      else
        pdata->m_price =
          &GetNextChild<KaxTagMultiPrice>(*pdata->commercial, *pdata->m_price);
      pdata->parents->push_back(E_MultiPrice);
    } else
      perror_nochild();

  } else if (parent == E_Date) {
    pdata->data_allowed = true;

    if (!strcmp(name, "DateType")) {
      check_instances(pdata->date, KaxTagMultiDateType);
      pdata->parents->push_back(E_DateType);
    } else if (!strcmp(name, "DateBegin")) {
      check_instances(pdata->date, KaxTagMultiDateDateBegin);
      pdata->parents->push_back(E_DateBegin);
    } else if (!strcmp(name, "DateEnd")) {
      check_instances(pdata->date, KaxTagMultiDateDateEnd);
      pdata->parents->push_back(E_DateEnd);
    } else
      perror_nochild();

  } else if (parent == E_Entity) {
    pdata->data_allowed = true;

    if (!strcmp(name, "EntityType")) {
      check_instances(pdata->entity, KaxTagMultiEntityType);
      pdata->parents->push_back(E_EntityType);
    } else if (!strcmp(name, "Name")) {
      check_instances(pdata->entity, KaxTagMultiEntityName);
      pdata->parents->push_back(E_Name);
    } else if (!strcmp(name, "URL")) {
      if (pdata->e_url == NULL)
        pdata->e_url = &GetChild<KaxTagMultiEntityURL>(*pdata->entity);
      else
        pdata->e_url =
          &GetNextChild<KaxTagMultiEntityURL>(*pdata->entity, *pdata->e_url);
      pdata->parents->push_back(E_URL);
    } else if (!strcmp(name, "Email")) {
      if (pdata->e_email == NULL)
        pdata->e_email = &GetChild<KaxTagMultiEntityEmail>(*pdata->entity);
      else
        pdata->e_email =
          &GetNextChild<KaxTagMultiEntityEmail>(*pdata->entity,
                                                *pdata->e_email);
      pdata->parents->push_back(E_Email);
    } else if (!strcmp(name, "Address")) {
      check_instances(pdata->entity, KaxTagMultiEntityAddress);
      pdata->parents->push_back(E_Address);
    } else
      perror_nochild();

  } else if (parent == E_Identifier) {
    pdata->data_allowed = true;

    if (!strcmp(name, "IdentifierType")) {
      check_instances(pdata->identifier, KaxTagMultiIdentifierType);
      pdata->parents->push_back(E_IdentifierType);
    } else if (!strcmp(name, "IdentifierBinary")) {
      check_instances(pdata->identifier, KaxTagMultiIdentifierBinary);
      pdata->parents->push_back(E_IdentifierBinary);
    } else if (!strcmp(name, "IdentifierString")) {
      check_instances(pdata->identifier, KaxTagMultiIdentifierString);
      pdata->parents->push_back(E_IdentifierString);
    } else
      perror_nochild();

  } else if (parent == E_Legal) {
    pdata->data_allowed = true;

    if (!strcmp(name, "LegalType")) {
      check_instances(pdata->legal, KaxTagMultiLegalType);
      pdata->parents->push_back(E_LegalType);
    } else if (!strcmp(name, "URL")) {
      if (pdata->l_url == NULL)
        pdata->l_url = &GetChild<KaxTagMultiLegalURL>(*pdata->legal);
      else
        pdata->l_url =
          &GetNextChild<KaxTagMultiLegalURL>(*pdata->legal, *pdata->l_url);
      pdata->parents->push_back(E_URL);
    } else if (!strcmp(name, "Address")) {
      check_instances(pdata->legal, KaxTagMultiLegalAddress);
      pdata->parents->push_back(E_Address);
    } else
      perror_nochild();

  } else if (parent == E_Title) {
    pdata->data_allowed = true;

    if (!strcmp(name, "TitleType")) {
      check_instances(pdata->title, KaxTagMultiTitleType);
      pdata->parents->push_back(E_TitleType);
    } else if (!strcmp(name, "Name")) {
      check_instances(pdata->title, KaxTagMultiTitleName);
      pdata->parents->push_back(E_Name);
    } else if (!strcmp(name, "SubTitle")) {
      check_instances(pdata->title, KaxTagMultiTitleSubTitle);
      pdata->parents->push_back(E_SubTitle);
    } else if (!strcmp(name, "Edition")) {
      check_instances(pdata->title, KaxTagMultiTitleEdition);
      pdata->parents->push_back(E_Edition);
    } else if (!strcmp(name, "Address")) {
      check_instances(pdata->title, KaxTagMultiTitleAddress);
      pdata->parents->push_back(E_Address);
    } else if (!strcmp(name, "URL")) {
      if (pdata->t_url == NULL)
        pdata->t_url = &GetChild<KaxTagMultiTitleURL>(*pdata->title);
      else
        pdata->t_url =
          &GetNextChild<KaxTagMultiTitleURL>(*pdata->title, *pdata->t_url);
      pdata->parents->push_back(E_URL);
    } else if (!strcmp(name, "Email")) {
      if (pdata->t_email == NULL)
        pdata->t_email = &GetChild<KaxTagMultiTitleEmail>(*pdata->title);
      else
        pdata->t_email =
          &GetNextChild<KaxTagMultiTitleEmail>(*pdata->title, *pdata->t_email);
      pdata->parents->push_back(E_Email);
    } else if (!strcmp(name, "Language")) {
      check_instances(pdata->title, KaxTagMultiTitleLanguage);
      pdata->parents->push_back(E_Language);
    } else
      perror_nochild();

  } else
    die("Unknown parent: level 4, %d", parent);
}

void start_level5(parser_data_t *pdata, const char *name) {
  string parent_name;
  int parent;

  parent_name = (*pdata->parent_names)[pdata->parent_names->size() - 2];
  parent = (*pdata->parents)[pdata->parents->size() - 1];

  if (parent == E_MultiPrice) {
    pdata->data_allowed = true;

    if (!strcmp(name, "Currency")) {
      check_instances(pdata->m_price, KaxTagMultiPriceCurrency);
      pdata->parents->push_back(E_Currency);
    } else if (!strcmp(name, "Amount")) {
      check_instances(pdata->m_price, KaxTagMultiPriceAmount);
      pdata->parents->push_back(E_Amount);
    } else if (!strcmp(name, "PriceDate")) {
      check_instances(pdata->m_price, KaxTagMultiPricePriceDate);
      pdata->parents->push_back(E_PriceDate);
    } else
      perror_nochild();

  } else
    die("Unknown parent: level 4, %d", parent);
}

void start_element(void *user_data, const char *name,
                   const char **atts) {
  parser_data_t *pdata;

  pdata = (parser_data_t *)user_data;

  if (atts[0] != NULL)
    perror(pdata, "Attributes are not allowed.");

  if (pdata->data_allowed)
    perror_unknown();

  pdata->parent_names->push_back(string(name));
  pdata->data_allowed = false;

  if (pdata->bin != NULL)
    assert("start_element: pdata->bin != NULL");

  if (pdata->depth == 0) {
    if (pdata->done_reading)
      perror(pdata, "More than one root element found.");
    if (strcmp(name, "Tags"))
      perror(pdata, "Root element must be <Tags>.");

    pdata->parents->push_back(E_Tags);
  } else if (pdata->depth == 1)
    start_level1(pdata, name);
  else if (pdata->depth == 2)
    start_level2(pdata, name);
  else if (pdata->depth == 3)
    start_level3(pdata, name);
  else if (pdata->depth == 4)
    start_level4(pdata, name);
  else if (pdata->depth == 5)
    start_level5(pdata, name);
  else
    die("Depth > 5: %d", pdata->depth);

  if (pdata->parent_names->size() != pdata->parents->size())
    perror_unknown();

  (pdata->depth)++;
}

void perror(parser_data_t *pdata, const char *fmt, ...) {
  va_list ap;

  fprintf(stderr, "Tag parsing error, line %d, column %d: ",
          XML_GetCurrentLineNumber(pdata->parser),
          XML_GetCurrentColumnNumber(pdata->parser));
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  fprintf(stderr, "\n");

#ifdef DEBUG
  int i;
  fprintf(stderr, "Parent names:\n");
  for (i = 0; i < pdata->parent_names->size(); i++)
    fprintf(stderr, "  %s\n", (*pdata->parent_names)[i].c_str());
  fprintf(stderr, "Parent types:\n");
  for (i = 0; i < pdata->parents->size(); i++)
    fprintf(stderr, "  %d\n", (*pdata->parents)[i]);
#endif

  exit(1);
}

static void add_data(void *user_data, const XML_Char *s, int len) {
  parser_data_t *pdata;
  int i;

  pdata = (parser_data_t *)user_data;

  if (!pdata->data_allowed) {
    for (i = 0; i < len; i++)
      if (!isspace(s[i]))
        perror(pdata, "Data is not allowed inside <%s>.",
               (*pdata->parent_names)
               [pdata->parent_names->size() - 1].c_str());
    return;
  }

  if (pdata->bin == NULL)
    pdata->bin = new string;

  for (i = 0; i < len; i++)
    (*pdata->bin) += s[i];
}

void parse_xml_tags(const char *name, KaxTags *tags) {
  char buffer[5000];
  int len, done;
  parser_data_t *pdata;
  mm_io_c *io;
  XML_Parser parser;

  try {
    io = new mm_io_c(name, MODE_READ);
  } catch(...) {
    mxprint(stderr, "Error: Could not open '%s' for reading.\n", name);
    exit(1);
  }

  done = 0;

  parser = XML_ParserCreate(NULL);

  pdata = (parser_data_t *)safemalloc(sizeof(parser_data_t));
  memset(pdata, 0, sizeof(parser_data_t));
  pdata->parser = parser;
  pdata->tags = tags;
  pdata->parent_names = new vector<string>;
  pdata->parents = new vector<int>;

  XML_SetUserData(parser, pdata);
  XML_SetElementHandler(parser, start_element, end_element);
  XML_SetCharacterDataHandler(parser, add_data);

  do {
    len = io->read(buffer, 5000);
    if (len != 5000)
      done = 1;
    if (XML_Parse(parser, buffer, len, done) == XML_STATUS_ERROR) {
      mxprint(stderr, "XML parser error at  line %d of '%s': %s. Aborting.\n",
              XML_GetCurrentLineNumber(parser), name,
              XML_ErrorString(XML_GetErrorCode(parser)));
      exit(1);
    }
  } while (!done);

  delete io;
  XML_ParserFree(parser);
}
