#include <dirent.h> 
#include <stdio.h> 
#include <ctype.h>
#include <string.h>

int main() {
    struct dirent* dir;
	DIR* proc = opendir("/proc");
	if (proc == NULL) {
		return 0;
	}

	char path[50];

	while ((dir = readdir(proc)) != NULL) {
		path[0] = '\0';
		strcpy(path, "/proc/");
		if (isdigit(dir->d_name[0])) {
			strcat(path, dir->d_name);
            printf("%s\n", path);
		}
    }
    closedir(proc);
    return 0;
}