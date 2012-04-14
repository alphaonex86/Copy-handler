/***************************************************************************
*   Copyright (C) 2001-2008 by Józef Starosczyk                           *
*   ixen@copyhandler.com                                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU Library General Public License          *
*   (version 2) as published by the Free Software Foundation;             *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU Library General Public     *
*   License along with this program; if not, write to the                 *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
#include "stdafx.h"
#include "CfgProperties.h"
#include "../libicpf/config_property.h"

#ifdef  DEBUG
#define new DEBUG NEW
#endif

bool RegisterProperties(icpf::config* pManager)
{
	const long long llHour = 3600UL*1000UL;
//	const long long llMinute = 60UL*1000UL;
	const long long llSecond = 1000UL;

	pManager->register_bool(_t("Program/Enabled clipboard monitoring"), false);
	pManager->register_signed_num(_t("Program/Monitor scan interval"), 1000, 0, llHour);
	pManager->register_bool(_t("Program/Reload after restart"), false);
	pManager->register_unsigned_num(_t("Program/Check for updates frequency"), eFreq_Weekly, eFreq_Never, eFreq_Max - 1);
	pManager->register_bool(_t("Program/Updater checks for beta"), true);

	pManager->register_bool(_t("Program/Shutdown system after finished"), false);
	pManager->register_signed_num(_t("Program/Time before shutdown"), 10000, 0, 24*llHour);
	pManager->register_bool(_t("Program/Force shutdown"), false);
	pManager->register_signed_num(_t("Program/Autosave interval"), 30*llSecond, 0, 24*llHour);
	pManager->register_signed_num(_t("Program/Process priority class"), NORMAL_PRIORITY_CLASS, 0, 0xffffffff);
//	pManager->register_string(_t("Program/Autosave directory"), _t("<APPDATA>\\"), icpf::property::flag_path);
	pManager->register_string(_t("Program/Language"), _t("<PROGRAM>\\Langs\\English.lng"));

	pManager->register_signed_num(_t("Status dialog/Status refresh interval"), 1000, 0, 24*llHour);
	pManager->register_bool(_t("Status dialog/Show details"), true);
	pManager->register_bool(_t("Status dialog/Auto remove finished"), false);

	pManager->register_signed_num(_t("Folder dialog/Dialog width"), -1, -1, 32767);
	pManager->register_signed_num(_t("Folder dialog/Dialog height"), -1, -1, 32767);
	pManager->register_signed_num(_t("Folder dialog/Shortcut list style"), 1, 0, 3);
	pManager->register_bool(_t("Folder dialog/Extended view"), true);
	pManager->register_bool(_t("Folder dialog/Ignore shell dialogs"), false);

	pManager->register_bool(_t("Mini view/Show filenames"), true);
	pManager->register_bool(_t("Mini view/Show single tasks"), true);
	pManager->register_signed_num(_t("Mini view/Miniview refresh interval"), 200, 0, 24*llHour);
	pManager->register_bool(_t("Mini view/Autoshow when run"), true);
	pManager->register_bool(_t("Mini view/Autohide when empty"), true);
	pManager->register_bool(_t("Mini view/Use smooth progress"), true);

	pManager->register_bool(_t("Copying/Use auto-complete files"), true);
	pManager->register_bool(_t("Copying/Always set destination attributes"), true);
	pManager->register_bool(_t("Copying/Always set destination time"), true);
	pManager->register_bool(_t("Copying/Protect read-only files"), false);
	pManager->register_signed_num(_t("Copying/Limit max operations"), 1, 0, 0xffffffff);
	pManager->register_bool(_t("Copying/Read tasks size before blocking"), true);
	pManager->register_signed_num(_t("Copying/Show visual feedback"), 2, 0, 2);
	pManager->register_bool(_t("Copying/Use timed feedback dialogs"), false);
	pManager->register_signed_num(_t("Copying/Feedback time"), 60*llSecond, 0, 24*llHour);
	pManager->register_bool(_t("Copying/Auto retry on error"), true);
	pManager->register_signed_num(_t("Copying/Auto retry interval"), 10*llSecond, 0, 24*llHour);
	pManager->register_signed_num(_t("Copying/Default priority"), THREAD_PRIORITY_NORMAL, 0, 0xffffffff);
	pManager->register_bool(_t("Copying/Disable priority boost"), false);
	pManager->register_bool(_t("Copying/Delete files after finished"), true);
	pManager->register_bool(_t("Copying/Create log file"), true);

	pManager->register_bool(_t("Shell/Show 'Copy' command"), true);
	pManager->register_bool(_t("Shell/Show 'Move' command"), true);
	pManager->register_bool(_t("Shell/Show 'Copy,move special' command"), true);
	pManager->register_bool(_t("Shell/Show 'Paste' command"), true);
	pManager->register_bool(_t("Shell/Show 'Paste special' command"), true);
	pManager->register_bool(_t("Shell/Show 'Copy to' command"), true);
	pManager->register_bool(_t("Shell/Show 'Move to' command"), true);
	pManager->register_bool(_t("Shell/Show 'Copy to,Move to special' command"), true);
	pManager->register_bool(_t("Shell/Show free space along with shortcut"), true);
	pManager->register_bool(_t("Shell/Show shell icons in shortcuts menu"), false);
	pManager->register_bool(_t("Shell/Intercept drag&drop"), true);
	pManager->register_bool(_t("Shell/Intercept keyboard actions"), true);
	pManager->register_bool(_t("Shell/Intercept context menu actions"), false);

	pManager->register_bool(_t("Buffer/Use only default buffer"), false);
	pManager->register_signed_num(_t("Buffer/Default buffer size"), 2097152, 1, 0xffffffff);
	pManager->register_signed_num(_t("Buffer/One physical disk buffer size"), 4194304, 1, 0xffffffff);
	pManager->register_signed_num(_t("Buffer/Two different hard disks buffer size"), 524288, 1, 0xffffffff);
	pManager->register_signed_num(_t("Buffer/CD buffer size"), 262144, 1, 0xffffffff);
	pManager->register_signed_num(_t("Buffer/LAN buffer size"), 131072, 1, 0xffffffff);
	pManager->register_bool(_t("Buffer/Use no buffering for large files"), true);
	pManager->register_signed_num(_t("Buffer/Large files lower boundary limit"), 2097152, 1, 0xffffffff);

	pManager->register_bool(_t("Log file/Enable logging"), true);
	pManager->register_signed_num(_t("Log file/Max log size limit"), 512384, 1024, 0xffffffff);
	pManager->register_unsigned_num(_t("Log file/Logging level"), 1, 0, 3);		// 0=debug, 1=info, 2=warning, 3=error

	pManager->register_bool(_t("Sounds/Play sounds"), true);
	pManager->register_string(_t("Sounds/Error sound path"), _t("<WINDOWS>\\media\\chord.wav"), icpf::property::flag_path);
	pManager->register_string(_t("Sounds/Finished sound path"), _t("<WINDOWS>\\media\\ding.wav"), icpf::property::flag_path);

	pManager->register_string(_t("Shortcuts/Shortcut"), _t(""), icpf::property::flag_array);
	pManager->register_string(_t("Recent paths/Path"), _t(""), icpf::property::flag_array);

	// invisible options
	pManager->register_unsigned_num(_t("Runtime state/Last checked for updates"), 0, 0, ULLONG_MAX);

	return true;
}
