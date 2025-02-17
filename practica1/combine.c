#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef MAX_ALUMNOS
# define MAX_ALUMNOS 100
#endif


struct alumno
{
	char nombre[50];
	int nota;
	int convocatoria;
};

void	bubble_sort(int total, struct alumno *alumnos)
{
	int i;
	int j;
	struct alumno temp;

	i = 0;
	while (i < total - 1)
	{
		j = 0;
		while (j < total - i - 1)
		{
			if (alumnos[j].nota > alumnos[j + 1].nota)
			{
				temp = alumnos[j];
				alumnos[j] = alumnos[j + 1];
				alumnos[j + 1] = temp;
			}
			j++;
		}
		i++;
	}
}

int	find_alumno(struct alumno *alumnos, struct alumno alumno, int total)
{
	int i;

	i = 0;
	while (i < total)
	{
		if (!strcmp(alumnos[i].nombre, alumno.nombre)
			&& alumnos[i].convocatoria == alumno.convocatoria)
			return (1);
		i++;
	}
	return (0);
}

int	read_file(int fd, struct alumno *alumnos)
{
	struct alumno temp;
	static int total;
	int struct_size;

	struct_size = sizeof(struct alumno);
	while (read(fd, &temp, struct_size) == struct_size)
	{
		if (total >= MAX_ALUMNOS)
		{
			perror("Error: Demasiados alumnos\n");
			close(fd);
			return (-1);
		}
		if (find_alumno(alumnos, temp, total))
		{
			perror("Error: Alumnos duplicados\n");
			close(fd);
			return (-1);
		}
		alumnos[total++] = temp;
	}
	close(fd);
	return (total);
}

int	check_read_error(int error, struct alumno *alumnos)
{
	if (error == -1)
	{
		free(alumnos);
		return (1);
	}
	return (0);
}

void	complete_statistics(struct alumno *alumnos, int size, int *categories)
{
	int i;

	i = 0;
	while (i < size)
	{
		if (alumnos[i].nota == 10)
			categories[0]++;
		else if (alumnos[i].nota == 9)
			categories[1]++;
		else if (alumnos[i].nota == 8 || alumnos[i].nota == 7)
			categories[2]++;
		else if (alumnos[i].nota == 6 || alumnos[i].nota == 5)
			categories[3]++;
		else
			categories[4]++;
		i++;
	}
}

int	generate_statistics(struct alumno *alumnos, int size)
{
	int fd_stats;
	int *categories;
	int	i;
	char buffer[100];
	const char labels[5] = {'M', 'S', 'N', 'A', 'F'};
	double	pertentage;
	int	len;

	categories = (int *)malloc(5 * sizeof(int));
	if (!categories)
	{
		perror("Error al asignar memoria");
		return (-1);
	}
	memset(categories, 0, 5 * sizeof(int));
	complete_statistics(alumnos, size, categories); // ¿Es un error que una nota no este comprendida entre 0 y 10?

	fd_stats = open("estadisticas.csv", O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (fd_stats == -1)
	{
		perror("Error al crear el archivo de estadísticas\n");
		free(categories);
		return (-1);
	}

	i = 0;
	while (i < 5)
	{
		pertentage = categories[i] * 100.0 / size;
		len = snprintf(buffer, sizeof(buffer), "%c;%d;%.2f%%\n", labels[i], categories[i], pertentage);
		write(fd_stats, buffer, len);
		i++;
	}
	free(categories);
	close(fd_stats);
	return (0);
}

int	combine(char *file_1, char *file_2, char *file_3)
{
	struct alumno *alumnos;
	int fd1;
	int fd2;
	int fd_out;
	int size;
	int	statistics_error;

	fd1 = open(file_1, O_RDONLY);
	fd2 = open(file_2, O_RDONLY);
	if (fd1 == -1 || fd2 == -1)
	{
		write(2, "Error al abrir los archivos de entrada\n", 40);
		return (-1);
	}

	alumnos = (struct alumno *)malloc(MAX_ALUMNOS * sizeof(struct alumno));
	if (!alumnos)
	{
		perror("Error al asignar memoria");
		return (-1);
	}

	size = read_file(fd1, alumnos);
	if (check_read_error(size, alumnos))
	{
		close(fd2);
		return (size);
	}
	size = read_file(fd2, alumnos);
	if (check_read_error(size, alumnos))
		return (size);

	bubble_sort(size, alumnos);

	fd_out = open(file_3, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (fd_out == -1)
	{
		write(2, "Error al abrir el archivo de salida\n", 37);
		return (-1);
	}
	write(fd_out, alumnos, size * sizeof(struct alumno));

	statistics_error = generate_statistics(alumnos, size);
	if (statistics_error == -1)
		return (statistics_error);

	free(alumnos);
	close(fd_out);
	return (0);
}

int	main(int argc, char *argv[])
{
	int return_value;

	if (argc != 4)
	{
		printf("Uso: %s <archivo1> <archivo2> <archivo_salida>\n", argv[0]);
		return (-1);
	}

	return_value = combine(argv[1], argv[2], argv[3]);
	return (return_value);
}
