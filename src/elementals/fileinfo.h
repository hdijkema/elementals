#ifndef __FILEINFO_HOD
#define __FILEINFO_HOD

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

#endif

