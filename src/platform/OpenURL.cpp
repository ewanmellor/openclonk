/*
 * OpenClonk, http://www.openclonk.org
 *
 * Copyright (c) 2008  Günther Brammer
 * Copyright (c) 2001-2009, RedWolf Design GmbH, http://www.clonk.de
 *
 * Portions might be copyrighted by other authors who have contributed
 * to OpenClonk.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * See isc_license.txt for full license and disclaimer.
 *
 * "Clonk" is a registered trademark of Matthes Bender.
 * See clonk_trademark_license.txt for full license.
 */
#include "C4Include.h"
#include <Standard.h>

#ifdef WITH_GLIB
#include <glib.h>
#endif

#ifdef __APPLE__
#include <cstdlib>
#include <string>
#endif

#ifdef _WIN32
#include <shellapi.h>
#endif

// open a weblink in an external browser
bool OpenURL(const char *szURL)
	{
#ifdef _WIN32
	if ((int)ShellExecute(NULL, "open", szURL, NULL, NULL, SW_SHOW) > 32)
		return true;
#endif
#ifdef WITH_GLIB
	const char * argv[][3] = {
		{ "xdg-open", szURL, 0 },
		{ "sensible-browser", szURL, 0 },
		{ "firefox", szURL, 0 },
		{ "mozilla", szURL, 0 },
		{ "konqueror", szURL, 0 },
		{ "epiphany", szURL, 0 },
		{ 0, 0, 0 } };
	for (int i = 0; argv[i][0]; ++i)
		{
		GError * error = 0;
		if (g_spawn_async (g_get_home_dir(), const_cast<char**>(argv[i]), 0, G_SPAWN_SEARCH_PATH, 0, 0, 0, &error))
			return true;
		else fprintf(stderr, "%s\n", error->message);
		}
#endif
#ifdef __APPLE__
	std::string command = std::string("open ") + '"' + szURL + '"';
	std::system(command.c_str());
	return true;
#endif
	// operating system not supported, or all opening method(s) failed
	return false;
	}
