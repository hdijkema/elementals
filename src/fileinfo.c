#include <elementals/memcheck.h>
#include <elementals/fileinfo.h>
#include <stdlib.h>

#define SEP '/'
static inline int is_sep(char c) {
  return c == SEP;
}

file_info_t *file_info_new(const char *path) 
{
  file_info_t* info = (file_info_t*) mc_malloc(sizeof(file_info_t));
  
  info->path = mc_strdup(path);
  
  int i;
  for(i=strlen(info->path)-1;i >= 0 && info->path[i] != '.';--i);
  info->ext = mc_strdup(&info->path[i+1]);
  
  int k;
  for(k = i;k >= 0 && !is_sep(info->path[k]); --k);
  if (i >= 0) {
    char h = info->path[i];
    info->path[i] = '\0';
    info->basename = mc_strdup(&info->path[k+1]);
    info->path[i] = h;
    info->filename = mc_strdup(&info->path[k+1]);
  } else {
    info->basename = mc_strdup("");
    info->filename = mc_strdup("");
  }
  
  if (k >= 0) {
    char h = info->path[k];
    info->path[k] = '\0';
    info->dirname = mc_strdup(info->path);
    info->path[k] = h;
  }
  
  info->absolute_path = mc_take_over(realpath(info->path, NULL));
  
  return info;
}

const char* file_info_absolute_path(file_info_t* info) 
{
  return info->absolute_path;
}

const char* file_info_ext(file_info_t* info)
{
  return info->ext;
}

const char* file_info_path(file_info_t* info)
{
  return info->path;
}

const char* file_info_basename(file_info_t* info)
{
  return info->basename;
}

const char* file_info_dirname(file_info_t* info)
{
  return info->dirname;
}

const char* file_info_filename(file_info_t* info)
{
  return info->filename;
}

void file_info_destroy(file_info_t* info)
{
  mc_free(info->absolute_path);
  mc_free(info->path);
  mc_free(info->ext);
  mc_free(info->basename);
  mc_free(info->dirname);
  mc_free(info->filename);
  mc_free(info);
}



