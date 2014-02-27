#include "ls.h"

static int ls_dispatcher(const arguments *args, const char *path, char recursive, char as_file);


static int print_filename_color(char type)
{
	switch (type) {
	case 'd': return printf("%s", FG_BLUE);
	case 's': return printf("%s", FG_YELLOW);
	case 'l': return printf("%s", FG_RED);
	case 'b': return printf("%s", FG_GREEN);
	case 'c': return printf("%s", FG_MAGENTA);
	case 'p': return printf("%s", FG_CYAN);
	case '-': return 0;
	default: return printf("%s", FG_BLACK);
	}
}


static int ls_file(const arguments *args, const char *path, struct stat *stat_buf)
{
	char s[] = "----------";
	
	// Определение типа файла
	switch(stat_buf->st_mode & S_IFMT) {
		case S_IFDIR:  s[0] = 'd'; break;	// Директория
		case S_IFSOCK: s[0] = 's'; break;	// Сокет
		case S_IFLNK:  s[0] = 'l'; break;	// Символьная ссылка
		case S_IFBLK:  s[0] = 'b'; break;	// Блочное устройство
		case S_IFCHR:  s[0] = 'c'; break;	// Символьное устройство
		case S_IFIFO:  s[0] = 'p'; break;	// FIFO (first in - first out)
		case S_IFREG:  s[0] = '-'; break;	// Обычный файл
		default: s[0] = '-';
    }
	
	
	// Отбрасываем часть пути (кроме имени файла)
	if (!args->recursive) {
		const char *tmp = path;
		while (*tmp != '\0')
			if (*tmp == '/' && *(tmp + 1) != '\0') path = ++tmp;	// tmp -> ".../..."
			else ++tmp;
	}
	
	
	if (args->long_format) {
		// Определение прав доступа
		if (stat_buf->st_mode & S_IRUSR) s[1] = 'r';
		if (stat_buf->st_mode & S_IWUSR) s[2] = 'w';
		if (stat_buf->st_mode & S_IXUSR) s[3] = 'x';
		if (stat_buf->st_mode & S_IRGRP) s[4] = 'r';
		if (stat_buf->st_mode & S_IWGRP) s[5] = 'w';
		if (stat_buf->st_mode & S_IXGRP) s[6] = 'x';
		if (stat_buf->st_mode & S_IROTH) s[7] = 'r';
		if (stat_buf->st_mode & S_IWOTH) s[8] = 'w';
		if (stat_buf->st_mode & S_IXOTH) s[9] = 'x';
		
		
		// Получаем информацию о владельце
		struct passwd *user_data = getpwuid(stat_buf->st_uid);
		if (user_data == NULL) {
			error_errno(NULL);
			return STATUS_ERROR;
		}
		
		struct group *group_data = getgrgid(stat_buf->st_gid);
		if (group_data == NULL) {
			error_errno(NULL);
			return STATUS_ERROR;
		}
		
		const char *name = user_data->pw_name,
				   *group = group_data->gr_name;
		
		// Форматируем время последнего изменения
		char datestring[256];
		struct tm *tm = localtime(&stat_buf->st_mtime);	// st_mtime - время последнего изменения
		strftime(datestring, sizeof(datestring), nl_langinfo(D_T_FMT), tm);
		
		// Суффиксы размеров
		static char size_suff[] = "BKMGTP";	// Байты, килобайты, мегабайты, гигабайты, терабайты, петабайты
		
		// Печатаем отчёт с приведением размера файла к удобочитаемому виду
		if (stat_buf->st_size < 1024) {
			printf("%s %4u %s  %s %5lld%c %s ", s, (unsigned int)stat_buf->st_nlink, name, group, (long long int)stat_buf->st_size, size_suff[0], datestring);
			if (args->colors) print_filename_color(s[0]);
			printf("%s", path);
			if (s[0] == 'l') {
				struct stat stat_buf2;
				char *linkname;
				lstat(path, &stat_buf2);
				
				linkname = malloc(stat_buf2.st_size + 1);
				readlink(path, linkname, stat_buf2.st_size + 1);
				linkname[stat_buf2.st_size] = '\0';
				
				printf("  ->  %s", linkname);
			}
			if (args->colors) printf("%s", FG_COLOR);
			putchar('\n');
		} else {
			double size = stat_buf->st_size;
			int i = 0;
			while (size > 1024 && i < sizeof(size_suff)) ++i, size /= 1024;
			if (i >= sizeof(size_suff)) --i, size *= 1024;
		
			printf("%s %4u %s  %s %5.1lf%c %s ", s, (unsigned int)stat_buf->st_nlink, name, group, size, size_suff[i], datestring);
			if (args->colors) print_filename_color(s[0]);
			printf("%s", path);
			if (s[0] == 'l') {
				struct stat stat_buf2;
				char *linkname;
				lstat(path, &stat_buf2);
				
				linkname = malloc(stat_buf2.st_size + 1);
				readlink(path, linkname, stat_buf2.st_size + 1);
				linkname[stat_buf2.st_size] = '\0';
				
				printf("  ->  %s", linkname);
			}
			if (args->colors) printf("%s", FG_COLOR);
			putchar('\n');
		}
	} else {
		if (args->colors) print_filename_color(s[0]);
		printf("%s", path);
		if (args->colors) printf("%s", FG_COLOR);
		putchar('\n');
	}
	return STATUS_OK;
}


static int ls_dir(const arguments *args, const char *path)
{
	// Открываем текущую директорию
	DIR *dir = opendir(path);
	if (dir == NULL) {
		error_errno(NULL);
		return STATUS_ERROR;
	}
	
	// Подготовка к работе с new_path
	size_t path_len = strlen(path);
	char *new_path = NULL, need_slash = (path[path_len - 1] != '/');
	if (need_slash) ++path_len;
	
	// Получение inode родителя и себя
	ino_t parent_ino, self_ino;
	{
		struct stat st1, st2;
		new_path = malloc(path_len + need_slash + 3);
		if (new_path == NULL) {
			error_str("Allocation error!");
			if (closedir(dir) != 0) error_errno(NULL);
		}
		
		strcpy(new_path, path);
		if (need_slash)
			strcpy(new_path + path_len - 1, "/");
		strcpy(new_path + path_len, ".");
		stat(new_path, &st1);
		strcpy(new_path + path_len + 1, ".");
		stat(new_path, &st2);
		self_ino = st1.st_ino;
		parent_ino = st2.st_ino;
		
		free(new_path);
		new_path = NULL;
	}
	
	struct dirent *dp;
	while ((dp = readdir(dir)) != NULL) {
		// Пропуск ".*", если надо
		if (dp->d_name[0] == '.' && !args->all)
			continue;
		
		// Пропуск ссылок на родителя и себя
		if (dp->d_ino == parent_ino || dp->d_ino == self_ino)
			continue;
		
		// Выделяем память
		size_t n = path_len + strlen(dp->d_name);
		if (need_slash) ++n;
		new_path = malloc(n);
		if (new_path == NULL) {
			error_str("Allocation error!");
			if (closedir(dir) != 0) error_errno(NULL);
		}
		
		// Формируем new_path
		strcpy(new_path, path);
		if (need_slash) new_path[path_len - 1] = '/', new_path[path_len] = '\0';
		strcpy(new_path + path_len, dp->d_name);
		
		// Рекурсивно запускаем ls
		ls_dispatcher(args, new_path, 1, 1);
		
		free(new_path);
		new_path = NULL;
	}
	
	// Закрываем текущую директорию
	if (closedir(dir) != 0) {
		error_errno(NULL);
		return STATUS_ERROR;
	}
	return STATUS_OK;
}


static int ls_dispatcher(const arguments *args, const char *path, char recursive, char as_file)
{
	struct stat stat_buf;
	int status = lstat(path, &stat_buf);
	if (status) {
		error_errno(NULL);
		return STATUS_ERROR;
	}
	
	// Если path - директория и это первый вызов ls или делать рекурсивные вызовы необходимо...
	if ((stat_buf.st_mode & S_IFDIR) && (!recursive || args->recursive)) {
		// Печатаем текущую директорию
		//if (!args->recursive)
		//	printf("%s\n", path);
		if (as_file) ls_file(args, path, &stat_buf);
		return ls_dir(args, path);	// ...то обрабатываем как директорию
	}
	return ls_file(args, path, &stat_buf);	// ...иначе - как файл
}


int ls(const arguments *args, const char *path)
{
	return ls_dispatcher(args, path, 0, 0);
}