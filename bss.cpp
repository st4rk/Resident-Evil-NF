/*
	BSS file depacker

	Copyright (C) 2009	Patrice Mandin

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <SDL/SDL.h>

#include "depack_vlc.h"
#include "depack_mdec.h"
#include "file_functions.h"

int convert_image(const char *filename)
{
	SDL_RWops *src;
	Uint8 *dstBuffer;
	int dstBufLen;
	int retval = 1;

	src = SDL_RWFromFile(filename, "rb");
	if (!src) {
		fprintf(stderr, "Can not open %s for reading\n", filename);
		return retval;
	}

	 int i = 0;
	 int length = SDL_RWseek(src, 0, RW_SEEK_END);
	 SDL_RWseek(src, 0, RW_SEEK_SET);
	 
	 while (SDL_RWseek(src, 0, RW_SEEK_CUR) < length) {
	  vlc_depack(src, &dstBuffer, &dstBufLen);
	  
	  int pos = SDL_RWseek(src, 0, RW_SEEK_CUR);
	  if ((pos & 0x7fff) != 0) SDL_RWseek(src, (pos & ~0x7fff) + 0x8000, RW_SEEK_SET);
	  
	  if (dstBuffer && dstBufLen) {
	   SDL_RWops *mdec_src;

	   mdec_src = SDL_RWFromMem(dstBuffer, dstBufLen);
	   
	   if (mdec_src) {
	    Uint8 *dstMdecBuf;
	    int dstMdecLen;

	    mdec_depack(mdec_src, &dstMdecBuf, &dstMdecLen, 320,240);
	    SDL_RWclose(mdec_src);

	    if (dstMdecBuf && dstMdecLen) {
	      SDL_Surface *image = mdec_surface(dstMdecBuf,320,240,0);
	     
	     if (image) {
		      char outname[10];
		      sprintf(outname, "%d", i++);
		      save_bmp(outname, image);

		      SDL_FreeSurface(image);

		      retval = 0;
	     }

	     free(dstMdecBuf);
	   }
	  }
	   	free(dstBuffer);
	  }
	}

	return retval;
}
