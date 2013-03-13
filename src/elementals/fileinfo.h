#ifndef __FILEINFO_HOD
#define __FILEINFO_HOD

#include <elementals/types.h>
#include <elementals/array.h>
#include <elementals/regexp.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct {
  char* path;
  char* ext;
  char* basename;
  char* dirname;
  char* absolute_path;
  char* filename;
} file_info_t;

DECLARE_EL_ARRAY(file_info_array, file_info_t);

file_info_t* file_info_new(const char* path);
void file_info_destroy(file_info_t* info);
const char* file_info_absolute_path(const file_info_t* info);
const char* file_info_ext(const file_info_t* info);
const char* file_info_path(const file_info_t* info);
const char* file_info_dirname(const file_info_t* info);
const char* file_info_basename(const file_info_t* info);
const char* file_info_filename(const file_info_t* info);

file_info_t* file_info_new_home(const char* file);
file_info_t* file_info_combine(const file_info_t* info, const char * name);

el_bool file_info_exists(const file_info_t* info);
el_bool file_info_is_dir(const file_info_t* info);
el_bool file_info_is_file(const file_info_t* info);
el_bool file_info_can_read(const file_info_t* info);
el_bool file_info_can_write(const file_info_t* info);
el_bool file_info_is_absolute(const file_info_t* info);
el_bool file_info_is_hidden(const file_info_t* info);

time_t file_info_mtime(const file_info_t* info);
size_t file_info_size(const file_info_t* info);

file_info_array file_info_scandir(const file_info_t* info, hre_t path_regexp);
file_info_array file_info_subdirs(const file_info_t* info);

el_bool file_info_mkdir_p(const file_info_t* info, mode_t mode);


#endif

