#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdbool.h>
#include "commands.h"
#include "fat16.h"
#include "support.h"

#include <errno.h>
#include <err.h>
#include <error.h>

/*
 * NOTE - Modificação
 * Motivo: off_t não definido
 * Diff: ø → #include <sys/types.h>
 */
#include <sys/types.h>

/*
 * NOTE - Modificação
 *
 * Motivo: É necessário representar o estado no qual o find não achou o arquivo.
 *         É necessário representar o index de onde o fat_dir está em relação ao
 *         diretório raíz.
 *         Strings FAT16 não usam nullbyte como delimitador.
 *         Nome não faz juz à função
 */

struct far_dir_searchres find_in_root(struct fat_dir *dirs, char *filename, struct fat_bpb *bpb)
{

	(void) bpb;

	struct far_dir_searchres res = { .found = false };

	for (size_t i = 0; i < sizeof (struct fat_dir) * bpb->possible_rentries; i++)
	{

		/*
		 * NOTE - Modificação
		 * Motivo: É necessário skipar entradas nulas (representadas com name[0] == '\0')
		 * Diff: ø → if (dirs[i].name[0] == '\0') continue;
		 */
		if (dirs[i].name[0] == '\0') continue;

		if (memcmp((char *) dirs[i].name, filename, FAT16STR_SIZE) == 0)
		{
			res.found = true;
			res.fdir  = dirs[i];
			res.idx   = i;
			break;
		}
	}

	return res;
}

struct fat_dir *ls(FILE *fp, struct fat_bpb *bpb)
{

	struct fat_dir *dirs = malloc(sizeof (struct fat_dir) * bpb->possible_rentries);

	for (int i = 0; i < bpb->possible_rentries; i++)
	{
		uint32_t offset = bpb_froot_addr(bpb) + i * sizeof(struct fat_dir);
		read_bytes(fp, offset, &dirs[i], sizeof(dirs[i]));
	}

	return dirs;
}

void mv(FILE *fp, char *source, char* dest, struct fat_bpb *bpb)
{

	char source_rname[FAT16STR_SIZE_WNULL], dest_rname[FAT16STR_SIZE_WNULL];

	bool badname = cstr_to_fat16wnull(source, source_rname)
	            || cstr_to_fat16wnull(dest,   dest_rname);

	if (badname)
	{
		fprintf(stderr, "Nome de arquivo inválido.\n");
		exit(EXIT_FAILURE);
	}

	uint32_t root_address = bpb_froot_addr(bpb);
	uint32_t root_size    = sizeof (struct fat_dir) * bpb->possible_rentries;

	struct fat_dir root[root_size];

	if (read_bytes(fp, root_address, &root, root_size) == RB_ERROR)
		error_at_line(EXIT_FAILURE, EINVAL, __FILE__, __LINE__, "erro ao ler struct fat_dir");

	struct far_dir_searchres dir1 = find_in_root(&root[0], source_rname, bpb);
	struct far_dir_searchres dir2 = find_in_root(&root[0], dest_rname,   bpb);

	if (dir2.found == true)
		error(EXIT_FAILURE, 0, "Não permitido substituir arquivo %s via mv.", dest);

	if (dir1.found == false)
		error(EXIT_FAILURE, 0, "Não foi possivel encontrar o arquivo %s.", source);

	memcpy(dir1.fdir.name, dest_rname, sizeof(char) * FAT16STR_SIZE);

	uint32_t source_address = sizeof (struct fat_dir) * dir1.idx + root_address;

	(void) fseek (fp, source_address, SEEK_SET);
	(void) fwrite
	(
		&dir1.fdir,
		sizeof (struct fat_dir),
		1,
		fp
	);

	printf("mv %s → %s.\n", source, dest);

	return;
}

void rm(FILE* fp, char* filename, struct fat_bpb* bpb)
{

	(void) fp, (void) filename, (void) bpb;

	;; /* TODO */
}

void cp(FILE *fp, char *filename, struct fat_bpb *bpb)
{

	(void) fp, (void) filename, (void) bpb;

	;; /* TODO */
}

/*
 * NOTE - Remoção
 * Motivo: Código morto.
 *
 * off_t fsize(const char *filename){
 * 	struct stat st;
 * 	if (stat(filename, &st) == 0)
 * 		return st.st_size;
 * 	return -1;
 * }
 */

/*
 * NOTE - Remoção
 * Motivo: Fazer na mão é mais facíl.
 *
 * int write_dir(FILE *fp, char *fname, struct fat_dir *dir){
 *    char* name = padding(fname);
 *    strcpy((char *) dir->name, (char *) name);
 *    if (fwrite(dir, 1, sizeof(struct fat_dir), fp) <= 0)
 *        return -1;
 *    return 0;
 * }
 */

/*
 * NOTE - Remoção
 * Motivo: Código morto
 *
 * int write_data(FILE *fp, char *fname, struct fat_dir *dir, struct fat_bpb *bpb){
 *
 * 	 *
 * 	 * NOTE - Modificação
 * 	 * Motivo: Variáveis não usadas
 * 	 * Diff: ø → (void) dir, (void) bpb;
 * 	 *
 * 	(void) dir, (void) bpb;
 *
 * 	FILE *localf = fopen(fname, "r");
 * 	int c;
 *
 * 	while ((c = fgetc(localf)) != EOF){
 * 		if (fputc(c, fp) != c)
 * 			return -1;
 * 	}
 * 	return 0;
 * }
 */

/*
 * NOTE - Remoção
 * Motivo: Código morto
 * int wipe(FILE *fp, struct fat_dir *dir, struct fat_bpb *bpb)
 * {
 * 	int start_offset = bpb_froot_addr(bpb) + (bpb->bytes_p_sect * dir->starting_cluster);
 * 	int limit_offset = start_offset + dir->file_size;
 *
 * 	while (start_offset <= limit_offset){
 * 		fseek(fp, ++start_offset, SEEK_SET);
 * 		if(fputc(0x0, fp) != 0x0)
 * 			return 01;
 * 	}
 * 	return 0;
 * }
*/
