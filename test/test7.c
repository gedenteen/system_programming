#include <stdio.h>
#include <dirent.h> //для просмотра каталогов и файлов
#include <malloc.h>

int main() {
 	struct dirent **namelist;
	int n;
	n = scandir(".", &namelist, 0, alphasort);
	if (n < 0)
		perror("scandir");
	else {
		while(n--) {
			printf("%s\n", namelist[n]->d_name);
			free(namelist[n]);
		}	
		free(namelist);
	}
	/*
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (".")) != NULL) {
		// print all the files and directories within directory 
		while ((ent = readdir (dir)) != NULL) {
			printf("%s \n", ent->d_name);
			//stat для получения другой инф-ии
		}
		closedir (dir);
	} else {
		// could not open directory 
		perror ("");
		return 1;
	}
	*/
	return 0;
}
