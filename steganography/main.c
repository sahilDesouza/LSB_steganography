/*
Author: Sahil Desouza
Date : 10/08/2020
Title: Steganography
*/

//including header files
#include "encode.h"
//#include "encode.c"
#include "types.h"
#include "decode.h"


int main(int argc, char **argv)
{
	//declaring variable type
	EncodeInfo encInfo;
	DecodeInfo decInfo;
	int type, status;
	//calling check operation inorder to know if we are doing encoding or decoding
	type = check_operation_type(argv);
	//type encode
	if (type == e_encode)
	{
		printf("Encoding Selected\n");
		//Validate Command line arguments
		status = read_and_validate_encode_args(argv, &encInfo);
		if (status == e_success)
		{
			printf("Read and Validate Success\n");
			//calling encoding function and passing address of encInfo into it
			if (do_encoding(&encInfo) == e_success)
			{
				printf("Encoding success\n");
			}
			else
			{
				printf("Encoding Failure\n");
				return -1;
			}
		}
		else
		{
			printf("Invalid input\n");
			return -1;
		}
	}
	//type decode
	else if (type == e_decode)
	{
		printf("Decoding Selected\n");
		//Validate Command line arguments
		status = read_and_validate_decode_args(argv, &decInfo);
		if (status == e_success)
		{
			printf("Read and Validate Success\n");
			//calling decoding function and passing address of decInfo into it
			if (do_decoding(&decInfo) == e_success)
			{
				printf("Decoding success\n");
			}
			else
			{
				printf("Decoding Failure\n");
				return -1;
			}
		}
		else
		{
			printf("Invalid input\n");
		}
	}
	//if invalid type return -1
	else
	{
		printf("INVALID input neither -e nor -d entered in the command line\n");
		return -1;
	}
	return 0;
}