#ifndef MAXMINDDB_H
#define MAXMINDDB_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#if HAVE_CONFIG_H
#include <config.h>
#endif
#include <maxminddb_config.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>

#define MMDB_DATA_TYPE_EXTENDED (0)
#define MMDB_DATA_TYPE_POINTER (1)
#define MMDB_DATA_TYPE_UTF8_STRING (2)
#define MMDB_DATA_TYPE_DOUBLE (3)
#define MMDB_DATA_TYPE_BYTES (4)
#define MMDB_DATA_TYPE_UINT16 (5)
#define MMDB_DATA_TYPE_UINT32 (6)
#define MMDB_DATA_TYPE_MAP (7)
#define MMDB_DATA_TYPE_INT32 (8)
#define MMDB_DATA_TYPE_UINT64 (9)
#define MMDB_DATA_TYPE_UINT128 (10)
#define MMDB_DATA_TYPE_ARRAY (11)
#define MMDB_DATA_TYPE_CONTAINER (12)
#define MMDB_DATA_TYPE_END_MARKER (13)
#define MMDB_DATA_TYPE_BOOLEAN (14)
#define MMDB_DATA_TYPE_FLOAT (15)

/* GEOIPDB flags */
#define MMDB_MODE_MMAP (1)
#define MMDB_MODE_MASK (7)

/* GEOIPDB err codes */
#define MMDB_SUCCESS (0)
#define MMDB_FILE_OPEN_ERROR (1)
#define MMDB_CORRUPT_SEARCH_TREE_ERROR (2)
#define MMDB_INVALID_METADATA_ERROR (3)
#define MMDB_IO_ERROR (4)
#define MMDB_OUT_OF_MEMORY_ERROR (5)
#define MMDB_UNKNOWN_DATABASE_FORMAT_ERROR (6)
#define MMDB_INVALID_DATA_ERROR (7)
#define MMDB_INVALID_LOOKUP_PATH (8)
#define MMDB_LOOKUP_PATH_DOES_NOT_MATCH_DATA (9)

/* This is a pointer into the data section for a given IP address lookup */
typedef struct MMDB_entry_s {
    struct MMDB_s *mmdb;
    uint32_t offset;
} MMDB_entry_s;

typedef struct MMDB_lookup_result_s {
    bool found_entry;
    MMDB_entry_s entry;
    uint16_t netmask;
} MMDB_lookup_result_s;

typedef struct MMDB_entry_data_s {
    bool has_data;
    union {
        uint32_t pointer;
        const char *utf8_string;
        double double_value;
        const uint8_t *bytes;
        uint16_t uint16;
        uint32_t uint32;
        int32_t int32;
        uint64_t uint64;
#if MMDB_UINT128_IS_BYTE_ARRAY
        uint8_t uint128[16];
#else
        unsigned __int128 uint128;
#endif
        bool boolean;
        float float_value;
    };
    /* This is a 0 if a given entry cannot be found. This can only happen
     * when a call to MMDB_(v)get_value() asks for hash keys or array
     * indices that don't exist. */
    uint32_t offset;
    /* This is the next entry in the data section, but it's really only
     * relevant for entries that part of a larger map or array
     * struct. There's no good reason for an end user to look at this
     * directly. */
    uint32_t offset_to_next;
    /* This is only valid for strings, utf8_strings or binary data */
    uint32_t data_size;
    /* This is an MMDB_DATA_TYPE_* constant */
    uint32_t type;
} MMDB_entry_data_s;

/* This is the return type when someone asks for all the entry data in a map or array */
typedef struct MMDB_entry_data_list_s {
    MMDB_entry_data_s entry_data;
    struct MMDB_entry_data_list_s *next;
} MMDB_entry_data_list_s;

typedef struct MMDB_description_s {
    const char *language;
    const char *description;
} MMDB_description_s;

typedef struct MMDB_metadata_s {
    uint32_t node_count;
    uint16_t record_size;
    uint16_t ip_version;
    const char *database_type;
    struct {
        size_t count;
        const char **names;
    } languages;
    uint16_t binary_format_major_version;
    uint16_t binary_format_minor_version;
    uint64_t build_epoch;
    struct {
        size_t count;
        MMDB_description_s **descriptions;
    } description;
} MMDB_metadata_s;

typedef struct MMDB_s {
    uint32_t flags;
    const char *filename;
    ssize_t file_size;
    const uint8_t *file_content;
    const uint8_t *data_section;
    uint32_t data_section_size;
    const uint8_t *metadata_section;
    uint32_t metadata_section_size;
    uint16_t full_record_byte_size;
    uint16_t depth;
    MMDB_metadata_s metadata;
} MMDB_s;

    /* *INDENT-OFF* */
    /* --prototypes automatically generated by dev-bin/regen-prototypes.pl - don't remove this comment */
    extern int MMDB_open(const char *filename, uint32_t flags, MMDB_s *mmdb);
    extern MMDB_lookup_result_s MMDB_lookup_string(MMDB_s *mmdb, const char *ipstr,
                                                   int *gai_error, int *mmdb_error);
    extern MMDB_lookup_result_s MMDB_lookup_sockaddr(MMDB_s *mmdb,
                                                     struct sockaddr *sockaddr,
                                                     int *mmdb_error);
    extern int MMDB_get_value(MMDB_entry_s *start, MMDB_entry_data_s *entry_data, ...);
    extern int MMDB_vget_value(MMDB_entry_s *start, MMDB_entry_data_s *entry_data,
                               va_list va_path);
    extern int MMDB_aget_value(MMDB_entry_s *start, MMDB_entry_data_s *entry_data,
                               char **path);
    extern int MMDB_get_metadata_as_entry_data_list(
               MMDB_s *mmdb, MMDB_entry_data_list_s **entry_data_list);
    extern int MMDB_get_entry_data_list(MMDB_entry_s *start,
                                        MMDB_entry_data_list_s **entry_data_list);
    extern void MMDB_free_entry_data_list(MMDB_entry_data_list_s *entry_data_list);
    extern void MMDB_close(MMDB_s *mmdb);
    extern const char *MMDB_lib_version(void);
    extern int MMDB_dump_entry_data_list(FILE *stream,
                                         MMDB_entry_data_list_s *entry_data_list,
                                         int indent);
    extern const char *MMDB_strerror(int error_code);
    /* --prototypes end - don't remove this comment-- */
    /* *INDENT-ON* */

#endif                          /* MAXMINDDB_H */
