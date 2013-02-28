#ifndef __FILEINFO_HOD
#define __FILEINFO_HOD

#include <elementals/types.h>

typedef struct {
  char* path;
  char* ext;
  char* basename;
  char* dirname;
  char* absolute_path;
  char* filename;
} file_info_t;

file_info_t* file_info_new(const char *path);
void file_info_destroy(file_info_t* info);
const char* file_info_absolute_path(file_info_t* info);
const char* file_info_ext(file_info_t* info);
const char* file_info_path(file_info_t* info);
const char* file_info_dirname(file_info_t* info);
const char* file_info_basename(file_info_t* info);
const char* file_info_filename(file_info_t* info);

file_info_t* file_info_new_home(const char* file);

el_bool file_info_exists(file_info_t *info);
el_bool file_info_is_dir(file_info_t *info);
el_bool file_info_is_file(file_info_t *info);
el_bool file_info_can_read(file_info_t *info);
el_bool file_info_can_write(file_info_t *info);

#endif

