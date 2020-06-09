/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef _____DEBUG_______
#define _____DEBUG_______

#include <stdio.h>

#ifdef ZW_DEBUG 

#define DEBUG(...) \
{					\
	char error_msg_buf[40960]; \
	sprintf(error_msg_buf, __VA_ARGS__);\
	fprintf(stderr, "%s\n", error_msg_buf);\
}

#define DEBUG2(...) fprintf(stderr, __VA_ARGS__);fprintf(stderr, "\n");

#define wzw_assert(x, ...)  do{ \
	if(!(x)){\
		perror("error msg:");\
		char error_msg_buf[40960];\
		sprintf(error_msg_buf, "%20s:%-4d: ", __FILE__, __LINE__);\
		fprintf(stderr, "%s\n", error_msg_buf);\
        sprintf(error_msg_buf, #x);\
		fprintf(stderr, "%s\n", error_msg_buf);\
		sprintf(error_msg_buf, #__VA_ARGS__);                               \
		fprintf(stderr, "%s\n", error_msg_buf);\
		sprintf(error_msg_buf, "internal error");\
		fprintf(stderr, "%s\n", error_msg_buf);\
		exit(1);\
	}\
}while(0)


#else
#define wzw_assert(x, ...)  do{ \
	if(!(x)){\
		perror("error msg:");\
		char error_msg_buf[40960];\
		sprintf(error_msg_buf, "%20s:%-4d: ", __FILE__, __LINE__);\
		fprintf(stderr, "%s\n", error_msg_buf);\
        sprintf(error_msg_buf, #__VA_ARGS__);                               \
		fprintf(stderr, "%s\n", error_msg_buf);\
		sprintf(error_msg_buf, "wuzw's internal error");\
		fprintf(stderr, "%s\n", error_msg_buf);\
		exit(1);\
	}\
}while(0)

#define DEBUG(_fmt, ...)
#endif
#endif
