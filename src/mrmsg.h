/*******************************************************************************
 *
 *                              Delta Chat Core
 *                      Copyright (C) 2017 Björn Petersen
 *                   Contact: r10s@b44t.com, http://b44t.com
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see http://www.gnu.org/licenses/ .
 *
 ******************************************************************************/


#ifndef __MRMSG_H__
#define __MRMSG_H__
#ifdef __cplusplus
extern "C" {
#endif


typedef struct mrmailbox_t mrmailbox_t;
typedef struct mrparam_t   mrparam_t;
typedef struct sqlite3_stmt sqlite3_stmt;


/**
 * An object representing a single message in memory.  The message
 * object is not updated.  If you want an update, you have to recreate the
 * object.
 */
typedef struct mrmsg_t
{
	uint32_t        m_magic;

	/**
	 * Message ID.  Never 0.
	 */
	uint32_t        m_id;
	#define         MR_MSG_ID_MARKER1       1
	#define         MR_MSG_ID_DAYMARKER     9
	#define         MR_MSG_ID_LAST_SPECIAL  9


	/**
	 * Contact ID of the sender.  Never 0. See mrcontact_t::m_id for special IDs.
	 * Use mrmailbox_get_contact() to load details about this contact.
	 */
	uint32_t        m_from_id;


	/**
	 * Contact ID of the recipient. Never 0. See mrcontact_t::m_id for special IDs.
	 * Use mrmailbox_get_contact() to load details about this contact.
	 */
	uint32_t        m_to_id;


	/**
	 * Chat ID the message belongs to. Never 0. See mrchat_t::m_id for special IDs.
	 * Use mrmailbox_get_chat() to load details about the chat.
	 */
	uint32_t        m_chat_id;


	/*
	 * The mailbox object the chat belongs to. Never NULL.
	 */
	//mrmailbox_t*    m_mailbox;


	/** @privatesection */

	int             m_type;                   /**< Message type. It is recommended to use mrmsg_set_type() and mrmsg_get_type() to access this field. */
	#define         MR_MSG_UNDEFINED        0
	#define         MR_MSG_TEXT            10
	#define         MR_MSG_IMAGE           20 /* m_param may contain MRP_FILE, MRP_WIDTH, MRP_HEIGHT */
	#define         MR_MSG_GIF             21 /*   - " -  */
	#define         MR_MSG_AUDIO           40 /* m_param may contain MRP_FILE, MRP_DURATION */
	#define         MR_MSG_VOICE           41 /*   - " -  */
	#define         MR_MSG_VIDEO           50 /* m_param may contain MRP_FILE, MRP_WIDTH, MRP_HEIGHT, MRP_DURATION */
	#define         MR_MSG_FILE            60 /* m_param may contain MRP_FILE  */

	int             m_state;                  /**< Message state. It is recommended to use mrmsg_get_state() to access this field. */
	#define         MR_STATE_UNDEFINED      0
	#define         MR_STATE_IN_FRESH      10
	#define         MR_STATE_IN_NOTICED    13
	#define         MR_STATE_IN_SEEN       16
	#define         MR_STATE_OUT_PENDING   20
	#define         MR_STATE_OUT_ERROR     24
	#define         MR_STATE_OUT_DELIVERED 26 /* to check if a mail was sent, use mrmsg_is_sent() */
	#define         MR_STATE_OUT_MDN_RCVD  28

	time_t          m_timestamp;              /**< Unix time the message was sended or received. 0 if unset. */
	char*           m_text;                   /**< Message text.  NULL if unset.  It is recommended to use mrmsg_set_text() and mrmsg_get_text() to access this field. */

	mrmailbox_t*    m_mailbox;                /**< may be NULL, set on loading from database and on sending */
	char*           m_rfc724_mid;             /**< The RFC-742 Message-ID */
	char*           m_server_folder;          /**< Folder where the message was last seen on the server */
	uint32_t        m_server_uid;             /**< UID last seen on the server for this message */
	int             m_is_msgrmsg;             /**< Set to 1 if the message was sent by another messenger. 0 otherwise. */
	int             m_starred;                /**< Starred-state of the message. 0=no, 1=yes. */
	mrparam_t*      m_param;                  /**< Additional paramter for the message. Never a NULL-pointer. It is recommended to use setters and getters instead of accessing this field directly. */
} mrmsg_t;


mrmsg_t*        mrmsg_new                   ();
void            mrmsg_unref                 (mrmsg_t*);
void            mrmsg_empty                 (mrmsg_t*);

int             mrmsg_get_type              (mrmsg_t*);
int             mrmsg_get_state             (mrmsg_t*);
time_t          mrmsg_get_timestamp         (mrmsg_t*);
char*           mrmsg_get_text              (mrmsg_t*);
char*           mrmsg_get_file              (mrmsg_t*);
char*           mrmsg_get_filename          (mrmsg_t*);
char*           mrmsg_get_filemime          (mrmsg_t*);
uint64_t        mrmsg_get_filebytes         (mrmsg_t*);
mrlot_t*        mrmsg_get_mediainfo         (mrmsg_t*);
int             mrmsg_get_width             (mrmsg_t*);
int             mrmsg_get_height            (mrmsg_t*);
int             mrmsg_get_duration          (mrmsg_t*);
int             mrmsg_get_showpadlock       (mrmsg_t*);
mrlot_t*        mrmsg_get_summary           (mrmsg_t*, mrchat_t*);
char*           mrmsg_get_summarytext       (mrmsg_t*, int approx_characters);
int             mrmsg_is_sent               (mrmsg_t*);
int             mrmsg_is_starred            (mrmsg_t*);
int             mrmsg_is_forwarded          (mrmsg_t*);
int             mrmsg_is_systemcmd          (mrmsg_t*);
int             mrmsg_is_increation         (mrmsg_t*);

int             mrmsg_is_setupmessage       (mrmsg_t*);
char*           mrmsg_get_setupcodebegin    (mrmsg_t*);

void            mrmsg_latefiling_mediasize  (mrmsg_t*, int width, int height, int duration);


/* library-private */
#define         MR_MSG_FIELDS                        " m.id,rfc724_mid,m.server_folder,m.server_uid,m.chat_id, m.from_id,m.to_id,m.timestamp, m.type,m.state,m.msgrmsg,m.txt, m.param,m.starred "
int             mrmsg_set_from_stmt__                (mrmsg_t*, sqlite3_stmt* row, int row_offset); /* row order is MR_MSG_FIELDS */
int             mrmsg_load_from_db__                 (mrmsg_t*, mrmailbox_t*, uint32_t id);
int             mrmsg_is_increation__                (const mrmsg_t*);
char*           mrmsg_get_summarytext_by_raw         (int type, const char* text, mrparam_t*, int approx_bytes); /* the returned value must be free()'d */
void            mrmsg_save_param_to_disk__           (mrmsg_t*);
void            mrmsg_guess_msgtype_from_suffix      (const char* pathNfilename, int* ret_msgtype, char** ret_mime);
void            mrmsg_get_authorNtitle_from_filename (const char* pathNfilename, char** ret_author, char** ret_title);

#define MR_MSG_NEEDS_ATTACHMENT(a)         ((a)==MR_MSG_IMAGE || (a)==MR_MSG_GIF || (a)==MR_MSG_AUDIO || (a)==MR_MSG_VOICE || (a)==MR_MSG_VIDEO || (a)==MR_MSG_FILE)
#define MR_MSG_MAKE_FILENAME_SEARCHABLE(a) ((a)==MR_MSG_AUDIO || (a)==MR_MSG_FILE || (a)==MR_MSG_VIDEO ) /* add filename.ext (without path) to m_text? this is needed for the fulltext search. The extension is useful to get all PDF, all MP3 etc. */
#define MR_MSG_MAKE_SUFFIX_SEARCHABLE(a)   ((a)==MR_MSG_IMAGE || (a)==MR_MSG_GIF || (a)==MR_MSG_VOICE)

#define APPROX_SUBJECT_CHARS 32  /* as we do not cut inside words, this results in about 32-42 characters.
								 Do not use too long subjects - we add a tag after the subject which gets truncated by the clients otherwise.
								 It should also be very clear, the subject is _not_ the whole message.
								 The value is also used for CC:-summaries */


#ifdef __cplusplus
} /* /extern "C" */
#endif
#endif /* __MRMSG_H__ */
