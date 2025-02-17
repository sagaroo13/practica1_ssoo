#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int	crear(char *filename, char *mode_octal)
{
	mode_t	old_umask;
	mode_t	mode;
	int		fd;

	// Obtener el nombre del archivo y el modo en octal
	mode = strtol(mode_octal, NULL, 8);
	// Convertir el modo de cadena a octal
	// Guardar la máscara de creación actual
	old_umask = umask(0);
	// Crear el archivo con el modo especificado
	fd = open(filename, O_CREAT | O_WRONLY, mode);
	if (fd == -1)
	{
		perror("Error al crear el archivo");
		umask(old_umask); // Restaurar la máscara original
		return (-1);
	}
	close(fd);
	// Restaurar la máscara original
	umask(old_umask);
	return (0);
}

int	main(int argc, char *argv[])
{
	int	return_value;

	// Verificar número de argumentos
	if (argc != 3)
	{
		printf("Uso: %s <fichero> <modo>\n", argv[0]);
		return (-1);
	}
	return_value = crear(argv[1], argv[2]);
	return (return_value);
}
