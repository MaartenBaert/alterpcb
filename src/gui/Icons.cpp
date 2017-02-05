/*
Copyright (C) 2016  The AlterPCB team
Contact: Maarten Baert <maarten-baert@hotmail.com>

This file is part of AlterPCB.

AlterPCB is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

AlterPCB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this AlterPCB.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Icons.h"

QIcon g_icon_layermanager_layer_solid, g_icon_layermanager_layer_vstripe_light;
QIcon g_icon_librarymanager_library, g_icon_librarymanager_layout, g_icon_librarymanager_symbol, g_icon_librarymanager_schematic, g_icon_librarymanager_script;
QIcon g_icon_parameterviewer_override_onnormal, g_icon_parameterviewer_override_onmouseover,g_icon_parameterviewer_override_onpressed,g_icon_parameterviewer_override_offnormal,g_icon_parameterviewer_override_offmouseover, g_icon_parameterviewer_override_offpressed, g_icon_parameterviewer_select_normal, g_icon_parameterviewer_select_mouseover, g_icon_parameterviewer_select_pressed, g_icon_parameterviewer_deselect_normal, g_icon_parameterviewer_deselect_mouseover, g_icon_parameterviewer_deselect_pressed;

void LoadIcons() {

	g_icon_layermanager_layer_solid = QIcon(":/layermanager-layer-solid");
	g_icon_layermanager_layer_vstripe_light = QIcon(":/layermanager-layer-vstripe-light");

	g_icon_librarymanager_library = QIcon::fromTheme("document-save");
	g_icon_librarymanager_layout = QIcon::fromTheme("document-open");
	g_icon_librarymanager_symbol = QIcon::fromTheme("document-open");
	g_icon_librarymanager_schematic = QIcon::fromTheme("document-open");
	g_icon_librarymanager_script = QIcon::fromTheme("document-open");

	g_icon_parameterviewer_override_onnormal = QIcon::fromTheme("document-open");
	g_icon_parameterviewer_override_onmouseover = QIcon::fromTheme("document-save");
	g_icon_parameterviewer_override_onpressed = QIcon::fromTheme("document-close");
	g_icon_parameterviewer_override_offnormal = QIcon::fromTheme("document-save");
	g_icon_parameterviewer_override_offmouseover = QIcon::fromTheme("document-open");
	g_icon_parameterviewer_override_offpressed = QIcon::fromTheme("document-close");
	g_icon_parameterviewer_select_normal = QIcon::fromTheme("document-open");
	g_icon_parameterviewer_select_mouseover = QIcon::fromTheme("document-save");
	g_icon_parameterviewer_select_pressed = QIcon::fromTheme("document-open");
	g_icon_parameterviewer_deselect_normal = QIcon::fromTheme("document-save-as");
	g_icon_parameterviewer_deselect_mouseover = QIcon::fromTheme("document-open");
	g_icon_parameterviewer_deselect_pressed = QIcon::fromTheme("document-save");
}
