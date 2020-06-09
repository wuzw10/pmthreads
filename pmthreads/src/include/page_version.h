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
#ifndef __PAGE_VERSION__
#define __PAGE_VERSION__

typedef struct redo_entry{
	int pageNo;
	int version;
}redo_entry_t;
class page_version_manager
{
	public:
		page_version_manager(){
			index = 0;
		}

	void append(int pageNo, int version){
		int r = index;
		entries[r].pageNo = pageNo;
		entries[r].version = version;
		//flush(entries);
		index++;
	}
	
	private:
		int index;
		redo_entry_t entries[1<<16];
};
#endif 
