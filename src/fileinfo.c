#include <elementals/memcheck.h>
#include <elementals/fileinfo.h>
#include <elementals/regexp.h>
#include <elementals/log.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>

static file_info_t* copy(file_info_t* info)
{
  return file_info_new(file_info_path(info));
}

static void destroy(file_info_t* info)
{
  file_info_destroy(info);
}

IMPLEMENT_EL_ARRAY(file_info_array, file_info_t, copy, destroy);

#define SEP '/'
#define STRSEP "/"

static inline int is_sep(char c) {
  return c == SEP;
}

file_info_t *file_info_new_home(const char* file)
{
  const char* home=getenv("HOME");
  if (home == NULL) { home = "."; }
  if (file != NULL) {
    char* full = hre_concat3(home, STRSEP, file);
    file_info_t* info = file_info_new(full);
    mc_free(full);
    return info;
  } else {
    file_info_t* info = file_info_new(home);
    return info;
  }
}

file_info_t *file_info_new(const char *path) 
{
  file_info_t* info = (file_info_t*) mc_malloc(sizeof(file_info_t));
  
  info->path = mc_strdup(path);
  
  int i;
  for(i=strlen(info->path)-1;i >= 0 && info->path[i] != '.' && !is_sep(info->path[i]);--i);
  if (i>=0 && is_sep(info->path[i])) {
    info->ext = mc_strdup("");
    i = strlen(info->path)-1;
  } else {
    info->ext = mc_strdup(&info->path[i+1]);
  }
  
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
  
  //log_debug3("path = %s, k = %d", info->path, k);
  if (k >= 0) {
    char h = info->path[k];
    info->path[k] = '\0';
    info->dirname = mc_strdup(info->path);
    info->path[k] = h;
  } else {
    info->dirname = mc_strdup("");
  }
  
  info->absolute_path = realpath(info->path, NULL);
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
  } else {
    mc_take_control(info->absolute_path, strlen(info->absolute_path)+1);
  }
  
  return info;
}

el_bool file_info_is_hidden(const file_info_t* info)
{
  return file_info_filename(info)[0] == '.';
}

const char* file_info_absolute_path(const file_info_t* info) 
{
  return info->absolute_path;
}

const char* file_info_ext(const file_info_t* info)
{
  return info->ext;
}

const char* file_info_path(const file_info_t* info)
{
  return info->path;
}

const char* file_info_basename(const file_info_t* info)
{
  return info->basename;
}

const char* file_info_dirname(const file_info_t* info)
{
  return info->dirname;
}

const char* file_info_filename(const file_info_t* info)
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

el_bool file_info_exists(const file_info_t* info)
{
  return access(info->absolute_path, F_OK) == 0;
}

el_bool file_info_is_dir(const file_info_t* info)
{
  struct stat st;
  stat(info->absolute_path, &st);
  switch (st.st_mode & S_IFMT) {
    case S_IFDIR: return el_true;
    default: return el_false;
  }
}

el_bool file_info_is_file(const file_info_t* info)
{
  struct stat st;
  stat(info->absolute_path, &st);
  switch (st.st_mode & S_IFMT) {
    case S_IFREG: return el_true;
    default: return el_false;
  }
}

el_bool file_info_is_absolute(const file_info_t* info)
{
  return info->path[0] == SEP;
}

el_bool file_info_can_read(const file_info_t* info)
{
  return access(info->absolute_path, R_OK) == 0;
}

el_bool file_info_can_write(const file_info_t* info)
{
  return access(info->absolute_path, W_OK) == 0;
}

time_t file_info_mtime(const file_info_t* info)
{
  struct stat st;
  if (!stat(info->absolute_path, &st)) {
    return st.st_mtime;
  } else {
    return 0;
  }
}

size_t file_info_size(const file_info_t* info)
{
  struct stat st;
  if (!stat(info->absolute_path, &st)) {
    return st.st_size;
  } else {
    return 0;
  }
}

file_info_t* file_info_combine(const file_info_t* info, const char* name)
{
  int len = strlen(file_info_absolute_path(info)) + strlen(STRSEP) + strlen(name) + 1;
  char* s = (char*) mc_malloc(len);
  strcpy(s, file_info_path(info));
  int i = strlen(s) - 1;
  if (i < 0 || !is_sep(s[i])) { strcat(s, STRSEP); }
  strcat(s, name);
  
  file_info_t* result = file_info_new(s);
  mc_free(s);
  
  return result;
}

file_info_array file_info_scandir(const file_info_t* info, hre_t path_regexp)
{
  file_info_array matches = file_info_array_new();
  if (file_info_is_dir(info)) {
    DIR* dirh = opendir(file_info_absolute_path(info));
    if (dirh != NULL) {
      struct dirent entry;
      struct dirent *result;
      
      while (readdir_r(dirh, &entry, &result) == 0 && result != NULL) {
        if (hre_has_match(path_regexp, entry.d_name)) {
          file_info_t* e = file_info_combine(info, entry.d_name);
          file_info_array_append(matches, e);
          file_info_destroy(e);
        } 
      }
      
      closedir(dirh);
    }
  }
  return matches;
}

file_info_array file_info_subdirs(const file_info_t* info) 
{
  file_info_array matches = file_info_array_new();
  if (file_info_is_dir(info)) {
    DIR* dirh = opendir(file_info_absolute_path(info));
    if (dirh != NULL) {
      struct dirent entry;
      struct dirent *result;
      
      while (readdir_r(dirh, &entry, &result) == 0 && result != NULL) {
        file_info_t* e = mc_take_over(file_info_combine(info, entry.d_name));
        if (file_info_is_dir(e)) {
          if (strcmp(file_info_filename(e),".") != 0 &&
              strcmp(file_info_filename(e),"..") != 0) {
            //log_debug2("adding %s\n", file_info_path(e));
            file_info_array_append(matches, e);
          }
        } 
        file_info_destroy(e);
      }
      
      closedir(dirh);
    }
  }
  return matches;
}

el_bool file_info_mkdir_p(const file_info_t* info, mode_t mode)
{
  if (file_info_is_dir(info)) { 
    return el_true;
  }
  
  const char* dirname = file_info_dirname(info);
  if (strcmp(dirname, "") == 0) {
    return el_false; 
  }
  
  file_info_t* d = file_info_new(dirname);
  if (file_info_is_dir(d)) {
    file_info_t* dn = file_info_combine(d, file_info_filename(info));
    int r = mkdir(file_info_absolute_path(dn), mode);
    file_info_destroy(dn);
    file_info_destroy(d);
    return (r >= 0);
  } else {
    if (file_info_exists(d)) {
      file_info_destroy(d);
      return el_false;
    } else {
      if (file_info_mkdir_p(d, mode)) {
        el_bool r = file_info_mkdir_p(info, mode);
        file_info_destroy(d);
        return r;
      } else {
        file_info_destroy(d);
        return el_false;
      }
    }
  }
} 
