#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

int walk_dir (const char *path, void (*func) (const char *))
{
	DIR *dirp;
	struct dirent *dp;
	char *p, *full_path;
	int len;

	/* abre o diretório */

	if ((dirp = opendir (path)) == NULL)
		return (-1);

	len = strlen (path);

	/* aloca uma área na qual, garantidamente, o caminho caberá */

	if ((full_path = malloc (len + NAME_MAX + 2)) == NULL)
	{
		closedir (dirp);
		return (-1);
	}

	/* copia o prefixo e acrescenta a ‘/’ ao final */

	memcpy (full_path, path, len);

	p = full_path + len; *p++ = '/'; /* deixa “p” no lugar certo! */

	while ((dp = readdir (dirp)) != NULL)
	{
		/* ignora as entradas “.” e “..” */
		
		if (strcmp (dp->d_name, ".") == 0 || strcmp (dp->d_name, "..") == 0)
			continue;
		
		strcpy (p, dp->d_name);
		
		/* “full_path” armazena o caminho */
		
		(*func) (full_path);
	}

	free (full_path);
	closedir (dirp);

	return(0);

}	/* end walk_dir */


int main(void){

	walk_dir("/home/vitor/Desktop/so2",printf);
	return 0;
}
