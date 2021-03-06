/*
 * Mp3Splt -- Utility for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2011 Alexandru Munteanu - <io_fx@yahoo.fr>
 *
 * http://mp3splt.sourceforge.net
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111, USA.
 */

#ifdef __WIN32__

#include "windows_utils.h"
#include "utils.h"
#include "print_utils.h"

char **win32_get_utf8_args(main_data *data)
{
  char **argv_utf8 = my_malloc(sizeof(char *) * data->argc);
  LPWSTR *argv_utf16 = NULL;
  int nArgs = 0;
  int i = 0;

  argv_utf16 = CommandLineToArgvW(GetCommandLineW(), &nArgs);
  if (argv_utf16 == NULL )
  {
    if (argv_utf8)
    {
      free(argv_utf8);
      argv_utf8 = NULL;
    }

    print_error_exit(_("CommandLineToArgvW failed (oh !)"), data);
  }
  else
  {
    for (i=0; i<nArgs; i++)
    {
      argv_utf8[i] = mp3splt_win32_utf16_to_utf8((wchar_t *)argv_utf16[i]);
      if (argv_utf8[i] == NULL)
      {
        print_error_exit(_("failed to allocate argv_utf8 memory"), data);
      }
    }

    LocalFree(argv_utf16);
    argv_utf16 = NULL;
  }

  return argv_utf8;
}

#endif

