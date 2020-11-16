//including header files
#include "decode.h"
#include "types.h"

//function to get the arguments from the command line
//if the arguments are txt, bmp and in the proper order then return success
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
	//spliting the argument to get the extension file only
	//spliting using strtok function
	strcpy(decInfo->string, argv[2]);
	decInfo->token = strtok(decInfo->string, ".");
	decInfo->token = strtok(NULL, "\0");
	strcpy(decInfo->string, decInfo->token);
	if (strcmp(decInfo->string, "bmp") == 0)
	{
		decInfo->stego_image_fname = argv[2];
	}
	else
	{
		return e_failure;
	}
	if (argv[3] == NULL)
	{
		decInfo->secret_fname = "decode.txt";
	}
	else if (argv[3] != NULL)
	{
		strcpy(decInfo->string, argv[3]);
		decInfo->token = strtok(decInfo->string, ".");
		decInfo->token = strtok(NULL, "\0");
		strcpy(decInfo->string, decInfo->token);
		if (strcmp(decInfo->string, "txt") == 0)
		{
			strcpy(decInfo->extn_secret_file, ".txt");
			decInfo->secret_fname = argv[3];
		}
		else
		{
			return e_failure;
		}
		
	}
	//printf("1");
	return e_success;
}
//Function call to open respective files
Status open_files_decode(DecodeInfo *decInfo)
{
	//Secret file
    	decInfo->fptr_secret = fopen(decInfo->secret_fname, "w+");
    	//Do Error handling
   	if (decInfo->fptr_secret == NULL)
    	{
    		perror("fopen");
    		fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_fname);
    		return e_failure;
   	}

    	//Stego Image file
    	decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    	//Do Error handling
    	if (decInfo->fptr_stego_image == NULL)
    	{
    		perror("fopen");
    		fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);
    		return e_failure;
    	}

    	// No failure return e_success
    	return e_success;
}
//function call to decode data from lsb
char decode_byte_from_lsb(char *image_buffer)
{
	unsigned char bit, mask = 1, arr[8], ch;
    	unsigned int i,j,sum = 0;

    	for ( i = 0; i < 8; i++ )
    	{
		*(arr + i) = *(image_buffer + i) & mask;
    	}
	//calculating the decimal value of the data stored in arr
	//calculating from msb since binary values are in reverse order
    	j = 128;
    	for ( i = 0; i < 8; i++ )
    	{
		sum = sum + (arr[i] * j);
		j = j / 2;
    	}
	//returning character equivalent value
    	return ((char)sum);
}
//function to decode data that is read from stego.bmp
//number of bytes read depends on size
Status decode_data_from_image(int size, DecodeInfo *decInfo)
{
	unsigned char a[8];
    	int i;
    	for( i = 0; i < size; i++ )
    	{
		fread(a, 8, 1, decInfo->fptr_stego_image );
		*(decInfo->op_data + i) = decode_byte_from_lsb(a);
    	}
    	*(decInfo->op_data + i) = '\0'; 
    	
    	return e_success;
}
//function to decode magic string
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
	// Seek to 55th byte and onward
    	fseek(decInfo->fptr_stego_image, 55L, SEEK_SET);
	decode_data_from_image(strlen(magic_string), decInfo);

	//Checking existence of Magic string in the bmp file
	if (strcmp(decInfo->op_data, MAGIC_STRING) != 0 )		   
    	{
		printf("INFO: Error: Magic string does not match\n");
		return e_failure;
    	}
    	//printf("%s", decInfo->op_data);
    
	return e_success;
}
//function to decode file extension (.txt)
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
	//since we need to get 4 bytes from 32 bytes
	//pass 4 into the function
	//that means decode_data_from_image function will decode 32 bytes of data
    	decode_data_from_image(sizeof(int), decInfo);
    	strcpy(decInfo->extn_secret_file, decInfo->op_data);
    	//printf("%s", decInfo->op_data);
    	return e_success;
}
//function to decode file extension size(4)
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
	int sum = 0;
    	decode_data_from_image(sizeof(int), decInfo);
    	//decInfo->size_secret_file_extn = decInfo->op_data[0];
	for (int i = 0; i < sizeof(int); i++)
	{
		sum += decInfo->op_data[i];
	}
	
	decInfo->size_secret_file_extn = sum;
    	//decInfo->size_secret_file_extn = decInfo->op_data[3];
    	//printf("%d", decInfo->size_secret_file_extn);
    	return e_success;
}
//function to get the file extention size from stego.bmp
Status decode_secret_file_size(DecodeInfo *decInfo)
{
	int sum = 0;
	decode_data_from_image(sizeof(int), decInfo);
    	//decInfo->size_secret_file = decInfo->op_data[0];
	for (int i = 0; i < sizeof(int); i++)
	{
		sum += decInfo->op_data[i];
	}
	decInfo->size_secret_file =  sum;		
	//decInfo->size_secret_file =  decInfo->op_data[3];
	//printf("%d", decInfo->size_secret_file);
    	return e_success;
}
//function to get the encoded data from stego.bmp
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    decode_data_from_image(decInfo->size_secret_file, decInfo);
    //printf("%s", decInfo->op_data);
    fwrite(decInfo->op_data, decInfo->size_secret_file, 1 ,decInfo->fptr_secret);
    return e_success;
}
//main function that calls the appropriate functions to do the decoding process
Status do_decoding(DecodeInfo * decInfo)
{
	if (open_files_decode(decInfo) == e_success)
	{
		//if files opened succesfully decoded prints
		printf("Decoding Started\n");
		printf("Info: open file success\n");
		if (decode_magic_string(MAGIC_STRING, decInfo) == e_success)
		{
			//magic string decoded succesfully
			printf("Info: Magic string decoded\n");
			if (decode_secret_file_extn_size(decInfo) == e_success)
			{
				//secret file extension size decoded
				printf("Info: Secret file extension size decoded\n");
				if (decode_secret_file_extn(decInfo) == e_success)
				{
					//secret file extension decoded
					printf("Info: Secret file extension decoded\n");
					if (decode_secret_file_size(decInfo) == e_success)
					{
						//secret file size decoded
						printf("Info: Secret file size decoded\n");
						if (decode_secret_file_data(decInfo) == e_success)
						{
							//all decoding process done soo return success
							return e_success;
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
		printf("ERROR: Open files failure\n");
		return e_failure;
	}
}
	
