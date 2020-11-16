//including header files
#include "encode.h"
#include "types.h"

//function to get file size of beautiful.txt
uint get_image_size_for_bmp(FILE *fptr_image)
{
    	uint width, height;
    	// Seek to 18th byte
    	fseek(fptr_image, 18, SEEK_SET);

    	// Read the width (an int)
    	fread(&width, sizeof(int), 1, fptr_image);
    	printf("width = %u\n", width);

    	// Read the height (an int)
    	fread(&height, sizeof(int), 1, fptr_image);
    	printf("height = %u\n", height);

	fseek(fptr_image, 0L, SEEK_SET);
    	// Return image capacity
    	return width * height * 3;
}
//function call to return file size of secret.txt
uint get_file_size(FILE *fptr)
{
    	uint file_size;
    
    	// Seek to nth byte
    	fseek(fptr, 0L, SEEK_END);
    	// Save the current position value
    	file_size = ftell(fptr);
    	// Seek back to 0th byte
    	fseek(fptr, 0L, SEEK_SET);
    
    	//Return the size
    	return file_size;
}
//function call to open the files
//if opened succesfull then return success
Status open_files(EncodeInfo *encInfo)
{
	// Src Image file
    	encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    	// Do Error handling
    	if (encInfo->fptr_src_image == NULL)
    	{
		
    		perror("fopen");
    		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
    		return e_failure;
    	}

    	// Secret file
    	encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    	// Do Error handling
    	if (encInfo->fptr_secret == NULL)
    	{
    		perror("fopen");
    		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
    		return e_failure;
   	}

    	// Stego Image file
    	encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    	// Do Error handling
    	if (encInfo->fptr_stego_image == NULL)
    	{
    		perror("fopen");
    		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
    		return e_failure;
    	}

    	// No failure return e_success
    	return e_success;
}
//function call common to both ENCODING and DECODING
OperationType check_operation_type(char *argv[])
{
	if (strcmp(argv[1], "-e") == 0)
	{
		return e_encode;
	}
	else if (strcmp(argv[1], "-d") == 0)
	{
		return e_decode;
	}
	else
	{
		return e_unsupported;
	}
	
}
//function to get the arguments from the command line
//if the arguments are txt, bmp and in the proper order then return success
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
	//spliting the argument to get the extension file only
	//spliting using strtok function
	strcpy(encInfo->string, argv[2]);
	encInfo->token = strtok(encInfo->string, ".");
	encInfo->token = strtok(NULL, "\0");
	strcpy(encInfo->string, encInfo->token);
	if (strcmp(encInfo->string, "bmp") == 0)
	{
		encInfo->src_image_fname = argv[2];
	}
	else
	{
		return e_failure;
	}
	strcpy(encInfo->string, argv[3]);
	encInfo->token = strtok(encInfo->string, ".");
	encInfo->token = strtok(NULL, "\0");
	strcpy(encInfo->string, encInfo->token);
	if (strcmp(encInfo->string, "txt") == 0)
	{
		strcpy(encInfo->extn_secret_file, ".txt");
		encInfo->secret_fname = argv[3];
	}
	else
	{
		return e_failure;
	}
	if (argv[4] == NULL)
	{
		encInfo->stego_image_fname = "stego.bmp";
	}
	else if (argv[4] != NULL)
	{
		strcpy(encInfo->string, argv[4]);
		encInfo->token = strtok(encInfo->string, ".");
		encInfo->token = strtok(NULL, "\0");
		strcpy(encInfo->string, encInfo->token);
		if (strcmp(encInfo->string, "bmp") == 0)
		{
			encInfo->stego_image_fname = argv[4];
		}
		else
		{
			return e_failure;
		}
		
	}
	return e_success;
}
//function to check capacity of beautiful.bmp
//if capacity greater than certain value then return success
Status check_capacity(EncodeInfo *encInfo)
{
	encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
	encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
	
	if (encInfo->image_capacity > (166 + 8 * (encInfo->size_secret_file)))
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}
}
//function to copy header file of beautiful.bmp to stego.bmp
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
	char image_header_data[55];
    	int readptr, writeptr, i;
    
    	/* To read exactly 54 bytes that contain header info */
    	if ((readptr = fread(image_header_data, 1, 54, fptr_src_image)) != 54)
    	{
        	if (ferror(fptr_src_image) != 0)
        	{
            		fprintf(stderr, "Reading error. \n" );
            		clearerr(fptr_src_image);
            		return e_failure;
        	}
    
	}
        
    	/* If failed to write what is read into dest header */
    	if ((writeptr = fwrite(image_header_data, 1, readptr, fptr_dest_image)) != readptr)
    	{
        	return e_failure;
    	}
    
    	return e_success;
}
//function to encode the lsb of a particular byte from the image with a provided data string
Status encode_byte_to_lsb(unsigned char data, char *image_buffer)
{
	for (int i = 0; i < 8; i++)
	{
		image_buffer[i] = (0xFE & image_buffer[i]) | ((data & (1 << 7)) >> 7);
		data <<= 1;
	}
	
}
//function to encode lsb
Status encode_size_to_LSB(int size, char *buffer)
{
	for (int i = 0; i < 32; i++)
	{
		buffer[i] = (0xFE & buffer[i]) | ((size & (1 << 31)) >> 31);
		size <<= 1;
	}
	
}
//function to encode magic string with the file data
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
	// Seek to 55th byte and onward
    	fseek(encInfo->fptr_src_image, 55L, SEEK_SET);
    	// Seek to 55th byte and onward
    	fseek(encInfo->fptr_stego_image, 55L, SEEK_SET);
	//printf("%d", ftell(encInfo->fptr_src_image));
	//printf("%d", ftell(encInfo->fptr_stego_image));
	encode_data_to_image(magic_string, strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);
	return e_success;


}
//function to get 8 bytes at a time and encodes it using encode to lsb
//number of bytes you want to encode depends on size
Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image, EncodeInfo *encInfo)
{
	for (int i = 0; i < size; i++)
	{
		fread(encInfo->image_data, 8L, sizeof(char), encInfo->fptr_src_image);
		encode_byte_to_lsb(data[i], encInfo->image_data);
		fwrite(encInfo->image_data, 8L, sizeof(char), encInfo->fptr_stego_image);
		
	}
	return e_success;
}
//function to encode secret file extension
Status encode_size(int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
	char arr[32];
	fread(arr, 32, sizeof(char), fptr_src_image);
	encode_size_to_LSB(size, arr);
	fwrite(arr, 32, sizeof(char), fptr_stego_image);
	return e_success;

}
//function to encode extension(.txt) to stego.bmp
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
	encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);
	return e_success;

}
//functon to encode file size(4) to stego.bmo
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
	char arr[32];
	fread(arr, 32, sizeof(char), encInfo->fptr_src_image);
	encode_size_to_LSB((int)file_size, arr);
	fwrite(arr, 32, sizeof(char), encInfo->fptr_stego_image);
	return e_success;
}
//function o encode the data from secret file to stego.bmp
Status encode_secret_file_data(EncodeInfo *encInfo)
{
	char ch;
	for (int  i = 0; i < encInfo->size_secret_file; i++)
	{
		fread(encInfo->image_data, 8, sizeof(char), encInfo->fptr_src_image);
		fread(&ch, 1, sizeof(char), encInfo->fptr_secret);
		encode_byte_to_lsb(ch, encInfo->image_data);
		fwrite(encInfo->image_data, 8, sizeof(char), encInfo->fptr_stego_image);
	}
	return e_success;
}
//function to copy remaining data to the stego.bmp file
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
	char buffer;
	
	while (fread(&buffer, 1, 1, fptr_src) != 0)
	{
		fwrite(&buffer, 1, 1, fptr_dest);
	}
}
//main function that calls the appropriate functions to do the encoding process
Status do_encoding(EncodeInfo * encInfo)
{
	//open the files
	if (open_files(encInfo) == e_success)
	{
		printf("Encoding Started\n");
		printf("INFO: open file success\n");
		//checking capacity of beautiful.bmp
		if (check_capacity(encInfo) == e_success)
		{
			printf("Info: check capacity success\n");
			//copy bmp header to stego.bmp
			if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
			{
				printf("Info: copy bmp header success\n");
				//encode magic string to stego.bmp
				if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
				{
					printf("Info: Magic string encoded\n");
					//encode secret file extension size to stego.bmp
					if (encode_size(strlen(".txt"), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
					{
						printf("Info: Secret file extension size encoded\n");
						//encode secret file extension to stego.bmp
						if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
						{
							printf("Info: Secret file extension encoded\n");
							//encode secret file size to stego.bmp
							if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
							{
								printf("Info: Secret file size encoded\n");
								//encode secret file data to stego.bmp
								if (encode_secret_file_data(encInfo) == e_success)
								{
									//copy remaining data to stego.bmp
									if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
									{
										printf("Info: remaining bytes copied\n");
										return e_success;
									}
									else
									{
										printf("ERROR:remaining bytes not copied\n");
										return e_failure;
									}
								}
								else
								{
									printf("ERROR:Secret file size failure\n");
									return e_failure;
								}
							}
							else
							{
								printf("ERROR:Secret file size failure\n");
								return e_failure;
							}
							
						}
						else
						{
							printf("ERROR:Info: Secret file extension failure\n");
							return e_failure;
						}

					}
					else
					{
						printf("ERROR:Secret file extension size failure\n");
						return e_failure;
					}
				}
				else
				{
					printf("ERROR:Magic string failure\n");
					return e_failure;
				}
				
			}
			else
			{
				printf("ERROR:Copying header failure\n");
				return e_failure;
			}
		}
		else
		{
			printf("ERROR:Capacity not enough\n");
			return e_failure;
		}
	}
	else
	{
		printf("ERROR: Open files failure\n");
		return e_failure;
	}
	return e_success;
	
}
