#ifndef DECODE_H
#define DECODE_H

#include<stdio.h>
#include<string.h>
#include "common.h"
#include "types.h" // Contains user defined types
//#include "encode.h"

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

/* 
 * Structure to store information required for
 * decoding secret file from stego Image
 * Info about output and intermediate data is
 * also stored
 */

typedef struct _DecodeInfo
{
	/* Source Image info */
    	//char *dest_image_fname;
	//FILE *fptr_dest_image;
    	//uint image_capacity;
    	//uint bits_per_pixel;
    	char image_data[MAX_IMAGE_BUF_SIZE];

    	/* Secret File Info */
    	char *secret_fname;
    	FILE *fptr_secret;
	char op_data[1000];
    	char extn_secret_file[MAX_FILE_SUFFIX];
    	char secret_data[MAX_SECRET_BUF_SIZE];
    	int size_secret_file;
	int size_secret_file_extn;

    	/* Stego Image Info */
	//Now source file
    	char *stego_image_fname;
    	FILE *fptr_stego_image;

	//validation 
	char string[50];
	char * token;

} DecodeInfo;

/* Decoding function prototype */

//function prototype to validate the CLA arguments
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_decode(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);

//main function that calls the remaining decode functions
Status do_decoding(DecodeInfo *decInfo);

//function to decode data from lsb 
char decode_byte_from_lsb(char *image_buffer);

//function call to decode data from the image
Status decode_data_from_image(int size, DecodeInfo *decInfo);

//function to decode magic string
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

//function to decode file ext size
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

//function to decode and get file extension
Status decode_secret_file_extn(DecodeInfo *decInfo);

#endif