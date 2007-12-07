#####################################################################
#  $Id$
# 
#  Vidalia is distributed under the following license:
#
#  Copyright (C) 2006-2007,  Matt Edman, Justin Hipple
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
#  02110-1301, USA.
#####################################################################


## Vidalia translation files
qt4_add_translations(vidalia_QMS
  vidalia_ar.ts
  vidalia_bg.ts
  vidalia_cs.ts
  vidalia_de.ts
  vidalia_en.ts
  vidalia_es.ts
  vidalia_fa.ts
  vidalia_fi.ts
  vidalia_fr.ts
  vidalia_hu.ts
  vidalia_it.ts
  vidalia_nb.ts
  vidalia_nl.ts
  vidalia_pl.ts
  vidalia_pt.ts
  vidalia_ru.ts
  vidalia_sv.ts
  vidalia_zh-cn.ts
  vidalia_zh-tw.ts
)
add_custom_target(translations ALL DEPENDS ${vidalia_QMS})

