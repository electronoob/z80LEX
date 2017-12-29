#include <stdio.h>
#define TMPF_BUFFER_SIZE 1024

char *src_filename_buffer;
char *set_bitrate_buffer;
unsigned char *rawData;
unsigned int rawData_len;

unsigned char * import_file(char * file)
{
  FILE * input;
  input = fopen(file, "rb");
  if (input == NULL)
  {
    fprintf(stderr, "Unable to open file for reading.\n");
    exit(1);
  }
  unsigned int bytes;
  unsigned int the_file_len = 0;
  unsigned char * the_file_data;
  unsigned char * tmp_buffer;
  unsigned char * tmp_ptr;
  tmp_ptr = NULL;
  tmp_buffer = malloc(TMPF_BUFFER_SIZE);
  the_file_data = malloc(TMPF_BUFFER_SIZE);
  memset(the_file_data, 0, TMPF_BUFFER_SIZE);
  while (1)
  {
    memset(tmp_buffer, 0, TMPF_BUFFER_SIZE);
    bytes = fread(tmp_buffer, 1, TMPF_BUFFER_SIZE, input);
    if (bytes == 0)
    {
      break;
    }
    if ((tmp_ptr = realloc(the_file_data, the_file_len + bytes)) == NULL)
    {
      fprintf(stderr, "Unable to realloc().\n");
      exit(1);
    } else {
      the_file_data = tmp_ptr;
      memcpy(the_file_data + the_file_len, tmp_buffer, bytes);
    }
    the_file_len += bytes;
    if (bytes != TMPF_BUFFER_SIZE)
    {
      break;
    }
  }

  rawData_len = the_file_len;
  free(tmp_buffer);
  fclose(input);
  return the_file_data;
}

	void showhelp() {
		printf("example:\n");
		printf("\t./crusher --input mysing.raw --alsa\n");
		printf("\t./crusher --input mysing.raw --arduino --bitrate 10 > arduino_sketch.c\n");
		printf("\t./crusher --input mysing.raw --alsa --bitrate 2\n\n");
	}
	void showabout() {
                //printf("beverly-crusher (build %d) [http://electronoob.com]\n\n",BUILDNUMBER);
                printf("Released under the terms of the MIT license.\nCopyright (c) 2014 electronoob.\nAll rights reserved.\n\n");
	}

int main (int argc, char *argv[])
{
	/* begin processing commandline */
	int i;
	int input_from_file;
	for (i=1;i<argc;i++)
	{	
		if (strcmp("--input", argv[i]) == 0)
		{
        i++;
                                if(argc == i)
				{
					showabout();
					showhelp();
                                        fprintf (stderr, "--input [filename] error. missing filename parameter.\n");
                                        exit(1);
                                }
                                src_filename_buffer = malloc(strlen(argv[i]) +1);
                                if (src_filename_buffer == NULL) 
				{
					showabout();
                                        fprintf (stderr, "--input [filename] error. unable to malloc() memory.\n");
                                        exit(1);
                                }
                                memset(src_filename_buffer, 0, strlen(argv[i]) +1);
                                strcpy(src_filename_buffer, argv[i]);
				input_from_file = 1;
                                continue;
                        }
	}
	if(!input_from_file)
	{
			showabout();
			showhelp();
			fprintf (stderr, "error: --input not set.\n");
			exit(1);
	} else 
	{
		rawData = import_file(src_filename_buffer);
		printf("cocks\n");
	}
	free(src_filename_buffer);
	free(rawData);	
	return 0;
}

