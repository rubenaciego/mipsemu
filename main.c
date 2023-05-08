#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mipscpu.h>

#define MEMORY 0xffff

uint8_t* load_file(uint64_t* fsize, char* filename);

int main(int argc, char* argv[])
{
	if (argc < 2)
		goto end;

	if (strcmp(argv[1], "-e") == 0)
	{
		struct mipscpu cpu;
		initmips(&cpu);
		uint64_t file_size;
		uint8_t* file_data = load_file(&file_size, argv[2]);
		cpu.memsize = file_size + MEMORY;
		cpu.memory = malloc(cpu.memsize);
		memset(cpu.memory, 0, cpu.memsize);
		memcpy(cpu.memory + MEMORY, file_data, file_size);
		cpu.pc = MEMORY;

		free(file_data);

		while (cpu.running && cpu.pc < cpu.memsize)
			emulate_cycle_mips(&cpu);

		free(cpu.memory);
	}
	else if (strcmp(argv[1], "-d") == 0)
	{
		struct mipscpu cpu;
		initmips(&cpu);
		cpu.memory = load_file(&cpu.memsize, argv[2]);

		while (cpu.pc < cpu.memsize)
        {
			disassemble_mips(&cpu);
            cpu.pc += INS_SIZE;
        }

		free(cpu.memory);
	}
	else if (strcmp(argv[1], "-a") == 0)
	{
		uint64_t size;
		uint8_t* data = load_file(&size, argv[2]);

		char* str = malloc(strlen(argv[2]) * sizeof(char));
		strcpy(str, argv[2]);

		int dotpos = 0;

		while (str[++dotpos] != '.' && dotpos != strlen(argv[2]));
		str[dotpos] = 0;

		assemble_mips(data, str, &size);

		free(str);
		free(data);
	}
	else
	{
		end:
		printf("Usage:\n-e FILE\tEmulate\n-d FILE\tDisassemble\n-a FILE\tAssemble\n");
	}

    return 0;
}

uint8_t* load_file(uint64_t* fsize, char* filename)
{	
	FILE* file = fopen(filename, "rb");
	if (file == NULL)
	{
		printf("File error: %s\n", filename);
		exit(-1);
	}

	fseek(file, 0, SEEK_END);
	*fsize = ftell(file);
	rewind(file);

	uint8_t* buffer = (uint8_t*)malloc(*fsize);

	size_t result = fread(buffer, 1, *fsize, file);
	if (result != *fsize) 
	{
		printf("Reading error\n");
		exit(-1);
	}

	fclose(file);

    return buffer;
}
