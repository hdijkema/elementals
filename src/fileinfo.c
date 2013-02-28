#include <elementals/memcheck.h>
#include <elementals/fileinfo.h>
#include <elementals/regexp.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#define SEP '/'
#define STRSEP "/"

static inline int is_sep(char c) {
  return c == SEP;
}

file_info_t *file_info_new_home(const char* file)
{
  const char* home=getenv("HOME");
  if (home == NULL) { home = "."; }
  char* full = hre_concat3(home, STRSEP, file);
  file_info_t* info = file_info_new(full);
  mc_free(full);
  return info;
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
  if (info->absolute_path == NULL) {
    if (strlen(info->dirname) > 0) {
      file_info_t *i = file_info_new(info->dirname);
      if (i->absolute_path != NULL) {
        info->absolute_path = hre_concat3(i->absolute_path, STRSEP, info->filename);
      }
      file_info_destroy(i);
    } else {
      char buf[10240];
      if (getcwd(buf,10240) == buf) {
        info->absolute_path = mc_strdup(buf);
      }
    }
  }
  
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

el_bool file_info_exists(file_info_t* info)
{
  return access(info->absolute_path, F_OK) == 0;
}

el_bool file_info_is_dir(file_info_t* info)
{
  struct stat st;
  stat(info->absolute_path, &st);
  switch (st.st_mode & S_IFMT) {
    case S_IFDIR: return el_true;
    default: return el_false;
  }
}

el_bool file_info_is_file(file_info_t* info)
{
  struct stat st;
  stat(info->absolute_path, &st);
  switch (st.st_mode & S_IFMT) {
    case S_IFREG: return el_true;
    default: return el_false;
  }
}

el_bool file_info_can_read(file_info_t* info)
{
  return access(info->absolute_path, R_OK) == 0;
}

el_bool file_info_can_write(file_info_t* info)
{
  return access(info->absolute_path, W_OK) == 0;
}

time_t file_info_mtime(file_info_t* info)
{
  struct stat st;
  if (!stat(info->absolute_path, &st)) {
    return st.st_mtime;
  } else {
    return 0;
  }
}


