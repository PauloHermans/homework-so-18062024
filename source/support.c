#include "support.h"
#include <stdio.h>
#include <string.h>

/*
 * NOTE - Modificação
 * Motivo: toupper() não declarado
 * Diff: ø → #include <ctype.h>
 */
#include <ctype.h>

#include <stdbool.h>
#include "fat16.h"

/*
 * NOTE - Modificação
 * Motivo: Retornando por ponteiro variável de Stack
 *         Guardando 12 bytes em array de 11.
 *         Sem detecção de erro.
 *         Nome não fazia juz a função.
 */

/* Manipulate the path to lead com name, extensions and special characters */
bool cstr_to_fat16wnull(char *filename, char output[FAT16STR_SIZE_WNULL])
{


	char* strptr = filename;
	char* dot;
	dot = strchr(filename, '.');

	if (dot == NULL) return true;

	int i;
	for(i=0; strptr != dot; strptr++, i++){
		if(i==8)
			break;
		output[i] = *strptr;
	}

	int trail = 8 - i;
	for(; trail > 0; trail--, i++){
		output[i] = ' ';
	}

	strptr = dot;
	strptr++;
	for(i=8; i < 11; strptr++, i++){
		output[i] = *strptr;
	}

	output[11] = '\0';
	for(i = 0; output[i] != '\0'; i++){
		output[i] = toupper(output[i]);
	}

	return false;
}
